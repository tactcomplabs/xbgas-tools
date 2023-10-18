#*****************************************************************************
# erlb.S
#-----------------------------------------------------------------------------
#
# Test erlb instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN
  
  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ERLD_OP( 2, erlb, 0xffffffffffffffff, tdat, 0 );
  TEST_ERLD_OP( 102, erlb, 0xffffffffffffffff, tdat, 1 );

  #-------------------------------------------------------------
  # Bypassing tests
  #-------------------------------------------------------------

  TEST_ERLD_DEST_BYPASS( 12, 0, erlb, 0xfffffffffffffff0, tdat3, 0 );
  TEST_ERLD_DEST_BYPASS( 13, 1, erlb, 0x000000000000000f, tdat4, 0 );
  TEST_ERLD_DEST_BYPASS( 14, 2, erlb, 0x0000000000000000, tdat2, 0 );
  TEST_ERLD_DEST_BYPASS( 112, 0, erlb, 0xfffffffffffffff0, tdat3, 1 );
  TEST_ERLD_DEST_BYPASS( 113, 1, erlb, 0x000000000000000f, tdat4, 1 );
  TEST_ERLD_DEST_BYPASS( 114, 2, erlb, 0x0000000000000000, tdat2, 1 );

  TEST_ERLD_SRC1_BYPASS( 15, 0, erlb, 0xfffffffffffffff0, tdat3, 0 );
  TEST_ERLD_SRC1_BYPASS( 16, 1, erlb, 0x000000000000000f, tdat4, 0 );
  TEST_ERLD_SRC1_BYPASS( 17, 2, erlb, 0x0000000000000000, tdat2, 0 );
  TEST_ERLD_SRC1_BYPASS( 115, 0, erlb, 0xfffffffffffffff0, tdat3, 1 );
  TEST_ERLD_SRC1_BYPASS( 116, 1, erlb, 0x000000000000000f, tdat4, 1 );
  TEST_ERLD_SRC1_BYPASS( 117, 2, erlb, 0x0000000000000000, tdat2, 1 );

  TEST_ERLD_SRC2_BYPASS( 18, 0, erlb, 0xfffffffffffffff0, tdat3, 0 );
  TEST_ERLD_SRC2_BYPASS( 19, 1, erlb, 0x000000000000000f, tdat4, 0 );
  TEST_ERLD_SRC2_BYPASS( 20, 2, erlb, 0x0000000000000000, tdat2, 0 );
  TEST_ERLD_SRC2_BYPASS( 118, 0, erlb, 0xfffffffffffffff0, tdat3, 1 );
  TEST_ERLD_SRC2_BYPASS( 119, 1, erlb, 0x000000000000000f, tdat4, 1 );
  TEST_ERLD_SRC2_BYPASS( 120, 2, erlb, 0x0000000000000000, tdat2, 1 );

  #-------------------------------------------------------------
  # Test write-after-write hazard
  #-------------------------------------------------------------

  TEST_CASE( 21, x2, 2, \
    SET_REMOTE_ADDR(e5, 0) \
    la  x5, tdat; \
    erlb  x2, x5, e5; \
    li  x2, 2; \
  )
  TEST_CASE( 121, x2, 2, \
    SET_REMOTE_ADDR(e5, 1) \
    la  x5, tdat; \
    erlb  x2, x5, e5; \
    li  x2, 2; \
  )

  TEST_CASE( 22, x2, 2, \
    SET_REMOTE_ADDR(e5, 0) \
    la  x5, tdat; \
    erlb  x2, x5, e5; \
    nop; \
    li  x2, 2; \
  )
  TEST_CASE( 122, x2, 2, \
    SET_REMOTE_ADDR(e5, 1) \
    la  x5, tdat; \
    erlb  x2, x5, e5; \
    nop; \
    li  x2, 2; \
  )

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .byte 0xff
tdat2:  .byte 0x00
tdat3:  .byte 0xf0
tdat4:  .byte 0x0f

RVTEST_DATA_END
