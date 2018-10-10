// See LICENSE for license details.

#include "mmu.h"
#include "sim.h"
#include "devices.h"
#include "processor.h"
#include <mpi.h>
#include <iostream>
#define DEBUG
mmu_t::mmu_t(sim_t* sim, bus_t* bus, processor_t* proc)
 : sim(sim), bus(bus), proc(proc),
  check_triggers_fetch(false),
  check_triggers_load(false),
  check_triggers_store(false),
  matched_trigger(NULL),
  sst_func(NULL)
{
  flush_tlb();
}

mmu_t::~mmu_t()
{
}

// xBGAS Extensions
void mmu_t::xbgas_set_peer(int64_t target, mmu_t *mmu)
{
  xbgas_peers[target] = mmu;
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
  if (auto host_addr = bus->addr_to_mem(paddr)) {
    return refill_tlb(vaddr, paddr, host_addr, FETCH);
  } else {
    if (!bus->load(paddr, sizeof fetch_temp, (uint8_t*)&fetch_temp))
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
                              reg_t len, uint8_t *bytes)
{
  printf("mmu_t::store_remote_path (%d -> %ld): %ld bytes to 0x%08lx\n", proc->xbgas_id, target, len, addr);
  
  if (!xbgas_peers.count(target)) {
    throw std::runtime_error("physical target " + std::to_string(target) + " does not exist in peers map");
  }
  mmu_t *peer = xbgas_peers[target];
  reg_t paddr = peer->translate(addr, STORE);
  auto host_addr = peer->bus->addr_to_mem(paddr);
  if (!host_addr)
    throw std::runtime_error("unhandled case: translation failure for remote store");
  
  memcpy(host_addr, bytes, len);
}

// NOTE: currently used to trasnfer a "single" data element between two threads
//       based on the upper 64-bit address stored in OLB
void mmu_t::load_remote_path(int64_t target, reg_t addr,
                             reg_t len, uint8_t* bytes)
{
  printf("mmu_t::load_remote_path (%d <- %ld): %ld bytes from 0x%08lx\n", proc->xbgas_id, target, len, addr);

  if (!xbgas_peers.count(target)) {
    throw std::runtime_error("physical target " + std::to_string(target) + " does not exist in peers map");
  }
  mmu_t *peer = xbgas_peers[target];
  reg_t paddr = peer->translate(addr, LOAD);
  auto host_addr = peer->bus->addr_to_mem(paddr);
  if (!host_addr)
    throw std::runtime_error("unhandled case: translation failure for remote load");
  
  memcpy(bytes, host_addr, len);
}

void mmu_t::load_slow_path(reg_t addr, reg_t len, uint8_t* bytes)
{
  reg_t paddr = translate(addr, LOAD);

  if (auto host_addr = bus->addr_to_mem(paddr)) {
    memcpy(bytes, host_addr, len);
    if (tracer.interested_in_range(paddr, paddr + PGSIZE, LOAD))
      tracer.trace(paddr, len, LOAD);
    else
      refill_tlb(addr, paddr, host_addr, LOAD);
  } else if (!bus->load(paddr, len, bytes)) {
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

  if (auto host_addr = bus->addr_to_mem(paddr)) {
    memcpy(host_addr, bytes, len);
    if (tracer.interested_in_range(paddr, paddr + PGSIZE, STORE))
      tracer.trace(paddr, len, STORE);
    else
      refill_tlb(addr, paddr, host_addr, STORE);
  } else if (!bus->store(paddr, len, bytes)) {
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
    auto ppte = bus->addr_to_mem(base + idx * vm.ptesize);
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
