//require_extension('A');
require_rv64;
WRITE_RD(MMU.xbgas_cas_uint64(EXT1, RS1, RS2,"CAS", RD));
