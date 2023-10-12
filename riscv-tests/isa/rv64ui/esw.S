#*****************************************************************************
# esw.S
#-----------------------------------------------------------------------------
#
# Test esw instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_EST_OP( 2, lw, esw, 0x0000000000aa00aa, 0,  tdat, 0 );
  TEST_EST_OP( 3, lw, esw, 0xffffffffaa00aa00, 4,  tdat, 0 );
  TEST_EST_OP( 4, lw, esw, 0x000000000aa00aa0, 8,  tdat, 0 );
  TEST_EST_OP( 5, lw, esw, 0xffffffffa00aa00a, 12, tdat, 0 );

  PRESET_MEM(esw, 0xdeadbeef, tdat, 4, 4);
  TEST_EST_OP( 102, elw, esw, 0x0000000000aa00aa, 0,  tdat, 1 );
  TEST_EST_OP( 103, elw, esw, 0xffffffffaa00aa00, 4,  tdat, 1 );
  TEST_EST_OP( 104, elw, esw, 0x000000000aa00aa0, 8,  tdat, 1 );
  TEST_EST_OP( 105, elw, esw, 0xffffffffa00aa00a, 12, tdat, 1 );

  # Test with negative offset

  TEST_EST_OP( 6, lw, esw, 0x0000000000aa00aa, -12, tdat8, 0 );
  TEST_EST_OP( 7, lw, esw, 0xffffffffaa00aa00, -8,  tdat8, 0 );
  TEST_EST_OP( 8, lw, esw, 0x000000000aa00aa0, -4,  tdat8, 0 );
  TEST_EST_OP( 9, lw, esw, 0xffffffffa00aa00a, 0,   tdat8, 0 );

  PRESET_MEM(esw, 0xdeadbeef, tdat5, 4, 4);
  TEST_EST_OP( 106, elw, esw, 0x0000000000aa00aa, -12, tdat8, 1 );
  TEST_EST_OP( 107, elw, esw, 0xffffffffaa00aa00, -8,  tdat8, 1 );
  TEST_EST_OP( 108, elw, esw, 0x000000000aa00aa0, -4,  tdat8, 1 );
  TEST_EST_OP( 109, elw, esw, 0xffffffffa00aa00a, 0,   tdat8, 1 );

  # Test with a negative base

  TEST_CASE( 10, x5, 0x12345678, \
    SET_REMOTE_ADDR(e4, 0) \
    la  x1, tdat9; \
    li  x2, 0x12345678; \
    addi x4, x1, -32; \
    esw x2, 32(x4); \
    elw x5, 0(x1); \
  )

  PRESET_MEM(esw, 0xdeadbeef, tdat9, 4, 1);
  TEST_CASE( 110, x5, 0x12345678, \
    SET_REMOTE_ADDR(e4, 1) \
    SET_REMOTE_ADDR(e1, 1) \
    la  x1, tdat9; \
    li  x2, 0x12345678; \
    addi x4, x1, -32; \
    esw x2, 32(x4); \
    elw x5, 0(x1); \
  )

  # Test with unaligned base

  TEST_CASE( 11, x5, 0x58213098, \
    SET_REMOTE_ADDR(e1, 0) \
    la  x1, tdat9; \
    li  x2, 0x58213098; \
    addi x1, x1, -3; \
    esw x2, 7(x1); \
    la  x4, tdat10; \
    lw x5, 0(x4); \
  )

  PRESET_MEM(esw, 0xdeadbeef, tdat10, 4, 1);
  TEST_CASE( 111, x5, 0x58213098, \
    SET_REMOTE_ADDR(e1, 1) \
    SET_REMOTE_ADDR(e4, 1) \
    la  x1, tdat9; \
    li  x2, 0x58213098; \
    addi x1, x1, -3; \
    esw x2, 7(x1); \
    la  x4, tdat10; \
    elw x5, 0(x4); \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_EST_SRC12_BYPASS( 12, 0, 0, lw, esw, 0xffffffffaabbccdd, 0,  tdat, 0 );
  TEST_EST_SRC12_BYPASS( 13, 0, 1, lw, esw, 0xffffffffdaabbccd, 4,  tdat, 0 );
  TEST_EST_SRC12_BYPASS( 14, 0, 2, lw, esw, 0xffffffffddaabbcc, 8,  tdat, 0 );
  TEST_EST_SRC12_BYPASS( 15, 1, 0, lw, esw, 0xffffffffcddaabbc, 12, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 16, 1, 1, lw, esw, 0xffffffffccddaabb, 16, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 17, 2, 0, lw, esw, 0xffffffffbccddaab, 20, tdat, 0 );

  PRESET_MEM(esw, 0xdeadbeef, tdat, 4, 6);
  TEST_EST_SRC12_BYPASS( 112, 0, 0, elw, esw, 0xffffffffaabbccdd, 0,  tdat, 1 );
  TEST_EST_SRC12_BYPASS( 113, 0, 1, elw, esw, 0xffffffffdaabbccd, 4,  tdat, 1 );
  TEST_EST_SRC12_BYPASS( 114, 0, 2, elw, esw, 0xffffffffddaabbcc, 8,  tdat, 1 );
  TEST_EST_SRC12_BYPASS( 115, 1, 0, elw, esw, 0xffffffffcddaabbc, 12, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 116, 1, 1, elw, esw, 0xffffffffccddaabb, 16, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 117, 2, 0, elw, esw, 0xffffffffbccddaab, 20, tdat, 1 );

  TEST_EST_SRC21_BYPASS( 18, 0, 0, lw, esw, 0x00112233, 0,  tdat, 0 );
  TEST_EST_SRC21_BYPASS( 19, 0, 1, lw, esw, 0x30011223, 4,  tdat, 0 );
  TEST_EST_SRC21_BYPASS( 20, 0, 2, lw, esw, 0x33001122, 8,  tdat, 0 );
  TEST_EST_SRC21_BYPASS( 21, 1, 0, lw, esw, 0x23300112, 12, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 22, 1, 1, lw, esw, 0x22330011, 16, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 23, 2, 0, lw, esw, 0x12233001, 20, tdat, 0 );
  
  PRESET_MEM(esw, 0xdeadbeef, tdat, 4, 6);
  TEST_EST_SRC21_BYPASS( 118, 0, 0, elw, esw, 0x00112233, 0,  tdat, 1 );
  TEST_EST_SRC21_BYPASS( 119, 0, 1, elw, esw, 0x30011223, 4,  tdat, 1 );
  TEST_EST_SRC21_BYPASS( 120, 0, 2, elw, esw, 0x33001122, 8,  tdat, 1 );
  TEST_EST_SRC21_BYPASS( 121, 1, 0, elw, esw, 0x23300112, 12, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 122, 1, 1, elw, esw, 0x22330011, 16, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 123, 2, 0, elw, esw, 0x12233001, 20, tdat, 1 );

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .word 0xdeadbeef
tdat2:  .word 0xdeadbeef
tdat3:  .word 0xdeadbeef
tdat4:  .word 0xdeadbeef
tdat5:  .word 0xdeadbeef
tdat6:  .word 0xdeadbeef
tdat7:  .word 0xdeadbeef
tdat8:  .word 0xdeadbeef
tdat9:  .word 0xdeadbeef
tdat10: .word 0xdeadbeef

RVTEST_DATA_END
