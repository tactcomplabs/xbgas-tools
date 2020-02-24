#define DEBUG 1
require_rv64;
#ifdef DEBUG
	std::cout << "Calling EAG instruction" << std::endl;
	std::cout << "RS1 = " << RS1 <<", RS2 = " << RS2<<", RD = "<< RD <<"\n";
#endif
WRITE_RD(MMU.xbgas_aggregate(RS1, RS2));
