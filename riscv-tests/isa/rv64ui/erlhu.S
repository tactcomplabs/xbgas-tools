#*****************************************************************************
# erlhu.S
#-----------------------------------------------------------------------------
#
# Test erlhu instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN
  
  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ERLD_OP( 2, erlhu, 0x00000000000000ff, tdat, 0 );
  TEST_ERLD_OP( 102, erlhu, 0x00000000000000ff, tdat, 1 );

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_ERLD_DEST_BYPASS( 12, 0, erlhu, 0x0000000000000ff0, tdat3, 0 );
  TEST_ERLD_DEST_BYPASS( 13, 1, erlhu, 0x000000000000f00f, tdat4, 0 );
  TEST_ERLD_DEST_BYPASS( 14, 2, erlhu, 0x000000000000ff00, tdat2, 0 );
  TEST_ERLD_DEST_BYPASS( 112, 0, erlhu, 0x0000000000000ff0, tdat3, 1 );
  TEST_ERLD_DEST_BYPASS( 113, 1, erlhu, 0x000000000000f00f, tdat4, 1 );
  TEST_ERLD_DEST_BYPASS( 114, 2, erlhu, 0x000000000000ff00, tdat2, 1 );

  TEST_ERLD_SRC1_BYPASS( 15, 0, erlhu, 0x0000000000000ff0, tdat3, 0 );
  TEST_ERLD_SRC1_BYPASS( 16, 1, erlhu, 0x000000000000f00f, tdat4, 0 );
  TEST_ERLD_SRC1_BYPASS( 17, 2, erlhu, 0x000000000000ff00, tdat2, 0 );
  TEST_ERLD_SRC1_BYPASS( 115, 0, erlhu, 0x0000000000000ff0, tdat3, 1 );
  TEST_ERLD_SRC1_BYPASS( 116, 1, erlhu, 0x000000000000f00f, tdat4, 1 );
  TEST_ERLD_SRC1_BYPASS( 117, 2, erlhu, 0x000000000000ff00, tdat2, 1 );

  TEST_ERLD_SRC2_BYPASS( 18, 0, erlhu, 0x0000000000000ff0, tdat3, 0 );
  TEST_ERLD_SRC2_BYPASS( 19, 1, erlhu, 0x000000000000f00f, tdat4, 0 );
  TEST_ERLD_SRC2_BYPASS( 20, 2, erlhu, 0x000000000000ff00, tdat2, 0 );
  TEST_ERLD_SRC2_BYPASS( 118, 0, erlhu, 0x0000000000000ff0, tdat3, 1 );
  TEST_ERLD_SRC2_BYPASS( 119, 1, erlhu, 0x000000000000f00f, tdat4, 1 );
  TEST_ERLD_SRC2_BYPASS( 120, 2, erlhu, 0x000000000000ff00, tdat2, 1 );

  #-------------------------------------------------------------
  # Test write-after-write hazard
  #-------------------------------------------------------------

  TEST_CASE( 21, x2, 2, \
    SET_REMOTE_ADDR(e5, 0) \
    la  x5, tdat; \
    erlhu  x2, x5, e5; \
    li  x2, 2; \
  )
  TEST_CASE( 121, x2, 2, \
    SET_REMOTE_ADDR(e5, 1) \
    la  x5, tdat; \
    erlhu  x2, x5, e5; \
    li  x2, 2; \
  )

  TEST_CASE( 22, x2, 2, \
    SET_REMOTE_ADDR(e5, 0) \
    la  x5, tdat; \
    erlhu  x2, x5, e5; \
    nop; \
    li  x2, 2; \
  )
  TEST_CASE( 122, x2, 2, \
    SET_REMOTE_ADDR(e5, 1) \
    la  x5, tdat; \
    erlhu  x2, x5, e5; \
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
