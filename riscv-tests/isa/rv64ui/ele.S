#*****************************************************************************
# ele.S
#-----------------------------------------------------------------------------
#
# Test ele instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN
  
  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ELE_OP( 2, ele, 0x00ff00ff00ff00ff, 0,  tdat, 0 );
  TEST_ELE_OP( 3, ele, 0xff00ff00ff00ff00, 8,  tdat, 0 );
  TEST_ELE_OP( 4, ele, 0x0ff00ff00ff00ff0, 16, tdat, 0 );
  TEST_ELE_OP( 5, ele, 0xf00ff00ff00ff00f, 24, tdat, 0 );
  TEST_ELE_OP( 102, ele, 0x00ff00ff00ff00ff, 0,  tdat, 1 );
  TEST_ELE_OP( 103, ele, 0xff00ff00ff00ff00, 8,  tdat, 1 );
  TEST_ELE_OP( 104, ele, 0x0ff00ff00ff00ff0, 16, tdat, 1 );
  TEST_ELE_OP( 105, ele, 0xf00ff00ff00ff00f, 24, tdat, 1 );

  # Test with negative offset

  TEST_ELE_OP( 6, ele, 0x00ff00ff00ff00ff, -24, tdat4, 0 );
  TEST_ELE_OP( 7, ele, 0xff00ff00ff00ff00, -16, tdat4, 0 );
  TEST_ELE_OP( 8, ele, 0x0ff00ff00ff00ff0, -8,  tdat4, 0 );
  TEST_ELE_OP( 9, ele, 0xf00ff00ff00ff00f, 0,   tdat4, 0 );
  TEST_ELE_OP( 106, ele, 0x00ff00ff00ff00ff, -24, tdat4, 1 );
  TEST_ELE_OP( 107, ele, 0xff00ff00ff00ff00, -16, tdat4, 1 );
  TEST_ELE_OP( 108, ele, 0x0ff00ff00ff00ff0, -8,  tdat4, 1 );
  TEST_ELE_OP( 109, ele, 0xf00ff00ff00ff00f, 0,   tdat4, 1 );

  # Test with a negative base

  TEST_CASE( 10, x5, 0x00ff00ff00ff00ff, \
    SET_REMOTE_ADDR(e1, 0) \
    la  x1, tdat; \
    addi x1, x1, -32; \
    ele e5, 32(x1); \
    eaddi x5, e5, 0; \
  )
  TEST_CASE( 110, x5, 0x00ff00ff00ff00ff, \
    SET_REMOTE_ADDR(e1, 1) \
    la  x1, tdat; \
    addi x1, x1, -32; \
    ele e5, 32(x1); \
    eaddi x5, e5, 0; \
  )

  # Test with unaligned base

  TEST_CASE( 11, x5, 0xff00ff00ff00ff00, \
    SET_REMOTE_ADDR(e1, 0) \
    la  x1, tdat; \
    addi x1, x1, -3; \
    ele e5, 11(x1); \
    eaddi x5, e5, 0; \
   )
  TEST_CASE( 111, x5, 0xff00ff00ff00ff00, \
    SET_REMOTE_ADDR(e1, 1) \
    la  x1, tdat; \
    addi x1, x1, -3; \
    ele e5, 11(x1); \
    eaddi x5, e5, 0; \
  )

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_ELE_DEST_BYPASS( 12, 0, ele, 0x0ff00ff00ff00ff0, 8, tdat2, 0 );
  TEST_ELE_DEST_BYPASS( 13, 1, ele, 0xf00ff00ff00ff00f, 8, tdat3, 0 );
  TEST_ELE_DEST_BYPASS( 14, 2, ele, 0xff00ff00ff00ff00, 8, tdat1, 0 );
  TEST_ELE_DEST_BYPASS( 112, 0, ele, 0x0ff00ff00ff00ff0, 8, tdat2, 1 );
  TEST_ELE_DEST_BYPASS( 113, 1, ele, 0xf00ff00ff00ff00f, 8, tdat3, 1 );
  TEST_ELE_DEST_BYPASS( 114, 2, ele, 0xff00ff00ff00ff00, 8, tdat1, 1 );

  TEST_ELE_SRC1_BYPASS( 15, 0, ele, 0x0ff00ff00ff00ff0, 8, tdat2, 0 );
  TEST_ELE_SRC1_BYPASS( 16, 1, ele, 0xf00ff00ff00ff00f, 8, tdat3, 0 );
  TEST_ELE_SRC1_BYPASS( 17, 2, ele, 0xff00ff00ff00ff00, 8, tdat1, 0 );
  TEST_ELE_SRC1_BYPASS( 115, 0, ele, 0x0ff00ff00ff00ff0, 8, tdat2, 1 );
  TEST_ELE_SRC1_BYPASS( 116, 1, ele, 0xf00ff00ff00ff00f, 8, tdat3, 1 );
  TEST_ELE_SRC1_BYPASS( 117, 2, ele, 0xff00ff00ff00ff00, 8, tdat1, 1 );

  #-------------------------------------------------------------
  # Test write-after-write hazard
  #-------------------------------------------------------------

  TEST_CASE( 18, x2, 2, \
    SET_REMOTE_ADDR(e5, 0) \
    la  x5, tdat; \
    ele  e2, 0(x5); \
    eaddie e2, x1, 2; \
    eaddi x2, e2, 0; \
  )
  TEST_CASE( 118, x2, 2, \
    SET_REMOTE_ADDR(e5, 1) \
    la  x5, tdat; \
    ele  e2, 0(x5); \
    eaddie e2, x1, 1; \
    eaddi x2, e2, 0; \
  )

  TEST_CASE( 19, x2, 2, \
    SET_REMOTE_ADDR(e5, 0) \
    la  x5, tdat; \
    ele  e2, 0(x5); \
    nop; \
    eaddie e2, x1, 2; \
    eaddi x2, e2, 0; \
  )
  TEST_CASE( 119, x2, 2, \
    SET_REMOTE_ADDR(e5, 1) \
    la  x5, tdat; \
    ele  e2, 0(x5); \
    nop; \
    eaddie e2, x1, 1; \
    eaddi x2, e2, 0; \
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
