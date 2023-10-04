# See LICENSE for license details.

#*****************************************************************************
# lw.S
#-----------------------------------------------------------------------------
#
# Test lw instruction.
#

#include "riscv_test.h"
#include "test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ELD_OP( 2, elw, 0x0000000000ff00ff, 0,  tdat, 0 );
  TEST_ELD_OP( 3, elw, 0xffffffffff00ff00, 4,  tdat, 0 );
  TEST_ELD_OP( 4, elw, 0x000000000ff00ff0, 8,  tdat, 0 );
  TEST_ELD_OP( 5, elw, 0xfffffffff00ff00f, 12, tdat, 0 );
  TEST_ELD_OP( 102, elw, 0x0000000000ff00ff, 0,  tdat, 1 );
  TEST_ELD_OP( 103, elw, 0xffffffffff00ff00, 4,  tdat, 1 );
  TEST_ELD_OP( 104, elw, 0x000000000ff00ff0, 8,  tdat, 1 );
  TEST_ELD_OP( 105, elw, 0xfffffffff00ff00f, 12, tdat, 1 );

  # Test with negative offset

  TEST_ELD_OP( 6, elw, 0x0000000000ff00ff, -12, tdat4, 0 );
  TEST_ELD_OP( 7, elw, 0xffffffffff00ff00, -8,  tdat4, 0 );
  TEST_ELD_OP( 8, elw, 0x000000000ff00ff0, -4,  tdat4, 0 );
  TEST_ELD_OP( 9, elw, 0xfffffffff00ff00f, 0,   tdat4, 0 );
  TEST_ELD_OP( 6, elw, 0x0000000000ff00ff, -12, tdat4, 1 );
  TEST_ELD_OP( 7, elw, 0xffffffffff00ff00, -8,  tdat4, 1 );
  TEST_ELD_OP( 8, elw, 0x000000000ff00ff0, -4,  tdat4, 1 );
  TEST_ELD_OP( 9, elw, 0xfffffffff00ff00f, 0,   tdat4, 1 );

  # # Test with a negative base

  # TEST_CASE( 10, x5, 0x0000000000ff00ff, \
  #   la  x1, tdat; \
  #   addi x1, x1, -32; \
  #   lw x5, 32(x1); \
  # )

  # # Test with unaligned base

  # TEST_CASE( 11, x5, 0xffffffffff00ff00, \
  #   la  x1, tdat; \
  #   addi x1, x1, -3; \
  #   lw x5, 7(x1); \
  # )

  # #-------------------------------------------------------------
  # # Bypassing tests
  # #-------------------------------------------------------------

  # TEST_LD_DEST_BYPASS( 12, 0, lw, 0x000000000ff00ff0, 4, tdat2 );
  # TEST_LD_DEST_BYPASS( 13, 1, lw, 0xfffffffff00ff00f, 4, tdat3 );
  # TEST_LD_DEST_BYPASS( 14, 2, lw, 0xffffffffff00ff00, 4, tdat1 );

  # TEST_LD_SRC1_BYPASS( 15, 0, lw, 0x000000000ff00ff0, 4, tdat2 );
  # TEST_LD_SRC1_BYPASS( 16, 1, lw, 0xfffffffff00ff00f, 4, tdat3 );
  # TEST_LD_SRC1_BYPASS( 17, 2, lw, 0xffffffffff00ff00, 4, tdat1 );

  # #-------------------------------------------------------------
  # # Test write-after-write hazard
  # #-------------------------------------------------------------

  # TEST_CASE( 18, x2, 2, \
  #   la  x5, tdat; \
  #   lw  x2, 0(x5); \
  #   li  x2, 2; \
  # )

  # TEST_CASE( 19, x2, 2, \
  #   la  x5, tdat; \
  #   lw  x2, 0(x5); \
  #   nop; \
  #   li  x2, 2; \
  # )

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .word 0x00ff00ff
tdat2:  .word 0xff00ff00
tdat3:  .word 0x0ff00ff0
tdat4:  .word 0xf00ff00f

RVTEST_DATA_END
