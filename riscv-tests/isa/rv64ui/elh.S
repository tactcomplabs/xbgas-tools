#*****************************************************************************
# elh.S
#-----------------------------------------------------------------------------
#
# Test elh instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ELD_OP( 2, elh, 0x00000000000000ff, 0,  tdat, 0 );
  TEST_ELD_OP( 3, elh, 0xffffffffffffff00, 2,  tdat, 0 );
  TEST_ELD_OP( 4, elh, 0x0000000000000ff0, 4,  tdat, 0 );
  TEST_ELD_OP( 5, elh, 0xfffffffffffff00f, 6, tdat, 0 );
  TEST_ELD_OP( 102, elh, 0x00000000000000ff, 0,  tdat, 1 );
  TEST_ELD_OP( 103, elh, 0xffffffffffffff00, 2,  tdat, 1 );
  TEST_ELD_OP( 104, elh, 0x0000000000000ff0, 4,  tdat, 1 );
  TEST_ELD_OP( 105, elh, 0xfffffffffffff00f, 6, tdat, 1 );

  # Test with negative offset

  TEST_ELD_OP( 6, elh, 0x00000000000000ff, -6,  tdat4, 0 );
  TEST_ELD_OP( 7, elh, 0xffffffffffffff00, -4,  tdat4, 0 );
  TEST_ELD_OP( 8, elh, 0x0000000000000ff0, -2,  tdat4, 0 );
  TEST_ELD_OP( 9, elh, 0xfffffffffffff00f,  0, tdat4, 0 );
  TEST_ELD_OP( 106, elh, 0x00000000000000ff, -6,  tdat4, 1 );
  TEST_ELD_OP( 107, elh, 0xffffffffffffff00, -4,  tdat4, 1 );
  TEST_ELD_OP( 108, elh, 0x0000000000000ff0, -2,  tdat4, 1 );
  TEST_ELD_OP( 109, elh, 0xfffffffffffff00f,  0, tdat4, 1 );

  # Test with a negative base

  TEST_CASE( 10, x5, 0x00000000000000ff, \
    SET_REMOTE_ADDR(e1, 0)\
    la  x1, tdat; \
    addi x1, x1, -32; \
    elh x5, 32(x1); \
  )
  TEST_CASE( 110, x5, 0x00000000000000ff, \
    SET_REMOTE_ADDR(e1, 1)\
    la  x1, tdat; \
    addi x1, x1, -32; \
    elh x5, 32(x1); \
  )

  # Test with unaligned base

  TEST_CASE( 11, x5, 0xffffffffffffff00, \
    SET_REMOTE_ADDR(e1, 0)\
    la  x1, tdat; \
    addi x1, x1, -5; \
    elh x5, 7(x1); \
  )
  TEST_CASE( 111, x5, 0xffffffffffffff00, \
    SET_REMOTE_ADDR(e1, 1)\
    la  x1, tdat; \
    addi x1, x1, -5; \
    elh x5, 7(x1); \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_ELD_DEST_BYPASS( 12, 0, elh, 0x0000000000000ff0, 2, tdat2, 0 );
  TEST_ELD_DEST_BYPASS( 13, 1, elh, 0xfffffffffffff00f, 2, tdat3, 0 );
  TEST_ELD_DEST_BYPASS( 14, 2, elh, 0xffffffffffffff00, 2, tdat1, 0 );
  TEST_ELD_DEST_BYPASS( 112, 0, elh, 0x0000000000000ff0, 2, tdat2, 1 );
  TEST_ELD_DEST_BYPASS( 113, 1, elh, 0xfffffffffffff00f, 2, tdat3, 1 );
  TEST_ELD_DEST_BYPASS( 114, 2, elh, 0xffffffffffffff00, 2, tdat1, 1 );

  TEST_ELD_SRC1_BYPASS( 15, 0, elh, 0x0000000000000ff0, 2, tdat2, 0 );
  TEST_ELD_SRC1_BYPASS( 16, 1, elh, 0xfffffffffffff00f, 2, tdat3, 0 );
  TEST_ELD_SRC1_BYPASS( 17, 2, elh, 0xffffffffffffff00, 2, tdat1, 0 );
  TEST_ELD_SRC1_BYPASS( 115, 0, elh, 0x0000000000000ff0, 2, tdat2, 1 );
  TEST_ELD_SRC1_BYPASS( 116, 1, elh, 0xfffffffffffff00f, 2, tdat3, 1 );
  TEST_ELD_SRC1_BYPASS( 117, 2, elh, 0xffffffffffffff00, 2, tdat1, 1 );

  #-------------------------------------------------------------
  # Test write-after-write hazard
  #-------------------------------------------------------------

  TEST_CASE( 18, x2, 2, \
    SET_REMOTE_ADDR(e5, 0)\
    la  x5, tdat; \
    elh  x2, 0(x5); \
    li  x2, 2; \
  )
  TEST_CASE( 118, x2, 2, \
    SET_REMOTE_ADDR(e5, 1)\
    la  x5, tdat; \
    elh  x2, 0(x5); \
    li  x2, 2; \
  )

  TEST_CASE( 19, x2, 2, \
    SET_REMOTE_ADDR(e5, 0)\
    la  x5, tdat; \
    elh  x2, 0(x5); \
    nop; \
    li  x2, 2; \
  )
  TEST_CASE( 119, x2, 2, \
    SET_REMOTE_ADDR(e5, 1)\
    la  x5, tdat; \
    elh  x2, 0(x5); \
    nop; \
    li  x2, 2; \
  )

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .half 0x00ff
tdat2:  .half 0xff00
tdat3:  .half 0x0ff0
tdat4:  .half 0xf00f

RVTEST_DATA_END
