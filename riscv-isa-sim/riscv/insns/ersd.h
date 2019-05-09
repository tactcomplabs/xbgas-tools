require_rv64;
#ifdef DEBUG
int i;
for(i = 0; i< 32; i++){
	if(READ_XBREG(i))
		std::cout << "\nDEBUG::  e" <<std::dec<<i<<" = "<< READ_XBREG(i);
}
#endif
MMU.xbgas_store_uint64(EXT3, RS2, RS1);
