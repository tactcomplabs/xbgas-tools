// See LICENSE for license details.

#include "mmu.h"
#include "sim.h"
#include "processor.h"
#include <mpi.h>
#include <iostream>
//#define DEBUG
#undef DEBUG

int64_t amo_insn = 0;
int64_t amo_add = 0;
int64_t amo_and = 0;
int64_t amo_or = 0;
int64_t amo_xor = 0;
int64_t amo_max = 0;
int64_t amo_min = 0;
int64_t amo_swap = 0;



int64_t		EAG_ne			= 0;
uint64_t	EAG_addr 		= 0;
int64_t		EAG_stride 	= 0; // Curently Not Supported due to the MPI Simulation Interface
int64_t		EAG_flag		= 0;
int64_t		EAG_debug		= 0;
char*  		agg_buf			= NULL;

int64_t   trap_flag		= 0;
reg_t			trap_addr   = 0;

extern int64_t inst;
extern int64_t insn_check;
int64_t check_accum = 0;
int64_t check_buf = 0;
int64_t ic_check = 0;
enum xbgas_amo{
	xbgas_add,
	xbgas_xor,
	xbgas_and,
	xbgas_or,
	xbgas_max,
	xbgas_min,
	xbgas_cas,
	xbgas_unknown
};

xbgas_amo strhash(std::string const& inString) {
	    if (inString == "ADD") return xbgas_add;
			else if (inString == "MAX") return xbgas_max;
			else if (inString == "MIN") return xbgas_min;
			else if (inString == "AND") return xbgas_and;
			else if (inString == "XOR") return xbgas_xor;
			else if (inString == "OR") return xbgas_or;
			else if (inString == "CAS") return xbgas_cas;

			// error type
		  return xbgas_unknown;
}

mmu_t::mmu_t(sim_t* sim, processor_t* proc)
 : sim(sim), proc(proc),
  check_triggers_fetch(false),
  check_triggers_load(false),
  check_triggers_store(false),
  matched_trigger(NULL)
{
  flush_tlb();
}

mmu_t::~mmu_t()
{
	//print_stat();
	//if(inst)
		//std::cout  << "Exec Instruction Cnt = " <<inst << std::endl;
	//inst = 0;
}

// xBGAS Extensions
bool mmu_t::set_xbgas(){
  xbgas_enable = true;
  return true;
}

/*void mmu_t::print_stat()
{
	//std::cout << "Core "<< proc->get_id() << ", Instruction Cnt = " <<insn_cnt << std::endl;
	std::cout  << "Instruction Cnt = " <<insn_cnt << std::endl;

}*/


void mmu_t::flush_icache()
{
  for (size_t i = 0; i < ICACHE_ENTRIES; i++)
    icache[i].tag = -1;
}

void mmu_t::flush_tlb()
{
  memset(tlb_insn_tag, -1, sizeof(tlb_insn_tag));
  memset(tlb_load_tag, -1, sizeof(tlb_load_tag));
  memset(tlb_store_tag, -1, sizeof(tlb_store_tag));

  flush_icache();
}

reg_t mmu_t::translate(reg_t addr, access_type type)
{
  if (!proc)
    return addr;
	//std::cout << "DEBUG:: Entering translate() function\n";
  reg_t mode = proc->state.prv;
  if (type != FETCH) {
    if (!proc->state.dcsr.cause && get_field(proc->state.mstatus, MSTATUS_MPRV))
      mode = get_field(proc->state.mstatus, MSTATUS_MPP);
  }

 reg_t page = 	walk(addr, type, mode);
#ifdef DEBUG
			if(EAG_flag)
			{
				std::cout << "DEBUG::  Translated physical address is 0x" << std::hex << (page | (addr & (PGSIZE-1))) << std::endl;
				std::cout << "DEBUG::  leaving walk() function\n";
			}
#endif
  //return walk(addr, type, mode) | (addr & (PGSIZE-1)); // base PPN | PG offset
  return page | (addr & (PGSIZE-1)); // base PPN | PG offset
}
//
// reg_t mmu_t::translate_remote(reg_t addr)
// {
//   if (!proc)
//     return addr;
// 	//std::cout << "DEBUG:: Entering translate() function\n";
//    reg_t mode = proc->state.prv;
//   // if (type != FETCH) {
//   //   if (!proc->state.dcsr.cause && get_field(proc->state.mstatus, MSTATUS_MPRV))
//   //     mode = get_field(proc->state.mstatus, MSTATUS_MPP);
//   // }
//
//  reg_t page = 	walk_remote(addr, mode);
// 			if(EAG_flag)
// 			{
// 				std::cout << "DEBUG::  Translated physical address is 0x" << std::hex << (page | (addr & (PGSIZE-1))) << std::endl;
// 				std::cout << "DEBUG::  leaving walk() function\n";
// 			}
//   //return walk(addr, type, mode) | (addr & (PGSIZE-1)); // base PPN | PG offset
//   return page | (addr & (PGSIZE-1)); // base PPN | PG offset
// }





tlb_entry_t mmu_t::fetch_slow_path(reg_t vaddr)
{
  reg_t paddr = translate(vaddr, FETCH);
  // host_addr is the real address (va) in the allocted memory
  if (auto host_addr = sim->addr_to_mem(paddr)) {
		if(EAG_debug)
			std::cout << "DEBUG:: Thread " <<sim->myid <<", insn_vaddr = "<<std::hex
			<<vaddr<< ", address translation succeeded\n";
    return refill_tlb(vaddr, paddr, host_addr, FETCH);
  } else {
		if(EAG_debug)
			std::cout << "DEBUG:: Thread " <<sim->myid <<", insn_vaddr = "<<std::hex
			<<vaddr<< ", address translation failed\n";
    if (!sim->mmio_load(paddr, sizeof fetch_temp, (uint8_t*)&fetch_temp))
      throw trap_instruction_access_fault(vaddr);
    tlb_entry_t entry = {(char*)&fetch_temp - vaddr, paddr - vaddr};
    return entry;
  }
}

reg_t reg_from_bytes(size_t len, const uint8_t* bytes)
{
  switch (len) {
    case 1:
      return bytes[0];
    case 2:
      return bytes[0] |
        (((reg_t) bytes[1]) << 8);
    case 4:
      return bytes[0] |
        (((reg_t) bytes[1]) << 8) |
        (((reg_t) bytes[2]) << 16) |
        (((reg_t) bytes[3]) << 24);
    case 8:
      return bytes[0] |
        (((reg_t) bytes[1]) << 8) |
        (((reg_t) bytes[2]) << 16) |
        (((reg_t) bytes[3]) << 24) |
        (((reg_t) bytes[4]) << 32) |
        (((reg_t) bytes[5]) << 40) |
        (((reg_t) bytes[6]) << 48) |
        (((reg_t) bytes[7]) << 56);
  }
  abort();
}


void mmu_t::remote_amo(int64_t target, reg_t addr, reg_t len, uint8_t* bytes, std::string op, uint8_t* results)
{
	//std::cout << "start the remote amo, operation is "<<op<< ", request size is " << len<< " bytes" << std::endl;
	reg_t paddr = translate(addr, LOAD);
	auto host_addr = sim->addr_to_mem(paddr);
	uint64_t addr_offset = host_addr - (sim->mems[0].second->contents());
#ifdef DEBUG
	if (target == sim->myid)
		target = 1 - target;
	printf("remote target = %ld, local addr = %lu, local var = %lu, RS2 = %lu\n", target, addr, *(uint64_t*)host_addr, *bytes);
#endif
  MPI_Win_lock_all(0, sim->win);
	switch (strhash(op))
	{
		case xbgas_add:
		// remote fetch and add
		  if(len == 4)
				MPI_Fetch_and_op(bytes, results, MPI_UINT32_T, target, (MPI_Aint)addr_offset,
				MPI_SUM,sim->win);
			else{
				//printf("get into case eamoadd.d\n");
				MPI_Fetch_and_op(bytes, results, MPI_UINT64_T, target, (MPI_Aint)addr_offset,
				MPI_SUM,sim->win);}
			break;
		case xbgas_and:
		// remote fetch and and
			if(len == 4)
				MPI_Fetch_and_op(bytes, results, MPI_UINT32_T, target, (MPI_Aint)addr_offset,
			 	MPI_BAND,sim->win);
			else
			 	MPI_Fetch_and_op(bytes, results, MPI_UINT64_T, target, (MPI_Aint)addr_offset,
			 	MPI_BAND,sim->win);
			break;
		case xbgas_xor:
		// remote fetch and and
			if(len == 4)
				MPI_Fetch_and_op(bytes, results, MPI_UINT32_T, target, (MPI_Aint)addr_offset,
				MPI_BXOR,sim->win);
			else
				MPI_Fetch_and_op(bytes, results, MPI_UINT64_T, target, (MPI_Aint)addr_offset,
				MPI_BXOR,sim->win);

			break;
		case xbgas_or:
			if( len ==4 )
		// remote fetch and and
				MPI_Fetch_and_op(bytes, results, MPI_UINT32_T, target, (MPI_Aint)addr_offset,
				MPI_BOR,sim->win);
			else
				MPI_Fetch_and_op(bytes, results, MPI_UINT64_T, target, (MPI_Aint)addr_offset,
				MPI_BOR,sim->win);
			break;

		case xbgas_max:
		// remote fetch and max
			if(len == 4)
				MPI_Fetch_and_op(bytes, results, MPI_UINT32_T, target, (MPI_Aint)addr_offset,
				MPI_MAX,sim->win);
			else
				MPI_Fetch_and_op(bytes, results, MPI_UINT64_T, target, (MPI_Aint)addr_offset,
				MPI_MAX,sim->win);

			break;

		case xbgas_min:
		// remote fetch and min
			if( len == 4 )
				MPI_Fetch_and_op(bytes, results, MPI_UINT32_T, target, (MPI_Aint)addr_offset,
				MPI_MIN,sim->win);
			else
				MPI_Fetch_and_op(bytes, results, MPI_UINT64_T, target, (MPI_Aint)addr_offset,
				MPI_MIN,sim->win);
			break;

		case xbgas_cas:
		// remote fetch and and
			if( len == 4)
				MPI_Compare_and_swap(host_addr, bytes, results, MPI_UINT32_T, target,
				(MPI_Aint)addr_offset,sim->win);
			else
				MPI_Compare_and_swap(host_addr, bytes, results, MPI_UINT64_T, target,
				(MPI_Aint)addr_offset,sim->win);

			break;
		case xbgas_unknown:
			// Invalid operation type
			std::cout << "Error: Invalid Remote Atomic Operation " << op << std::endl;
	}

		MPI_Win_flush(target, sim->win);
		//MPI_Win_flush(sim->myid, sim->win);

    MPI_Win_unlock_all(sim->win);

#ifdef DEBUG
		printf("Returned Value = %lu\n", *results);
#endif
}




void mmu_t::store_remote_path(int64_t target, reg_t addr,
                              reg_t len, uint8_t* bytes )
{

	// resume the rest remote operations from traps
	if(EAG_flag && trap_flag){
		addr = trap_addr;
		trap_flag = 0;
		trap_addr = 0;
	}



#ifdef DEBUG
  std::cout << "DEBUG::  Target ID = "
            << target << " Local Addr = " <<std::hex <<addr << " value = "
            << std::dec<<(uint64_t)(*bytes) << std::endl;
#endif
  int rank			=	sim->myid;
	int same_addr = 0;

	// dest and src base address are identical
	// implying the same dest and src buffer
	// thus, can avoid redundant translations
	if(EAG_addr == addr)
		same_addr = 1;

  // Temporarily go through the MMU address translation
	//load_slow_path(addr, len, bytes);
  reg_t paddr = translate(addr, LOAD);
  auto host_addr = sim->addr_to_mem(paddr);
	auto src_addr 	= host_addr;

  //std::cout << "Thread " << rank << " xbgas store starts\n";
	// Check if aggregation is enabled

	//reg_t rest				= EAG_ne;
	reg_t copy_ne			= 0;
	//load_slow_path(addr, len, bytes);
	if(EAG_flag){

		if(!same_addr)
			src_addr 	= sim->addr_to_mem(translate(EAG_addr,LOAD));

		std::cout << "DEBUG::  Thread " << rank << ", Aggregation Stores\n";
		// src_addr 	= sim->addr_to_mem(translate(EAG_addr,LOAD));
		// len 					= 	EAG_ne*len;
		// Copy the data from one or multiple pages into the buffer
		//if((reg_t)src_addr + (int64_t)len <= ((reg_t)src_addr|0xfff))
		//	bytes 			= 	(uint8_t*)src_addr;
		//else{
// 			buffer 			= 	(uint8_t*)malloc(sizeof(uint8_t)*len);
// 			buf_p				=		buffer;
// 		 	//bytes 			= 	buffer;
// 			std::cout << "DEBUG::  Thread " << rank << ", Aggregation Stores\n";
// 			if(!buffer)
// 				std::cout << "allocation failed\n";
//   		//std::cout << "Thread " << rank << " allocate the buffer for aggregation with size  "
//         //    << sizeof(uint8_t)*len
//           //  <<"\n";
// //#ifdef DEBUG
//   		std::cout << "Thread " << rank << " allocate the buffer for aggregation with size  "
//             << std::dec<< sizeof(uint8_t)*len
//             <<"\n";
// //#endif
// 			std::cout<< "DEBUG::  Thread " << rank << " Buffer Address Range [0x"
// 			         << std::hex << (reg_t)buffer << " ~ 0x" << (reg_t)(&buffer[len-1])<< "]\n";
// 		  //copy_ne = 	((reg_t)src_addr|0xfff) - (reg_t)src_addr + 1;
// 		  copy_ne = 	((reg_t)EAG_addr|0xfff) - (reg_t)EAG_addr + 1;
// 			if(copy_ne > len)
// 				copy_ne = len;
// 			rest    = 	len - copy_ne;
// 			std::cout << "DEBUG::  Thread " << rank << " starting the first memcpy\n";
// 			std::cout << "DEBUG::  Thread  "
// 				          << rank << " EAG_addr = 0x" <<std::hex << EAG_addr << ", src_addr = 0x"
// 			            << (reg_t)src_addr << ", buffer_addr = 0x" << (reg_t)buffer<<std::endl;
// 			memcpy( buffer, src_addr, sizeof(uint8_t)*copy_ne);
// 			//EAG_debug = 1;
// 			flush_tlb();
// 			std::cout << "DEBUG::  Thread " <<std::dec << rank << " completing the first memcpy, copied ne = "
// 								<< copy_ne << ", rest ne = " << rest <<std::endl;
// 			EAG_addr 		+= 	copy_ne;
// 			buffer    	+= 	copy_ne;
// 			// if(rest>=4096)
// 			// 	std::cout << "DEBUG::  Thread "<< rank << " starting the copy in loop \n";
// 			// while(rest>=4096){
// 			// 	std::cout << "DEBUG::  Thread "<< rank << " translating address \n";
// 			// 	src_addr 	= 	sim->addr_to_mem(translate(EAG_addr, LOAD));
// 			// 	std::cout << "DEBUG::  Thread "<< rank << " translation completes \n";
// 			// 	std::cout << "DEBUG::  Thread  "
// 			// 	          << rank << " EAG_addr = 0x" <<std::hex << EAG_addr << ", src_addr = 0x"
// 			//             << (reg_t)src_addr << ", buffer_addr = 0x" << (reg_t)buffer<<std::endl;
// 			//
// 			// 	std::cout << "DEBUG::  Thread " << rank << " starting the memcpy in loop\n";
// 			// 	memcpy(buffer, src_addr, sizeof(uint8_t)*4096);
// 			// 	flush_tlb();
// 			// 	std::cout << "DEBUG::  Thread " << rank << " completing the memcpy in loop\n";
// 			// 	rest    	= 	rest - 4096;
// 			// 	EAG_addr 	+= 	4096;
// 			// 	buffer    += 	4096;
// 			// 	std::cout << "DEBUG::  Thread " << rank << " rest = " <<std::dec<< rest <<"\n";
// 			// }
//
//
// 			if(rest > 0){
// 				std::cout << "DEBUG::  LAST COPY - Thread "
// 				          << rank << " EAG_addr = 0x" <<std::hex << EAG_addr << ", src_addr = 0x"
// 			            << (reg_t)src_addr << ", buffer_addr = 0x" << (reg_t)buffer<<std::endl;
// 				std::cout << "DEBUG::  Thread "<< rank << " translating address \n";
// 				src_addr 	= 	sim->addr_to_mem(translate(EAG_addr, LOAD));
// 				std::cout << "DEBUG::  Thread "<< rank << " translation completes \n";
// 				std::cout << "DEBUG::  Thread "<< rank << " starting the last memcopy \n";
// 				memcpy(buffer, src_addr, rest);
// 				flush_tlb();
// 				std::cout << "DEBUG::  Thread "<< rank << " completed the last memcopy \n";
// 			}
//
// 			std::cout << "DEBUG::  Thread " << rank << " aggregated store completes local memcpy\n ";
// 		//}

	}




#ifdef DEBUG
  std::cout << "DEBUG:: Thread " << rank << ", Host Addr = 0x"
            << std::hex << (reg_t)host_addr
            << ", device addr = "
            << (reg_t)(sim->mems[0].second->contents()) <<"\n";
  std::cout << "Thread " << rank << ", address offset = 0x"
            << (reg_t)host_addr - (reg_t)(sim->mems[0].second->contents())
            <<"\n";
#endif

  //MPI_Win_fence(0, sim->win);
  //COMMENTED if condition for one-side communications
  //if( rank == target ){

#ifdef DEBUG
//    std::cout << "DEBUG::  Thread " << rank << " executing the xbgas store\n";
#endif
  //}else{ //Requster thread
#ifdef DEBUG
    std::cout << "DEBUG:: Thread " << rank << " executing the xbgas store\n";
#endif
		uint64_t buf = 66;
		uint64_t addr_offset = host_addr - (sim->mems[0].second->contents());
		//std::cout << "DEBUG:: Thread " << rank << " address offset = "<< std::hex << addr_offset <<std::endl;
#ifdef DEBUG
    std::cout << "DEBUG:: Thread " << rank << " acquiring the lock\n";
#endif
//    MPI_Win_lock( MPI_LOCK_EXCLUSIVE, target, 0, sim->win);
#ifdef DEBUG
    std::cout << "DEBUG:: Thread " << rank << " acquired the lock, sending Put\n";
#endif

	// Due to the page table mapping, we have to break MPI transactions into page-aligned trunks
	if(EAG_flag){

			while(EAG_ne)
			{

				//std::cout << "DEBUG:: \033[1m\033[31m Thread "<< rank << " translating src address \x1B[0m \n";
				//load_slow_path(EAG_addr, len, bytes);
				//*bytes = host_addr;
				//*bytes = load_int32(EAG_addr);

				// update src and host_addr in each itereation
				//std::cout << "DEBUG:: \033[1m\033[31m Thread "<< rank << " translating src address \x1B[0m \n";
				//src_addr 	= sim->addr_to_mem(translate(EAG_addr,STORE));
				//std::cout << "DEBUG::  Thread "<< rank << " translating address completes \n";
    		MPI_Win_lock_all(0, sim->win);
   			if(MPI_SUCCESS != MPI_Put(/*(uint8_t*)bytes,*/ (uint8_t*)src_addr,  len, MPI_UINT8_T, target,
            (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),len, MPI_UINT8_T, sim->win))
				printf( "\033[1m\033[31m SPIKE: MPI_PUT FAILED\n \x1B[0m");
				//std::cout << "DEBUG::  Thread "<< rank << " MPI_put completes \n";
				MPI_Win_flush(target, sim->win);
    		MPI_Win_unlock_all(sim->win);

				EAG_ne--;
				if(EAG_ne){

					//host_addr = sim->addr_to_mem(translate_remote(addr));
					//host_addr = sim->addr_to_mem(translate(addr, LOAD));
					std::cout << "DEBUG::  Thread "		<< rank 
										<< ", Target  = "				<< target 
										<< ", Rest elements = "	<< std::dec 
										<< EAG_ne << "\n";
					
					// agg_buf = NULL;
					addr+=len;
					EAG_addr+=len;

					// record the remote base addr in case of traps
					trap_addr = addr;

					// Translate remote base address when crossing the page boundary
					// We assume the requests are well-aligned
					if((addr&0xfff) != 0)
						host_addr	+=len;
					else{
						std::cout << "DEBUG::  \033[1m\033[32m translate remote base address \x1B[0m \n";
						host_addr = sim->addr_to_mem(translate(addr, LOAD));
						std::cout << "DEBUG::  \033[1m\033[34m Translation completes \x1B[0m \n";
					}

					// If src & dest addr are not the identical
					if(!same_addr){
						// Translate local src address when crossing the page boundary
						// We assume the requests are well-aligned
						if((EAG_addr&0xfff) != 0)
							src_addr	+=len;
						else{
							std::cout << "DEBUG::  \033[1m\033[32m translate local src address \x1B[0m \n";
							src_addr 	= sim->addr_to_mem(translate(EAG_addr,LOAD));
							std::cout << "DEBUG::  \033[1m\033[34m Translation completes \x1B[0m \n";
						}
					}
					else
						src_addr = host_addr;

				//load_slow_path(addr, len, bytes);
				// if(agg_buf)
				// 	host_addr = agg_buf;
				// else{
				// 	std::cout << "DEBUG::  \033[1m\033[31m Invalid real address, translating again \x1B[0m\n";
				// 	std::cout << "DEBUG::  \033[1m\033[32m Thread "<< rank << " translating remote base address = 0x"<<std::hex<<addr <<" \x1B[0m \n";
				// 	host_addr = sim->addr_to_mem(translate(addr, LOAD));
				// 	std::cout << "DEBUG::  Thread "<< rank << " translating address completes \n";
				//
				// }
				}

			}


#ifdef DEBUG
			std::cout <<"DEBUG:: Thread " << rank <<" Aggrgeated Put completes\n";
#endif

	}
	else{

    MPI_Win_lock_all(0, sim->win);
   if(MPI_SUCCESS != MPI_Put((uint8_t*)bytes, len, MPI_UINT8_T, target,
            (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),len, MPI_UINT8_T, sim->win))
			printf( "\033[1m\033[31m SPIKE: MPI_PUT FAILED\n \x1B[0m");
    if(MPI_SUCCESS != MPI_Win_flush(target, sim->win))
			printf( "\033[1m\033[31m SPIKE: MPI_FLUSH FAILED\n \x1B[0m");
    MPI_Win_unlock_all(sim->win);
	}
#if 0
    if(MPI_SUCCESS != MPI_Accumulate(bytes, len, MPI_UINT8_T, target,
            (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
            len, MPI_UINT8_T, MPI_REPLACE, sim->win))
			printf( "\033[1m\033[31m SPIKE: MPI_PUT FAILED\n \x1B[0m");
#endif

    //MPI_Win_flush_all(sim->win);


#if 0
		// Check the stored value
		MPI_Get((uint8_t*)&buf, len, MPI_UINT8_T, target,
						(MPI_Aint)addr_offset,len, MPI_UINT8_T, sim->win);
    if(MPI_SUCCESS != MPI_Win_flush(target, sim->win))
			printf( "\033[1m\033[31m SPIKE: MPI_FLUSH FAILED\n \x1B[0m");
#endif
#ifdef DEBUG
    std::cout << "DEBUG:: Thread " << rank << " Put complete; unlocking\n";
#endif
    //MPI_Win_unlock(target, sim->win);
#ifdef DEBUG
		std::cout << "DEBUG:: Thread " << rank << " address offset = "<< std::hex << addr_offset <<std::endl;
    std::cout << "DEBUG:: Thread " << rank << " remote value = " <<std::hex<< (uint64_t)(buf)
    					<< " stored value = "<<(uint64_t)(*bytes) << "\n";
    std::cout << "DEBUG:: Thread " << rank << " unlock complete\n";

#endif

		// Reset EAG status
		if(EAG_flag){
			EAG_addr 	= 0;
			EAG_flag 	= 0;
			EAG_ne	 	= 0;
			std::cout << "DEBUG::  Thread " << rank << ", Aggregation Store Completes\n";
			//agg_buf   = NULL;
		}

		//std::cout << "DEBUG:: Thread " << rank << " REMOTE STORE COMPLETES\n";




    //MPI_Request put_req;
    //MPI_Win_lock_all(0, sim->win);
    //MPI_Rput(bytes, len, MPI_UINT8_T, target,
    //        (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
    //        len, MPI_UINT8_T, sim->win, &put_req);
    //MPI_Win_unlock_all(sim->win);
  //}

  //MPI_Win_fence(0, sim->win);
  //MPI_Barrier(MPI_COMM_WORLD);
#ifdef DEBUG
  std::cout << "DEBUG::  Thread " << rank << " completed the xbgas store\n";
#endif
}

// NOTE: currently used to trasnfer a "single" data element between two threads
//       based on the upper 64-bit address stored in OLB
void mmu_t::load_remote_path(int64_t target, reg_t addr,
                             reg_t len, uint8_t* bytes)
{
	// resume the rest remote operations from traps
	if(EAG_flag && trap_flag){
		addr = trap_addr;
		trap_flag = 0;
		trap_addr = 0;
	}

  int rank			=	sim->myid;
	int same_addr = 0;
  MPI_Status status;
  // Temporarily go through the MMU address translation
  reg_t paddr = translate(addr, LOAD);
  auto host_addr = sim->addr_to_mem(paddr);
#ifdef DEBUG
  std::cout << "DEBUG:: Thread " << rank << ", Host Addr = "<< (reg_t)host_addr
            << ", device addr = "
            << (reg_t)(sim->mems[0].second->contents()) <<"\n";
  std::cout << "Thread " << rank << " sent the address offset: "
            << (reg_t)host_addr - (reg_t)(sim->mems[0].second->contents())
            <<"\n";
#endif

	if(EAG_addr == addr)
		same_addr = 1;



	auto 			dest_addr 	= host_addr;
	//int64_t   EAG_addr_copy = EAG_addr;
	reg_t			copy_ne			= 0;
	//reg_t			rest 				= EAG_ne;
	// Check if aggregation is enabled
	if(EAG_flag){
		// record the remote base addr in case of traps
		trap_addr = addr;

		std::cout << "DEBUG::  Thread " << rank << ", Aggregation Loads\n";
		if(!same_addr)
       dest_addr  = sim->addr_to_mem(translate(EAG_addr,STORE));
		// dest_addr 	= sim->addr_to_mem(translate(EAG_addr, STORE));
		// len 					= EAG_ne*len; // aggregated size in bytes
		// buffer 			= (uint8_t*)malloc(sizeof(uint8_t)*len);
		// std::cout<< "Thread " << rank << ", LEN = "<< len << ", EAG_ne = " << EAG_ne <<"\n";
  	// 	std::cout << "Thread " << rank << " allocate the buffer for aggregation with size  "
    //         << len
    //         <<"\n";
#ifdef DEBUG
		std::cout<< "Thread " << rank << ", LEN = "<< len << ", EAG_ne = " << EAG_ne <<"\n";
  		std::cout << "Thread " << rank << " allocate the buffer for aggregation with size  "
            << len
            <<"\n";
#endif


	}




#ifdef DEBUG
    std::cout << "DEBUG::  Thread " << rank << " executing the xbgas load\n";
#endif
    //MPI_Recv(&offset, 1, MPI_UINT64_T, sim->world_size - target - 1, 0, MPI_COMM_WORLD, &status);
    //std::cout << "Target Thread " << rank << " received the address offset: "<< offset <<" \n";
    //MPI_Send((uint8_t*)(sim->x_mem.first + offset), len, MPI_UINT8_T, sim->world_size - target - 1, 1, MPI_COMM_WORLD);
    //MPI_Send((uint8_t*)(host_addr), len, MPI_UINT8_T, sim->world_size - target - 1, 1, MPI_COMM_WORLD);

  //}else{ // Requestor Threads
    std::pair<reg_t, reg_t> message;
    message = std::make_pair(len, addr);
#ifdef DEBUG
    std::cout << "DEBUG::  Thread " << rank << " executing the xbgas load\n";
#endif
    //MPI_Request send_req;
    //MPI_Send(&message.second, 1, MPI_UINT64_T, target, 0, MPI_COMM_WORLD);
    //MPI_Recv(bytes, len, MPI_UINT8_T, target, 1, MPI_COMM_WORLD, &status );

    //MPI_Win_lock(MPI_LOCK_SHARED, target, 0, sim->win);
    //MPI_Win_lock( MPI_LOCK_EXCLUSIVE, target, 0, sim->win);
    //MPI_Win_lock( target, sim->win);

#ifdef DEBUG
    std::cout << "DEBUG:: Thread " << rank << " acquired the lock; executing Get\n";
#endif
//#if 0

		if(EAG_flag){

			while(EAG_ne)
			{
				std::cout << "DEBUG::  Rest elements =  "<< std::dec<< EAG_ne << "\n";
				
				// update dest_addr and host_addr in each itereation
				MPI_Win_lock_all(0, sim->win);
				MPI_Get(dest_addr, len, MPI_UINT8_T, target,
						(MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
						len, MPI_UINT8_T, sim->win);
				MPI_Win_flush(target,sim->win);
				MPI_Win_unlock_all(sim->win);



				EAG_ne--;
				addr			+=len;
				EAG_addr	+=len;

				// record the remote base addr in case of traps
				trap_addr = addr;

				if((addr&0xfff) != 0)
					host_addr	+= len;
				else{
					std::cout << "DEBUG::  \033[1m\033[32m translate remote base address \x1B[0m \n";
					host_addr = sim->addr_to_mem(translate(addr, LOAD));
					std::cout << "DEBUG::  \033[1m\033[34m Translation completes \x1B[0m \n";
				}

				// If src & dest addr are not identical
				if(!same_addr){
					// Translate local dest address when crossing the page boundary
					// We assume the requests are well-aligned
					if((EAG_addr&0xfff) != 0)
						dest_addr	+= len;
					else{
						std::cout << "DEBUG::  \033[1m\033[32m translate local dest address \x1B[0m \n";
						dest_addr 	= sim->addr_to_mem(translate(EAG_addr,STORE));
						std::cout << "DEBUG::  \033[1m\033[34m Translation completes \x1B[0m \n";
					}
				}
				else
					dest_addr = host_addr;


				// dest_addr 	= sim->addr_to_mem(translate(EAG_addr, STORE));
				// host_addr 	= sim->addr_to_mem(translate(addr,LOAD));
			}
			EAG_addr 			= 0;
			EAG_flag 			= 0;
			EAG_ne	 			= 0;
			//agg_buf				= NULL;
			std::cout << "DEBUG::  Thread " << rank << ", Aggregation Load Completes\n";
	 }
	 else{


		MPI_Win_lock_all(0, sim->win);
    MPI_Get(bytes, len, MPI_UINT8_T, target,
            (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
            len, MPI_UINT8_T, sim->win);
		MPI_Win_flush(target,sim->win);
		MPI_Win_unlock_all(sim->win);
	 }
//#endif
#if 0
    uint8_t *result = NULL;
    result = new uint8_t[len];
    MPI_Get_accumulate(bytes, len, MPI_UINT8_T,
            result, len, MPI_UINT8_T,
            target,
            (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
            len, MPI_UINT8_T, MPI_NO_OP, sim->win);
    std::cout << "DEBUG:: Thread " << rank << " completed the Get; releasing the lock\n";
    delete result;
#endif
    //MPI_Win_unlock(target, sim->win);

		//MPI_Win_unlock( target, sim->win);
#ifdef DEBUG
    std::cout << "DEBUG:: Thread " << rank << " released the lock\n";
#endif
	 // If data size is larger than page size, then we need to copy data in each page
	 //if((reg_t)dest_addr + (int64_t)len > ((reg_t)dest_addr|0xfff)){
		// if(unlikely(EAG_flag)){
		//   //copy_ne = 	((reg_t)dest_addr|0xfff) - (reg_t)dest_addr;
		//   copy_ne = 	((reg_t)EAG_addr|0xfff) - (reg_t)EAG_addr;
		// 	if(copy_ne > len)
		// 		copy_ne = len;
		//   rest		    = 	len - copy_ne;
		// 	//EAG_addr		= 	EAG_addr_copy;
		// 	buffer			=   buf_p;
		// 	memcpy(dest_addr, buffer, copy_ne);
		// 	EAG_addr 		+= 	copy_ne;
		// 	buffer    	+= 	copy_ne;
		//
		// 	while(rest>=4096){
		// 		dest_addr = 	sim->addr_to_mem(translate(EAG_addr, STORE));
		// 		memcpy(dest_addr,buffer, 4096);
		// 		rest    	= 	rest - 4096;
		// 		EAG_addr 	+= 	4096;
		// 		buffer    += 	4096;
		// 	}
		//
		// 	if(rest > 0){
		// 		dest_addr = 	sim->addr_to_mem(translate(EAG_addr, STORE));
		// 		memcpy(dest_addr, buffer, rest);
		// 	}


			//reset aggregation status

	 //}




    //MPI_Request get_req;
    //MPI_Win_lock_all(0, sim->win);
    //MPI_Rget(bytes, len, MPI_UINT8_T, target,
    //        (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
    //        len, MPI_UINT8_T, sim->win, &get_req);
    //MPI_Wait(&get_req,MPI_STATUS_IGNORE);
    //MPI_Win_unlock_all(sim->win);

  //}

  //MPI_Win_fence(0, sim->win);
  //MPI_Barrier(MPI_COMM_WORLD);
#ifdef DEBUG
  std::cout << "DEBUG::  Thread " << rank << " completes the xbgas load\n";
#endif
}

void mmu_t::load_slow_path(reg_t addr, reg_t len, uint8_t* bytes)
{
	/*if (addr == 0xAAAAAAAAAAAAAAAA || addr == 0xBBBBBBBBBBBBBBBB){

		MPI_Barrier(MPI_COMM_WORLD);
		std::cout <<"Rank " << sim->myid << " comletes MPI_Barrier\n";
		//rintf("Rank %d comletes MPI_Barrier\n",sim->myid);
		return;
	}*/
  reg_t paddr = translate(addr, LOAD);

  if (auto host_addr = sim->addr_to_mem(paddr)) {
		// if(EAG_flag)
		// 	agg_buf = host_addr;
    memcpy(bytes, host_addr, len);
    if (tracer.interested_in_range(paddr, paddr + PGSIZE, LOAD))
      tracer.trace(paddr, len, LOAD);
    else
      refill_tlb(addr, paddr, host_addr, LOAD);
  } else if (!sim->mmio_load(paddr, len, bytes)) {
    throw trap_load_access_fault(addr);
  }

  if (!matched_trigger) {
    reg_t data = reg_from_bytes(len, bytes);
    matched_trigger = trigger_exception(OPERATION_LOAD, addr, data);
    if (matched_trigger)
      throw *matched_trigger;
  }
}

void mmu_t::store_slow_path(reg_t addr, reg_t len, const uint8_t* bytes)
{

	/*if (addr == 0xAAAAAAAAAAAAAAAA || addr == 0xBBBBBBBBBBBBBBBB){

		MPI_Barrier(MPI_COMM_WORLD);
		std::cout <<"Rank " << sim->myid << " comletes MPI_Barrier\n";
		//rintf("Rank %d comletes MPI_Barrier\n",sim->myid);
		return;
	}*/
  reg_t paddr = translate(addr, STORE);

  if (!matched_trigger) {
    reg_t data = reg_from_bytes(len, bytes);
    matched_trigger = trigger_exception(OPERATION_STORE, addr, data);
    if (matched_trigger)
      throw *matched_trigger;
  }

  if (auto host_addr = sim->addr_to_mem(paddr)) {
    memcpy(host_addr, bytes, len);
    if (tracer.interested_in_range(paddr, paddr + PGSIZE, STORE))
      tracer.trace(paddr, len, STORE);
    else
      ;//refill_tlb(addr, paddr, host_addr, STORE);
  } else if (!sim->mmio_store(paddr, len, bytes)) {
    throw trap_store_access_fault(addr);
  }
}

tlb_entry_t mmu_t::refill_tlb(reg_t vaddr, reg_t paddr, char* host_addr, access_type type)
{
  // index = VPN % 256
  reg_t idx = (vaddr >> PGSHIFT) % TLB_ENTRIES;
  reg_t expected_tag = vaddr >> PGSHIFT;

  if ((tlb_load_tag[idx] & ~TLB_CHECK_TRIGGERS) != expected_tag)
    tlb_load_tag[idx] = -1;
  if ((tlb_store_tag[idx] & ~TLB_CHECK_TRIGGERS) != expected_tag)
    tlb_store_tag[idx] = -1;
  if ((tlb_insn_tag[idx] & ~TLB_CHECK_TRIGGERS) != expected_tag)
    tlb_insn_tag[idx] = -1;

  if ((check_triggers_fetch && type == FETCH) ||
      (check_triggers_load && type == LOAD) ||
      (check_triggers_store && type == STORE))
    expected_tag |= TLB_CHECK_TRIGGERS;

  if (type == FETCH) tlb_insn_tag[idx] = expected_tag;
  else if (type == STORE) tlb_store_tag[idx] = expected_tag;
  else tlb_load_tag[idx] = expected_tag;


  // host_addr - vaddr  =   real_VA - sim_VA
  tlb_entry_t entry = {host_addr - vaddr, paddr - vaddr};
  tlb_data[idx] = entry;
  return entry;
}

reg_t mmu_t::walk(reg_t addr, access_type type, reg_t mode)
{

#ifdef DEBUG
	if(EAG_flag) std::cout << "DEBUG::  Enter walk() function\n";
#endif

  vm_info vm = decode_vm_info(proc->max_xlen, mode, proc->get_state()->sptbr);
  if (vm.levels == 0){
#ifdef DEBUG
		if(EAG_flag) std::cout << "DEBUG:: vm.levels = 0, leaving walk() function\n";
#endif

    return addr & ((reg_t(2) << (proc->xlen-1))-1); // zero-extend from xlen
	}
	//if(EAG_flag)
	//EAG_flag = 0;
  bool s_mode = mode == PRV_S;
  bool sum = get_field(proc->state.mstatus, MSTATUS_SUM);
  bool mxr = get_field(proc->state.mstatus, MSTATUS_MXR);

  // verify bits xlen-1:va_bits-1 are all equal
  int va_bits = PGSHIFT + vm.levels * vm.idxbits;
  reg_t mask = (reg_t(1) << (proc->xlen - (va_bits-1))) - 1;
  reg_t masked_msbs = (addr >> (va_bits-1)) & mask;
  if (masked_msbs != 0 && masked_msbs != mask)
    vm.levels = 0;

  // Page table base address.
  // By default, xlen = 40, SPTBR_MODE_SV39, it is 0x80016000
  reg_t base = vm.ptbase;
//#ifdef DEBUG
//  std::cout << " vm.levels  = "<< vm.levels
//            << " vm.idxbits = "<< vm.idxbits
//	    << " vm.ptesize = "<< vm.ptesize
//            << std::endl;
//#endif
  // process the multi-level page-tables
  for (int i = vm.levels - 1; i >= 0; i--) {
    int ptshift = i * vm.idxbits;
    reg_t idx = (addr >> (PGSHIFT + ptshift)) & ((1 << vm.idxbits) - 1);

    // check that physical address of PTE is legal
    // ppte is the real VA of the allocated memory for spike
    auto ppte = sim->addr_to_mem(base + idx * vm.ptesize);
    if (!ppte){
#ifdef DEBUG
			if(EAG_flag) std::cout << "DEBUG:: Illegal memory physical address\n";
#endif
		  throw trap_load_access_fault(addr);
		}
    // check whether the it is 32 or 64 bits
    reg_t pte = vm.ptesize == 4 ? *(uint32_t*)ppte : *(uint64_t*)ppte;
    reg_t ppn = pte >> PTE_PPN_SHIFT;

//#ifdef DEBUG
//  std::cout << " ppte  = "<<  std::hex << &ppte
//            << " pte   = "<<  std::hex << pte
//	    << " ppn   = "<<  std::hex << ppn
//            << std::endl;
//#endif
    if (PTE_TABLE(pte)) { // next level of page table
      base = ppn << PGSHIFT;
    } else if ((pte & PTE_U) ? s_mode && (type == FETCH || !sum) : !s_mode) {
      break;
    } else if (!(pte & PTE_V) || (!(pte & PTE_R) && (pte & PTE_W))) {
      break;
    } else if (type == FETCH ? !(pte & PTE_X) :
               type == LOAD ?  !(pte & PTE_R) && !(mxr && (pte & PTE_X)) :
                               !((pte & PTE_R) && (pte & PTE_W))) {
      break;
    } else if ((ppn & ((reg_t(1) << ptshift) - 1)) != 0) {
      break;
    } else {
      reg_t ad = PTE_A | ((type == STORE) * PTE_D);
#ifdef RISCV_ENABLE_DIRTY
      // set accessed and possibly dirty bits.
      *(uint32_t*)ppte |= ad;
#else
      // take exception if access or possibly dirty bit is not set.
      if ((pte & ad) != ad)
        break;
#endif
      // for superpage mappings, make a fake leaf PTE for the TLB's benefit.
      reg_t vpn = addr >> PGSHIFT;
      reg_t value = (ppn | (vpn & ((reg_t(1) << ptshift) - 1))) << PGSHIFT;

//#ifdef DEBUG
//      std::cout << "Final output address is " << std::hex << value << std::endl;
//#endif
		//if(EAG_ne!=0) EAG_flag = 1;


      return value; // base addr of the mapped phsycial page
    }
  }

fail:
  switch (type) {
    case FETCH: throw trap_instruction_page_fault(addr);
    case LOAD:
#ifdef DEBUG
			if(EAG_flag) std::cout << "DEBUG:: \033[1m\033[31m trap_load_page_fault \x1B[0m \n";
#endif
			if(EAG_flag) trap_flag = 1;
			throw trap_load_page_fault(addr);
    case STORE:
#ifdef DEBUG
			if(EAG_flag) std::cout << "DEBUG:: \033[1m\033[31m trap_store_page_fault \x1B[0m \n";
#endif
			if(EAG_flag) trap_flag = 1;
			throw trap_store_page_fault(addr);
    default: abort();
  }
}



//
// reg_t mmu_t::walk_remote(reg_t addr, reg_t mode)
// {
// 	if(EAG_flag) std::cout << "DEBUG::  Enter walk_remote() function\n";
//   vm_info vm = decode_vm_info(proc->max_xlen, mode, proc->get_state()->sptbr);
//   if (vm.levels == 0){
// 	//	std::cout << "DEBUG:: vm.levels = 0, leaving walk() function\n";
// 		if(EAG_flag) std::cout << "DEBUG:: vm.levels = 0, leaving walk() function\n";
//     return addr & ((reg_t(2) << (proc->xlen-1))-1); // zero-extend from xlen
// 	}
// 	// if(EAG_flag)
// 	// EAG_flag = 0;
//   bool s_mode = mode == PRV_S;
//   bool sum = get_field(proc->state.mstatus, MSTATUS_SUM);
//   bool mxr = get_field(proc->state.mstatus, MSTATUS_MXR);
//
//   // verify bits xlen-1:va_bits-1 are all equal
//   int va_bits = PGSHIFT + vm.levels * vm.idxbits;
//   reg_t mask = (reg_t(1) << (proc->xlen - (va_bits-1))) - 1;
//   reg_t masked_msbs = (addr >> (va_bits-1)) & mask;
//   if (masked_msbs != 0 && masked_msbs != mask)
//     vm.levels = 0;
//
//   // Page table base address.
//   // By default, xlen = 40, SPTBR_MODE_SV39, it is 0x80016000
//   reg_t base = vm.ptbase;
// //#ifdef DEBUG
// //  std::cout << " vm.levels  = "<< vm.levels
// //            << " vm.idxbits = "<< vm.idxbits
// //	    << " vm.ptesize = "<< vm.ptesize
// //            << std::endl;
// //#endif
//   // process the multi-level page-tables
//   for (int i = vm.levels - 1; i >= 0; i--) {
//     int ptshift = i * vm.idxbits;
//     reg_t idx = (addr >> (PGSHIFT + ptshift)) & ((1 << vm.idxbits) - 1);
//
//     // check that physical address of PTE is legal
//     // ppte is the real VA of the allocated memory for spike
//     auto ppte = sim->addr_to_mem(base + idx * vm.ptesize);
//     if (!ppte){
// 			if(EAG_flag) std::cout << "DEBUG:: Illegal memory physical address\n";
// 		  throw trap_load_access_fault(addr);
// 		}
//     // check whether the it is 32 or 64 bits
//     reg_t pte = vm.ptesize == 4 ? *(uint32_t*)ppte : *(uint64_t*)ppte;
//     reg_t ppn = pte >> PTE_PPN_SHIFT;
//
// //#ifdef DEBUG
// //  std::cout << " ppte  = "<<  std::hex << &ppte
// //            << " pte   = "<<  std::hex << pte
// //	    << " ppn   = "<<  std::hex << ppn
// //            << std::endl;
// //#endif
//     if (PTE_TABLE(pte)) { // next level of page table
//       base = ppn << PGSHIFT;
//     } else {
//       // for superpage mappings, make a fake leaf PTE for the TLB's benefit.
//       reg_t vpn = addr >> PGSHIFT;
//       reg_t value = (ppn | (vpn & ((reg_t(1) << ptshift) - 1))) << PGSHIFT;
//
// //#ifdef DEBUG
// //      std::cout << "Final output address is " << std::hex << value << std::endl;
// //#endif
// 			// if(EAG_ne!=0)EAG_flag = 1;
//
//
//       return value; // base addr of the mapped phsycial page
//     }
//   }
//
//     abort();
// }

void mmu_t::register_memtracer(memtracer_t* t)
{
  flush_tlb();
  tracer.hook(t);
}

#undef DEBUG
