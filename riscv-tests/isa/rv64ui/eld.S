#*****************************************************************************
# eld.S
#-----------------------------------------------------------------------------
#
# Test eld instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN
  
  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ELD_OP( 2, eld, 0x00ff00ff00ff00ff, 0,  tdat, 0 );
  TEST_ELD_OP( 3, eld, 0xff00ff00ff00ff00, 8,  tdat, 0 );
  TEST_ELD_OP( 4, eld, 0x0ff00ff00ff00ff0, 16, tdat, 0 );
  TEST_ELD_OP( 5, eld, 0xf00ff00ff00ff00f, 24, tdat, 0 );
  TEST_ELD_OP( 102, eld, 0x00ff00ff00ff00ff, 0,  tdat, 1 );
  TEST_ELD_OP( 103, eld, 0xff00ff00ff00ff00, 8,  tdat, 1 );
  TEST_ELD_OP( 104, eld, 0x0ff00ff00ff00ff0, 16, tdat, 1 );
  TEST_ELD_OP( 105, eld, 0xf00ff00ff00ff00f, 24, tdat, 1 );

  # Test with negative offset

  TEST_ELD_OP( 6, eld, 0x00ff00ff00ff00ff, -24, tdat4, 0 );
  TEST_ELD_OP( 7, eld, 0xff00ff00ff00ff00, -16, tdat4, 0 );
  TEST_ELD_OP( 8, eld, 0x0ff00ff00ff00ff0, -8,  tdat4, 0 );
  TEST_ELD_OP( 9, eld, 0xf00ff00ff00ff00f, 0,   tdat4, 0 );
  TEST_ELD_OP( 106, eld, 0x00ff00ff00ff00ff, -24, tdat4, 1 );
  TEST_ELD_OP( 107, eld, 0xff00ff00ff00ff00, -16, tdat4, 1 );
  TEST_ELD_OP( 108, eld, 0x0ff00ff00ff00ff0, -8,  tdat4, 1 );
  TEST_ELD_OP( 109, eld, 0xf00ff00ff00ff00f, 0,   tdat4, 1 );

  # Test with a negative base

  TEST_CASE( 10, x5, 0x00ff00ff00ff00ff, \
    SET_REMOTE_ADDR(e1, 0) \
    la  x1, tdat; \
    addi x1, x1, -32; \
    eld x5, 32(x1); \
  )
  TEST_CASE( 110, x5, 0x00ff00ff00ff00ff, \
    SET_REMOTE_ADDR(e1, 1) \
    la  x1, tdat; \
    addi x1, x1, -32; \
    eld x5, 32(x1); \
  )

  # Test with unaligned base

  TEST_CASE( 11, x5, 0xff00ff00ff00ff00, \
    SET_REMOTE_ADDR(e1, 0) \
    la  x1, tdat; \
    addi x1, x1, -3; \
    eld x5, 11(x1); \
   )
  TEST_CASE( 111, x5, 0xff00ff00ff00ff00, \
    SET_REMOTE_ADDR(e1, 1) \
    la  x1, tdat; \
    addi x1, x1, -3; \
    eld x5, 11(x1); \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_ELD_DEST_BYPASS( 12, 0, eld, 0x0ff00ff00ff00ff0, 8, tdat2, 0 );
  TEST_ELD_DEST_BYPASS( 13, 1, eld, 0xf00ff00ff00ff00f, 8, tdat3, 0 );
  TEST_ELD_DEST_BYPASS( 14, 2, eld, 0xff00ff00ff00ff00, 8, tdat1, 0 );
  TEST_ELD_DEST_BYPASS( 112, 0, eld, 0x0ff00ff00ff00ff0, 8, tdat2, 1 );
  TEST_ELD_DEST_BYPASS( 113, 1, eld, 0xf00ff00ff00ff00f, 8, tdat3, 1 );
  TEST_ELD_DEST_BYPASS( 114, 2, eld, 0xff00ff00ff00ff00, 8, tdat1, 1 );

  TEST_ELD_SRC1_BYPASS( 15, 0, eld, 0x0ff00ff00ff00ff0, 8, tdat2, 0 );
  TEST_ELD_SRC1_BYPASS( 16, 1, eld, 0xf00ff00ff00ff00f, 8, tdat3, 0 );
  TEST_ELD_SRC1_BYPASS( 17, 2, eld, 0xff00ff00ff00ff00, 8, tdat1, 0 );
  TEST_ELD_SRC1_BYPASS( 115, 0, eld, 0x0ff00ff00ff00ff0, 8, tdat2, 1 );
  TEST_ELD_SRC1_BYPASS( 116, 1, eld, 0xf00ff00ff00ff00f, 8, tdat3, 1 );
  TEST_ELD_SRC1_BYPASS( 117, 2, eld, 0xff00ff00ff00ff00, 8, tdat1, 1 );

  #-------------------------------------------------------------
  # Test write-after-write hazard
  #-------------------------------------------------------------

  TEST_CASE( 18, x2, 2, \
    SET_REMOTE_ADDR(e5, 0) \
    la  x5, tdat; \
    eld  x2, 0(x5); \
    li  x2, 2; \
  )
  TEST_CASE( 118, x2, 2, \
    SET_REMOTE_ADDR(e5, 1) \
    la  x5, tdat; \
    eld  x2, 0(x5); \
    li  x2, 2; \
  )

  TEST_CASE( 19, x2, 2, \
    SET_REMOTE_ADDR(e5, 0) \
    la  x5, tdat; \
    eld  x2, 0(x5); \
    nop; \
    li  x2, 2; \
  )
  TEST_CASE( 119, x2, 2, \
    SET_REMOTE_ADDR(e5, 1) \
    la  x5, tdat; \
    eld  x2, 0(x5); \
    nop; \
    li  x2, 2; \
  )

  #----------
  # Successive loads
  #----------

  TEST_CASE(121, x4, 0xf00ff00ff00ff00f, \
    SET_REMOTE_ADDR(e8, 1) \
    la x8, tdat; \
    eld x4, 0(x8); \
    eld x4, 8(x8); \
    eld x4, 16(x8); \
    eld x4, 24(x8); \
  )
  
  TEST_CASE(120, x2, 2, \
    SET_REMOTE_ADDR(e8, 1) \
    la x8, tdat; \
    eld x4, 0(x8); \
    eld x5, 8(x8); \
    eld x6, 16(x8); \
    eld x7, 24(x8); \
    srai x7, x7, 4; \
    bne x7, x5, fail; \
    srli x7, x7, 4; \
    bne x7, x6, fail; \
    srli x7, x7, 4; \
    bne x7, x4, fail; \
    li x2, 2; \
  )



  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .dword 0x00ff00ff00ff00ff
tdat2:  .dword 0xff00ff00ff00ff00
tdat3:  .dword 0x0ff00ff00ff00ff0
tdat4:  .dword 0xf00ff00ff00ff00f

RVTEST_DATA_END
