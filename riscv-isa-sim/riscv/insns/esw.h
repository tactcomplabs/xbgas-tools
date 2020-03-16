require_rv64;
#ifdef DEBUG
	if(trap_flag) printf("\033[1m\033[31m Debug::  Calling ESW \x1B[0m\n");
#endif
MMU.xbgas_store_uint32(EXT1, RS1 + insn.s_imm(), RS2);
