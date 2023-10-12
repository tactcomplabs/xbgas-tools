#*****************************************************************************
# ese.S
#-----------------------------------------------------------------------------
#
# Test ese instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ESE_OP( 2, ld, ese, 0x00aa00aa00aa00aa, 0,  tdat, 0 );
  TEST_ESE_OP( 3, ld, ese, 0xaa00aa00aa00aa00, 8,  tdat, 0 );
  TEST_ESE_OP( 4, ld, ese, 0x0aa00aa00aa00aa0, 16, tdat, 0 );
  TEST_ESE_OP( 5, ld, ese, 0xa00aa00aa00aa00a, 24, tdat, 0 );
  TEST_ESE_OP( 102, eld, ese, 0x00aa00aa00aa00aa, 0,  tdat, 1 );
  TEST_ESE_OP( 103, eld, ese, 0xaa00aa00aa00aa00, 8,  tdat, 1 );
  TEST_ESE_OP( 104, eld, ese, 0x0aa00aa00aa00aa0, 16, tdat, 1 );
  TEST_ESE_OP( 105, eld, ese, 0xa00aa00aa00aa00a, 24, tdat, 1 );

  # Test with negative offset

  TEST_ESE_OP( 6, ld, ese, 0x00aa00aa00aa00aa, -24, tdat8, 0  );
  TEST_ESE_OP( 7, ld, ese, 0xaa00aa00aa00aa00, -16, tdat8, 0  );
  TEST_ESE_OP( 8, ld, ese, 0x0aa00aa00aa00aa0, -8,  tdat8, 0  );
  TEST_ESE_OP( 9, ld, ese, 0xa00aa00aa00aa00a, 0,   tdat8, 0  );
  TEST_ESE_OP( 106, eld, ese, 0x00aa00aa00aa00aa, -24, tdat8, 1 );
  TEST_ESE_OP( 107, eld, ese, 0xaa00aa00aa00aa00, -16, tdat8, 1 );
  TEST_ESE_OP( 108, eld, ese, 0x0aa00aa00aa00aa0, -8,  tdat8, 1 );
  TEST_ESE_OP( 109, eld, ese, 0xa00aa00aa00aa00a, 0,   tdat8, 1 );

  # # Test with a negative base

  TEST_CASE( 10, x5, 0x1234567812345678, \
    SET_REMOTE_ADDR( e4, 0 ) \
    la  x1, tdat9; \
    li  x2, 0x1234567812345678; \
    eaddie e2, x2, 0; \
    addi x4, x1, -32; \
    ese e2, 32(x4); \
    ld x5, 0(x1); \
  )

  TEST_CASE( 110, x5, 0x1234567812345678, \
    SET_REMOTE_ADDR( e4, 1 ) \
    SET_REMOTE_ADDR( e1, 1 ) \
    la  x1, tdat9; \
    li  x2, 0x1234567812345678; \
    eaddie e2, x2, 0; \
    addi x4, x1, -32; \
    ese e2, 32(x4); \
    eld x5, 0(x1); \
  )

  # # Test with unaligned base

  TEST_CASE( 11, x5, 0x5821309858213098, \
    SET_REMOTE_ADDR( e1, 0 ) \
    la  x1, tdat9; \
    li  x2, 0x5821309858213098; \
    eaddie e2, x2, 0; \
    addi x1, x1, -3; \
    ese e2, 11(x1); \
    la  x4, tdat10; \
    ld x5, 0(x4); \
  )
  TEST_CASE( 111, x5, 0x5821309858213098, \
    SET_REMOTE_ADDR( e1, 1 ) \
    SET_REMOTE_ADDR( e4, 1 ) \
    la  x1, tdat9; \
    li  x2, 0x5821309858213098; \
    eaddie e2, x2, 0; \
    addi x1, x1, -3; \
    ese e2, 11(x1); \
    la  x4, tdat10; \
    eld x5, 0(x4); \
  )

  # #-------------------------------------------------------------
  # # Bypassing tests
  # #-------------------------------------------------------------

  TEST_ESE_SRC12_BYPASS( 12, 0, 0, ld, ese, 0xabbccdd, 0,  tdat, 0 );
  TEST_ESE_SRC12_BYPASS( 13, 0, 1, ld, ese, 0xaabbccd, 8,  tdat, 0 );
  TEST_ESE_SRC12_BYPASS( 14, 0, 2, ld, ese, 0xdaabbcc, 16, tdat, 0 );
  TEST_ESE_SRC12_BYPASS( 15, 1, 0, ld, ese, 0xddaabbc, 24, tdat, 0 );
  TEST_ESE_SRC12_BYPASS( 16, 1, 1, ld, ese, 0xcddaabb, 32, tdat, 0 );
  TEST_ESE_SRC12_BYPASS( 17, 2, 0, ld, ese, 0xccddaab, 40, tdat, 0 );
  TEST_ESE_SRC12_BYPASS( 112, 0, 0, eld, ese, 0xabbccdd, 0,  tdat, 1 );
  TEST_ESE_SRC12_BYPASS( 113, 0, 1, eld, ese, 0xaabbccd, 8,  tdat, 1 );
  TEST_ESE_SRC12_BYPASS( 114, 0, 2, eld, ese, 0xdaabbcc, 16, tdat, 1 );
  TEST_ESE_SRC12_BYPASS( 115, 1, 0, eld, ese, 0xddaabbc, 24, tdat, 1 );
  TEST_ESE_SRC12_BYPASS( 116, 1, 1, eld, ese, 0xcddaabb, 32, tdat, 1 );
  TEST_ESE_SRC12_BYPASS( 117, 2, 0, eld, ese, 0xccddaab, 40, tdat, 1 );

  TEST_ESE_SRC21_BYPASS( 18, 0, 0, ld, ese, 0x00112233, 0,  tdat, 0 );
  TEST_ESE_SRC21_BYPASS( 19, 0, 1, ld, ese, 0x30011223, 8,  tdat, 0 );
  TEST_ESE_SRC21_BYPASS( 20, 0, 2, ld, ese, 0x33001122, 16, tdat, 0 );
  TEST_ESE_SRC21_BYPASS( 21, 1, 0, ld, ese, 0x23300112, 24, tdat, 0 );
  TEST_ESE_SRC21_BYPASS( 22, 1, 1, ld, ese, 0x22330011, 32, tdat, 0 );
  TEST_ESE_SRC21_BYPASS( 23, 2, 0, ld, ese, 0x12233001, 40, tdat, 0 );
  TEST_ESE_SRC21_BYPASS( 118, 0, 0, eld, ese, 0x00112233, 0,  tdat, 1 );
  TEST_ESE_SRC21_BYPASS( 119, 0, 1, eld, ese, 0x30011223, 8,  tdat, 1 );
  TEST_ESE_SRC21_BYPASS( 120, 0, 2, eld, ese, 0x33001122, 16, tdat, 1 );
  TEST_ESE_SRC21_BYPASS( 121, 1, 0, eld, ese, 0x23300112, 24, tdat, 1 );
  TEST_ESE_SRC21_BYPASS( 122, 1, 1, eld, ese, 0x22330011, 32, tdat, 1 );
  TEST_ESE_SRC21_BYPASS( 123, 2, 0, eld, ese, 0x12233001, 40, tdat, 1 );

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .dword 0xdeadbeefdeadbeef
tdat2:  .dword 0xdeadbeefdeadbeef
tdat3:  .dword 0xdeadbeefdeadbeef
tdat4:  .dword 0xdeadbeefdeadbeef
tdat5:  .dword 0xdeadbeefdeadbeef
tdat6:  .dword 0xdeadbeefdeadbeef
tdat7:  .dword 0xdeadbeefdeadbeef
tdat8:  .dword 0xdeadbeefdeadbeef
tdat9:  .dword 0xdeadbeefdeadbeef
tdat10: .dword 0xdeadbeefdeadbeef

RVTEST_DATA_END
