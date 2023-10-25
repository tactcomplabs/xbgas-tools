//require_extension('A');
require_rv64;
WRITE_RD(MMU.xbgas_amo_uint64(EXT1, RS1, RS2, "MIN"));
