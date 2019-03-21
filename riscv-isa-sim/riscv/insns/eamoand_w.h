//require_extension('A');
require_rv64;
WRITE_RD(sext32(MMU.xbgas_amo_uint32(EXT1, RS1, RS2,"AND")));
