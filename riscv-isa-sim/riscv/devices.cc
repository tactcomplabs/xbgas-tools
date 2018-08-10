#include "devices.h"

void bus_t::add_device(reg_t addr, abstract_device_t* dev)
{
  devices[-addr] = dev;
}

bool bus_t::load(reg_t addr, size_t len, uint8_t* bytes)
{
  auto it = devices.lower_bound(-addr);
  if (it == devices.end())
    return false;
  return it->second->load(addr - -it->first, len, bytes);
}

bool bus_t::store(reg_t addr, size_t len, const uint8_t* bytes)
{
  auto it = devices.lower_bound(-addr);
  if (it == devices.end())
    return false;
  return it->second->store(addr - -it->first, len, bytes);
}

std::pair<reg_t, abstract_device_t*> bus_t::find_device(reg_t addr)
{
  auto it = devices.lower_bound(-addr);
  if (it == devices.end() || addr < -it->first)
    return std::make_pair((reg_t)0, (abstract_device_t*)NULL);
  return std::make_pair(-it->first, it->second);
}

char* bus_t::addr_to_mem(reg_t addr) {
  auto desc = find_device(addr);
  if (auto mem = dynamic_cast<mem_t*>(desc.second))
    if (addr - desc.first < mem->size())
      return mem->contents() + (addr - desc.first);
  return NULL;
}
