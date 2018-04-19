// See LICENSE for license details.

#include "sim.h"
#include "mmu.h"
#include "remote_bitbang.h"
#include "cachesim.h"
#include "extension.h"
#include <dlfcn.h>
#include <fesvr/option_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <memory>
#include <mpi.h>


#define DEBUG
static void help()
{
  fprintf(stderr, "usage: spike [host options] <target program> [target options]\n");
  fprintf(stderr, "Host Options:\n");
  fprintf(stderr, "  -p<n>                 Simulate <n> processors [default 1]\n");
  fprintf(stderr, "  -m<n>                 Provide <n> MiB of target memory [default 2048]\n");
  fprintf(stderr, "  -m<a:m,b:n,...>       Provide memory regions of size m and n bytes\n");
  fprintf(stderr, "                          at base addresses a and b (with 4 KiB alignment)\n");
  fprintf(stderr, "  -d                    Interactive debug mode\n");
  fprintf(stderr, "  -g                    Track histogram of PCs\n");
  fprintf(stderr, "  -l                    Generate a log of execution\n");
  fprintf(stderr, "  -h                    Print this help message\n");
  fprintf(stderr, "  -H                 Start halted, allowing a debugger to connect\n");
  fprintf(stderr, "  --isa=<name>          RISC-V ISA string [default %s]\n", DEFAULT_ISA);
  fprintf(stderr, "  --pc=<address>        Override ELF entry point\n");
  fprintf(stderr, "  --ic=<S>:<W>:<B>      Instantiate a cache model with S sets,\n");
  fprintf(stderr, "  --dc=<S>:<W>:<B>        W ways, and B-byte blocks (with S and\n");
  fprintf(stderr, "  --l2=<S>:<W>:<B>        B both powers of 2).\n");
  fprintf(stderr, "  --extension=<name>    Specify RoCC Extension\n");
  fprintf(stderr, "  --extlib=<name>       Shared library to load\n");
  fprintf(stderr, "  --rbb-port=<port>     Listen on <port> for remote bitbang connection\n");
  fprintf(stderr, "  --dump-dts  Print device tree string and exit\n");
  fprintf(stderr, "  ----------- xBGAS Options -----------\n" );
  fprintf(stderr, "  -x<n>                  Enable xBGAS Extionsion and provide n MB shared memory\n" );
  exit(1);
}

static std::pair<char*, size_t> make_shared_mem(const char* arg)
{
  // handle legacy xbgas argument
  char* p;
  auto mb = strtoull(arg, &p, 0);
  if (*p == 0) {
    reg_t size = (reg_t(mb) << 20);
    p = (char*)calloc(1, size);
#ifdef DEBUG
		//std::cout << "x_mem addr = " << std::hex<<(uint64_t)p << "\n";
#endif
    if(!p) 
		throw std::runtime_error("couldn't allocate " + std::to_string(size) + " bytes of xBGAS shared memory");
    return std::make_pair(p, size);
  }
  	return std::make_pair((char*)NULL, 0);
}

static std::vector<std::pair<reg_t, mem_t*>> make_mems(const char* arg)
{
  // handle legacy mem argument
  char* p;
  auto mb = strtoull(arg, &p, 0);
  if (*p == 0) {
    reg_t size = reg_t(mb) << 20;
    return std::vector<std::pair<reg_t, mem_t*>>(1, std::make_pair(reg_t(DRAM_BASE), new mem_t(size)));
  }

  // handle base/size tuples
  std::vector<std::pair<reg_t, mem_t*>> res;
  while (true) {
    auto base = strtoull(arg, &p, 0);
    if (!*p || *p != ':')
      help();
    auto size = strtoull(p + 1, &p, 0);
    if ((size | base) % PGSIZE != 0)
      help();
    res.push_back(std::make_pair(reg_t(base), new mem_t(size)));
    if (!*p)
      break;
    if (*p != ',')
      help();
    arg = p + 1;
  }
  return res;
}

int main(int argc, char** argv)
{
  bool debug = false;
  bool halted = false;
  bool histogram = false;
  bool log = false;
  bool dump_dts = false;
  size_t nprocs = 1;
  reg_t start_pc = reg_t(-1);
  std::vector<std::pair<reg_t, mem_t*>> mems;
  std::unique_ptr<icache_sim_t> ic;
  std::unique_ptr<dcache_sim_t> dc;
  std::unique_ptr<cache_sim_t> l2;
  std::function<extension_t*()> extension;
  const char* isa = DEFAULT_ISA;
  uint16_t rbb_port = 0;
  bool use_rbb = false;
  int ret;
  // xbgas extensions
  int 	xbgas 			= 0;
  int	 	rank	  		= 0; 
	int 	world_size	= 0; 
  std::pair<char*, size_t> shared_mem;
	MPI_Win win;
  MPI_Init(&argc, &argv);

  option_parser_t parser;
  parser.help(&help);
  parser.option('h', 0, 0, [&](const char* s){help();});
  parser.option('d', 0, 0, [&](const char* s){debug = true;});
  parser.option('g', 0, 0, [&](const char* s){histogram = true;});
  parser.option('l', 0, 0, [&](const char* s){log = true;});
  parser.option('p', 0, 1, [&](const char* s){nprocs = atoi(s);});
  parser.option('m', 0, 1, [&](const char* s){mems = make_mems(s);});
  // I wanted to use --halted, but for some reason that doesn't work.
  parser.option('H', 0, 0, [&](const char* s){halted = true;});
  // xBGAS Extensions
  parser.option('x', 0, 1, [&](const char* s){
		xbgas = 1; 
		shared_mem = make_shared_mem(s);
		});
  parser.option(0, "rbb-port", 1, [&](const char* s){use_rbb = true; rbb_port = atoi(s);});
  parser.option(0, "pc", 1, [&](const char* s){start_pc = strtoull(s, 0, 0);});
  parser.option(0, "ic", 1, [&](const char* s){ic.reset(new icache_sim_t(s));});
  parser.option(0, "dc", 1, [&](const char* s){dc.reset(new dcache_sim_t(s));});
  parser.option(0, "l2", 1, [&](const char* s){l2.reset(cache_sim_t::construct(s, "L2$"));});
  parser.option(0, "isa", 1, [&](const char* s){isa = s;});
  parser.option(0, "extension", 1, [&](const char* s){extension = find_extension(s);});
  parser.option(0, "dump-dts", 0, [&](const char *s){dump_dts = true;});
  parser.option(0, "extlib", 1, [&](const char *s){

    void *lib = dlopen(s, RTLD_NOW | RTLD_GLOBAL);
    if (lib == NULL) {
      fprintf(stderr, "Unable to load extlib '%s': %s\n", s, dlerror());
      exit(-1);
    }
  });
	

  auto argv1 = parser.parse(argv);
  std::vector<std::string> htif_args(argv1, (const char*const*)argv + argc);
  if (mems.empty())
    mems = make_mems("2048");
#ifdef DEBUG
  std::cout<< "DEBUG::  Allocated memory address is 0x" << (reg_t)mems[0].second->contents() << std::hex << std::endl;  
#endif	
  // Init the xBGAS extensions 
  if(xbgas){
#ifdef DEBUG		
    std::cout << "DEBUG::  xBGAS extension is enabled\n";
#endif	
    char 	processor_name[MPI_MAX_PROCESSOR_NAME];
    int	 	name_len;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &name_len);
		MPI_Win_create_dynamic(MPI_INFO_NULL, MPI_COMM_WORLD, &win);
		// Attach the shared memory 
	  // MPI_Win_attach(win, shared_mem.first, shared_mem.second);	

		// Attach the original memory 
	  //MPI_Win_attach(win, mems[0].second->contents(), mems[0].second->size());	
		MPI_Win_create(mems[0].second->contents(), mems[0].second->size(), 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	 //std::cout << "size of the attached window is " << mems[0].second->size() << std::endl;

#ifdef DEBUG
    std::cout <<"DEBUG::  Hello world from processor " 
 	    << processor_name
	    << ", rank" 	<<  rank 
 	    << " out of " 	<<  world_size 
	    << " processors"    <<  std::endl;
#endif
		 
  }

  //if(xbgas == true && shared_mem.first == NULL)
  //shared_mem = make_shared_mem("512");
#ifdef DEBUG
  std::cout << "DEBUG::  Thread " << rank <<", Shared memory address is 0x" <<std::hex<< (uint64_t)shared_mem.first<<"\n";
#endif
  sim_t s(isa, nprocs, halted, start_pc, mems, htif_args, shared_mem, world_size, rank, xbgas, win);
	// Init OLB in each core 
  if (xbgas){
		if(s.olb_init() == -1)
			throw std::runtime_error("olb init failed\n");
	}


  std::unique_ptr<remote_bitbang_t> remote_bitbang((remote_bitbang_t *) NULL);
  std::unique_ptr<jtag_dtm_t> jtag_dtm(new jtag_dtm_t(&s.debug_module));
  if (use_rbb) {
    remote_bitbang.reset(new remote_bitbang_t(rbb_port, &(*jtag_dtm)));
    s.set_remote_bitbang(&(*remote_bitbang));
  }

  if (dump_dts) {
    printf("%s", s.get_dts());
    return 0;
  }

  if (!*argv1)
    help();

  if (ic && l2) ic->set_miss_handler(&*l2);
  if (dc && l2) dc->set_miss_handler(&*l2);
  for (size_t i = 0; i < nprocs; i++)
  {
    if (ic) s.get_core(i)->get_mmu()->register_memtracer(&*ic);
    if (dc) s.get_core(i)->get_mmu()->register_memtracer(&*dc);
    if (extension) s.get_core(i)->register_extension(extension());
  }

  // xBGAS Extensions
  if(xbgas) s.get_core(0)->enable_xbgas();

  s.set_debug(debug);
  s.set_log(log);
  s.set_histogram(histogram);
  ret = s.run();
	if(xbgas){
		MPI_Win_detach(win,mems[0].second->contents());
		MPI_Win_free(&win);
  	MPI_Finalize();
	}
  return ret;
}
