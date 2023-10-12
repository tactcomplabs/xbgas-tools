#*****************************************************************************
# esh.S
#-----------------------------------------------------------------------------
#
# Test esh instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_EST_OP( 2, lh, esh, 0x00000000000000aa, 0, tdat, 0 );
  TEST_EST_OP( 3, lh, esh, 0xffffffffffffaa00, 2, tdat, 0 );
  TEST_EST_OP( 4, lw, esh, 0xffffffffbeef0aa0, 4, tdat, 0 );
  TEST_EST_OP( 5, lh, esh, 0xffffffffffffa00a, 6, tdat, 0 );

  PRESET_MEM(esh, 0xbeef, tdat, 2, 4);
  TEST_EST_OP( 102, elh, esh, 0x00000000000000aa, 0, tdat, 1 );
  TEST_EST_OP( 103, elh, esh, 0xffffffffffffaa00, 2, tdat, 1 );
  TEST_EST_OP( 104, elw, esh, 0xffffffffbeef0aa0, 4, tdat, 1 );
  TEST_EST_OP( 105, elh, esh, 0xffffffffffffa00a, 6, tdat, 1 );

  # Test with negative offset

  TEST_EST_OP( 6, lh, esh, 0x00000000000000aa, -6, tdat8, 0 );
  TEST_EST_OP( 7, lh, esh, 0xffffffffffffaa00, -4, tdat8, 0 );
  TEST_EST_OP( 8, lh, esh, 0x0000000000000aa0, -2, tdat8, 0 );
  TEST_EST_OP( 9, lh, esh, 0xffffffffffffa00a, 0,  tdat8, 0 );
  
  PRESET_MEM(esh, 0xbeef, tdat5, 2, 4);
  TEST_EST_OP( 106, elh, esh, 0x00000000000000aa, -6, tdat8, 1 );
  TEST_EST_OP( 107, elh, esh, 0xffffffffffffaa00, -4, tdat8, 1 );
  TEST_EST_OP( 108, elh, esh, 0x0000000000000aa0, -2, tdat8, 1 );
  TEST_EST_OP( 109, elh, esh, 0xffffffffffffa00a, 0,  tdat8, 1 );

  # Test with a negative base

  TEST_CASE( 10, x5, 0x5678, \
    SET_REMOTE_ADDR(e4, 0)\
    la  x1, tdat9; \
    li  x2, 0x12345678; \
    addi x4, x1, -32; \
    esh x2, 32(x4); \
    lh x5, 0(x1); \
  )
  
  PRESET_MEM(esh, 0xbeef, tdat9, 2, 1);
  TEST_CASE( 110, x5, 0x5678, \
    SET_REMOTE_ADDR(e4, 1) \
    SET_REMOTE_ADDR(e1, 1) \
    la  x1, tdat9; \
    li  x2, 0x12345678; \
    addi x4, x1, -32; \
    esh x2, 32(x4); \
    elh x5, 0(x1); \
  )

  # Test with unaligned base

  TEST_CASE( 11, x5, 0x3098, \
    SET_REMOTE_ADDR(e1, 0) \
    la  x1, tdat9; \
    li  x2, 0x00003098; \
    addi x1, x1, -5; \
    esh x2, 7(x1); \
    la  x4, tdat10; \
    lh x5, 0(x4); \
  )
  
  PRESET_MEM(esh, 0xbeef, tdat10, 2, 1);
  TEST_CASE( 111, x5, 0x3098, \
    SET_REMOTE_ADDR(e1, 1) \
    SET_REMOTE_ADDR(e4, 1) \
    la  x1, tdat9; \
    li  x2, 0x00003098; \
    addi x1, x1, -5; \
    esh x2, 7(x1); \
    la  x4, tdat10; \
    elh x5, 0(x4); \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_EST_SRC12_BYPASS( 12, 0, 0, lh, esh, 0xffffffffffffccdd, 0,  tdat, 0 );
  TEST_EST_SRC12_BYPASS( 13, 0, 1, lh, esh, 0xffffffffffffbccd, 2,  tdat, 0 );
  TEST_EST_SRC12_BYPASS( 14, 0, 2, lh, esh, 0xffffffffffffbbcc, 4,  tdat, 0 );
  TEST_EST_SRC12_BYPASS( 15, 1, 0, lh, esh, 0xffffffffffffabbc, 6, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 16, 1, 1, lh, esh, 0xffffffffffffaabb, 8, tdat, 0 );
  TEST_EST_SRC12_BYPASS( 17, 2, 0, lh, esh, 0xffffffffffffdaab, 10, tdat, 0 );
  
  PRESET_MEM(esh, 0xbeef, tdat, 2, 6);
  TEST_EST_SRC12_BYPASS( 112, 0, 0, elh, esh, 0xffffffffffffccdd, 0,  tdat, 1 );
  TEST_EST_SRC12_BYPASS( 113, 0, 1, elh, esh, 0xffffffffffffbccd, 2,  tdat, 1 );
  TEST_EST_SRC12_BYPASS( 114, 0, 2, elh, esh, 0xffffffffffffbbcc, 4,  tdat, 1 );
  TEST_EST_SRC12_BYPASS( 115, 1, 0, elh, esh, 0xffffffffffffabbc, 6, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 116, 1, 1, elh, esh, 0xffffffffffffaabb, 8, tdat, 1 );
  TEST_EST_SRC12_BYPASS( 117, 2, 0, elh, esh, 0xffffffffffffdaab, 10, tdat, 1 );

  TEST_EST_SRC21_BYPASS( 18, 0, 0, lh, esh, 0x2233, 0,  tdat, 0 );
  TEST_EST_SRC21_BYPASS( 19, 0, 1, lh, esh, 0x1223, 2,  tdat, 0 );
  TEST_EST_SRC21_BYPASS( 20, 0, 2, lh, esh, 0x1122, 4,  tdat, 0 );
  TEST_EST_SRC21_BYPASS( 21, 1, 0, lh, esh, 0x0112, 6, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 22, 1, 1, lh, esh, 0x0011, 8, tdat, 0 );
  TEST_EST_SRC21_BYPASS( 23, 2, 0, lh, esh, 0x3001, 10, tdat, 0 );
  
  PRESET_MEM(esh, 0xbeef, tdat, 2, 6);
  TEST_EST_SRC21_BYPASS( 118, 0, 0, elh, esh, 0x2233, 0,  tdat, 1 );
  TEST_EST_SRC21_BYPASS( 119, 0, 1, elh, esh, 0x1223, 2,  tdat, 1 );
  TEST_EST_SRC21_BYPASS( 120, 0, 2, elh, esh, 0x1122, 4,  tdat, 1 );
  TEST_EST_SRC21_BYPASS( 121, 1, 0, elh, esh, 0x0112, 6, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 122, 1, 1, elh, esh, 0x0011, 8, tdat, 1 );
  TEST_EST_SRC21_BYPASS( 123, 2, 0, elh, esh, 0x3001, 10, tdat, 1 );

  li a0, 0xbeef
  la a1, tdat
  esh a0, 6(a1)

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .half 0xbeef
tdat2:  .half 0xbeef
tdat3:  .half 0xbeef
tdat4:  .half 0xbeef
tdat5:  .half 0xbeef
tdat6:  .half 0xbeef
tdat7:  .half 0xbeef
tdat8:  .half 0xbeef
tdat9:  .half 0xbeef
tdat10: .half 0xbeef

RVTEST_DATA_END
