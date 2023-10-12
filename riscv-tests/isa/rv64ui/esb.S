#*****************************************************************************
# esb.S
#-----------------------------------------------------------------------------
#
# Test esb instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_EST_OP( 2, lb, esb, 0xffffffffffffffaa, 0, tdat, 0 );
  TEST_EST_OP( 3, lb, esb, 0x0000000000000000, 1, tdat, 0 );
  TEST_EST_OP( 4, lh, esb, 0xffffffffffffefa0, 2, tdat, 0 );
  TEST_EST_OP( 5, lb, esb, 0x000000000000000a, 3, tdat, 0 );

  PRESET_MEM(esb, 0xef, tdat, 1, 4);
  TEST_EST_OP( 102, elb, esb, 0xffffffffffffffaa, 0, tdat, 1 );
  TEST_EST_OP( 103, elb, esb, 0x0000000000000000, 1, tdat, 1 );
  TEST_EST_OP( 104, elh, esb, 0xffffffffffffefa0, 2, tdat, 1 );
  TEST_EST_OP( 105, elb, esb, 0x000000000000000a, 3, tdat, 1 );
  
  # Test with negative offset

  TEST_EST_OP( 6, lb, esb, 0xffffffffffffffaa, -3, tdat8, 0 );
  TEST_EST_OP( 7, lb, esb, 0x0000000000000000, -2, tdat8, 0 );
  TEST_EST_OP( 8, lb, esb, 0xffffffffffffffa0, -1, tdat8, 0 );
  TEST_EST_OP( 9, lb, esb, 0x000000000000000a, 0,  tdat8, 0 );

  PRESET_MEM(esb, 0xef, tdat, 1, 4);
  TEST_EST_OP( 106, elb, esb, 0xffffffffffffffaa, -3, tdat8, 1 );
  TEST_EST_OP( 107, elb, esb, 0x0000000000000000, -2, tdat8, 1 );
  TEST_EST_OP( 108, elb, esb, 0xffffffffffffffa0, -1, tdat8, 1 );
  TEST_EST_OP( 109, elb, esb, 0x000000000000000a, 0,  tdat8, 1 );

  # Test with a negative base

  PRESET_MEM(esb, 0xef, tdat9, 1, 1);
  TEST_CASE( 10, x5, 0x78, \
    SET_REMOTE_ADDR(e4, 0) \
    la  x1, tdat9; \
    li  x2, 0x12345678; \
    addi x4, x1, -32; \
    esb x2, 32(x4); \
    lb x5, 0(x1); \
  )

  PRESET_MEM(esb, 0xef, tdat9, 1, 1);
  TEST_CASE( 110, x5, 0x78, \
    SET_REMOTE_ADDR(e4, 1) \
    SET_REMOTE_ADDR(e1, 1) \
    la  x1, tdat9; \
    li  x2, 0x12345678; \
    addi x4, x1, -32; \
    esb x2, 32(x4); \
    elb x5, 0(x1); \
  )

  # Test with unaligned base

  PRESET_MEM(esb, 0xef, tdat10, 1, 1);
  TEST_CASE( 11, x5, 0xffffffffffffff98, \
    SET_REMOTE_ADDR(e1, 0) \
    la  x1, tdat9; \
    li  x2, 0x00003098; \
    addi x1, x1, -6; \
    esb x2, 7(x1); \
    la  x4, tdat10; \
    lb x5, 0(x4); \
  )

  PRESET_MEM(esb, 0xef, tdat10, 1, 1);
  TEST_CASE( 111, x5, 0xffffffffffffff98, \
    SET_REMOTE_ADDR(e1, 1) \
    SET_REMOTE_ADDR(e4, 1) \
    la  x1, tdat9; \
    li  x2, 0x00003098; \
    addi x1, x1, -6; \
    esb x2, 7(x1); \
    la  x4, tdat10; \
    elb x5, 0(x4); \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  PRESET_MEM(esb, 0xef, tdat, 1, 5);
  TEST_EST_SRC12_BYPASS( 12, 0, 0, lb, esb, 0xffffffffffffffdd, 0, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 13, 0, 1, lb, esb, 0xffffffffffffffcd, 1, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 14, 0, 2, lb, esb, 0xffffffffffffffcc, 2, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 15, 1, 0, lb, esb, 0xffffffffffffffbc, 3, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 16, 1, 1, lb, esb, 0xffffffffffffffbb, 4, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 17, 2, 0, lb, esb, 0xffffffffffffffab, 5, tdat, 0 );

  PRESET_MEM(esb, 0xef, tdat, 1, 5);
  TEST_EST_SRC12_BYPASS( 112, 0, 0, elb, esb, 0xffffffffffffffdd, 0, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 113, 0, 1, elb, esb, 0xffffffffffffffcd, 1, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 114, 0, 2, elb, esb, 0xffffffffffffffcc, 2, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 115, 1, 0, elb, esb, 0xffffffffffffffbc, 3, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 116, 1, 1, elb, esb, 0xffffffffffffffbb, 4, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 117, 2, 0, elb, esb, 0xffffffffffffffab, 5, tdat, 1 );

  PRESET_MEM(esb, 0xef, tdat, 1, 5);
  TEST_EST_SRC21_BYPASS( 18, 0, 0, lb, esb, 0x33, 0, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 19, 0, 1, lb, esb, 0x23, 1, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 20, 0, 2, lb, esb, 0x22, 2, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 21, 1, 0, lb, esb, 0x12, 3, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 22, 1, 1, lb, esb, 0x11, 4, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 23, 2, 0, lb, esb, 0x01, 5, tdat, 0 );

  PRESET_MEM(esb, 0xef, tdat, 1, 5);
  TEST_EST_SRC21_BYPASS( 118, 0, 0, elb, esb, 0x33, 0, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 119, 0, 1, elb, esb, 0x23, 1, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 120, 0, 2, elb, esb, 0x22, 2, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 121, 1, 0, elb, esb, 0x12, 3, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 122, 1, 1, elb, esb, 0x11, 4, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 123, 2, 0, elb, esb, 0x01, 5, tdat, 1 );

  li a0, 0xef
  la a1, tdat
  esb a0, 3(a1)

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .byte 0xef
tdat2:  .byte 0xef
tdat3:  .byte 0xef
tdat4:  .byte 0xef
tdat5:  .byte 0xef
tdat6:  .byte 0xef
tdat7:  .byte 0xef
tdat8:  .byte 0xef
tdat9:  .byte 0xef
tdat10: .byte 0xef

RVTEST_DATA_END
