// See LICENSE for license details.

#include "mmu.h"
#include "sim.h"
#include "processor.h"
#include <mpi.h>
#include <iostream>
#define DEBUG
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
}

// xBGAS Extensions
bool mmu_t::set_xbgas(){
  xbgas_enable = true;
  return true;
}


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

  reg_t mode = proc->state.prv;
  if (type != FETCH) {
    if (!proc->state.dcsr.cause && get_field(proc->state.mstatus, MSTATUS_MPRV))
      mode = get_field(proc->state.mstatus, MSTATUS_MPP);
  }

  return walk(addr, type, mode) | (addr & (PGSIZE-1)); // base PPN | PG offset
}

tlb_entry_t mmu_t::fetch_slow_path(reg_t vaddr)
{
  reg_t paddr = translate(vaddr, FETCH);
  // host_addr is the real address (va) in the allocted memory 
  if (auto host_addr = sim->addr_to_mem(paddr)) {
    return refill_tlb(vaddr, paddr, host_addr, FETCH);
  } else {
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


void mmu_t::store_remote_path(int64_t target, reg_t addr,
                              reg_t len,uint8_t* bytes )
{

#ifdef DEBUG
  std::cout << "DEBUG::  Target ID = "
            << target << " Local Addr = " << addr << " value = "
            << std::dec<<(uint64_t)(*bytes) << std::endl;
#endif
  int rank	=	sim->myid;
  // Temporarily go through the MMU address translation
  reg_t paddr = translate(addr, STORE);
  auto host_addr = sim->addr_to_mem(paddr);
#ifdef DEBUG
  std::cout << "DEBUG:: Thread " << rank << ", Host Addr = 0x"
            << (reg_t)host_addr
            << ", device addr = "
            << (reg_t)(sim->mems[0].second->contents()) <<"\n";
  std::cout << "Thread " << rank << ", address offset = 0x"
            << (reg_t)host_addr - (reg_t)(sim->mems[0].second->contents())
            <<"\n";
#endif

  //MPI_Win_fence(0, sim->win);
  //Target thread
  if( rank == target ){

#ifdef DEBUG
//    std::cout << "DEBUG::  Thread " << rank << " executing the xbgas store\n";
#endif
  }else{ //Requster thread
#ifdef DEBUG
    std::cout << "DEBUG:: Thread " << rank << " executing the xbgas store\n";
		uint64_t buf = 66;
#endif

    //MPI_Win_lock(MPI_LOCK_SHARED, target, 0, sim->win);
    std::cout << "DEBUG:: Thread " << rank << " acquiring the lock\n";
    MPI_Win_lock_all(0, sim->win);
    std::cout << "DEBUG:: Thread " << rank << " acquired the lock, sending Put\n";
#if 0
    MPI_Put(bytes, len, MPI_UINT8_T, target,
            (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
            len, MPI_UINT8_T, sim->win);
#endif
    MPI_Accumulate(bytes, len, MPI_UINT8_T, target,
            (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
            len, MPI_UINT8_T, MPI_REPLACE, sim->win);
#ifdef DEBUG
		// Check the stored value
		MPI_Get((uint8_t*)&buf, len, MPI_UINT8_T, target,
						(MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
						len, MPI_UINT8_T, sim->win);
#endif

    MPI_Win_flush(rank,sim->win);
    std::cout << "DEBUG:: Thread " << rank << " Put complete; unlocking\n";
    //MPI_Win_unlock(target, sim->win);
    MPI_Win_unlock_all(sim->win);
#ifdef DEBUG
    std::cout << "DEBUG:: Thread " << rank << " remote value = " << (uint64_t)(buf)
    					<< " stored value = "<<(uint64_t)(*bytes) << "\n";
#endif
    std::cout << "DEBUG:: Thread " << rank << " unlock complete\n";

    //MPI_Request put_req;
    //MPI_Win_lock_all(0, sim->win);
    //MPI_Rput(bytes, len, MPI_UINT8_T, target,
    //        (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
    //        len, MPI_UINT8_T, sim->win, &put_req);
    //MPI_Win_unlock_all(sim->win);
  }

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
  int rank	=	sim->myid;
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

  //MPI_Win_fence(0, sim->win);
  //Target thread
  if( rank == target ){
    MPI_Request recv_req;
    reg_t offset;
#ifdef DEBUG
//    std::cout << "DEBUG::  Thread " << rank << " executing the xbgas load\n"; 
#endif
    //MPI_Recv(&offset, 1, MPI_UINT64_T, sim->world_size - target - 1, 0, MPI_COMM_WORLD, &status);
    //std::cout << "Target Thread " << rank << " received the address offset: "<< offset <<" \n"; 
    //MPI_Send((uint8_t*)(sim->x_mem.first + offset), len, MPI_UINT8_T, sim->world_size - target - 1, 1, MPI_COMM_WORLD);
    //MPI_Send((uint8_t*)(host_addr), len, MPI_UINT8_T, sim->world_size - target - 1, 1, MPI_COMM_WORLD);

  }else{ // Requestor Threads
    std::pair<reg_t, reg_t> message;
    message = std::make_pair(len, addr);
#ifdef DEBUG
    std::cout << "DEBUG::  Thread " << rank << " executing the xbgas load\n";
#endif
    //MPI_Request send_req;
    //MPI_Send(&message.second, 1, MPI_UINT64_T, target, 0, MPI_COMM_WORLD);
    //MPI_Recv(bytes, len, MPI_UINT8_T, target, 1, MPI_COMM_WORLD, &status );

    //MPI_Win_lock(MPI_LOCK_SHARED, target, 0, sim->win);
    MPI_Win_lock_all(0, sim->win);
    std::cout << "DEBUG:: Thread " << rank << " acquired the lock; executing Get\n";
//#if 0
    MPI_Get(bytes, len, MPI_UINT8_T, target,
            (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
            len, MPI_UINT8_T, sim->win);
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
    MPI_Win_flush(rank,sim->win);
    MPI_Win_unlock_all(sim->win);
    std::cout << "DEBUG:: Thread " << rank << " released the lock\n";

    //MPI_Request get_req;
    //MPI_Win_lock_all(0, sim->win);
    //MPI_Rget(bytes, len, MPI_UINT8_T, target,
    //        (MPI_Aint)host_addr - (MPI_Aint)(sim->mems[0].second->contents()),
    //        len, MPI_UINT8_T, sim->win, &get_req);
    //MPI_Wait(&get_req,MPI_STATUS_IGNORE);
    //MPI_Win_unlock_all(sim->win);

  }

  //MPI_Win_fence(0, sim->win);
  //MPI_Barrier(MPI_COMM_WORLD);
#ifdef DEBUG
  std::cout << "DEBUG::  Thread " << rank << " completes the xbgas load\n";
#endif
}

void mmu_t::load_slow_path(reg_t addr, reg_t len, uint8_t* bytes)
{
  reg_t paddr = translate(addr, LOAD);

  if (auto host_addr = sim->addr_to_mem(paddr)) {
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
      refill_tlb(addr, paddr, host_addr, STORE);
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
  vm_info vm = decode_vm_info(proc->max_xlen, mode, proc->get_state()->sptbr);
  if (vm.levels == 0)
    return addr & ((reg_t(2) << (proc->xlen-1))-1); // zero-extend from xlen

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
    if (!ppte)
      throw trap_load_access_fault(addr);
    
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
      return value; // base addr of the mapped phsycial page
    }
  }

fail:
  switch (type) {
    case FETCH: throw trap_instruction_page_fault(addr);
    case LOAD: throw trap_load_page_fault(addr);
    case STORE: throw trap_store_page_fault(addr);
    default: abort();
  }
}

void mmu_t::register_memtracer(memtracer_t* t)
{
  flush_tlb();
  tracer.hook(t);
}

void mmu_t::set_sst_func( void *ptr ){
  sst_func = (void (*)(uint64_t,uint32_t,bool,bool,bool,bool,uint32_t))(ptr);
}
