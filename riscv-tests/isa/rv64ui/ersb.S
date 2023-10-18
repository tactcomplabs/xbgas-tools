#*****************************************************************************
# ersb.S
#-----------------------------------------------------------------------------
#
# Test ersb instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ERST_OP( 2, erlb, ersb, 0xffffffffffffffaa, tdat, 0 );
  TEST_ERST_OP( 12, erlb, ersb, 0x0000000000000000, tdat2, 0 );
  PRESET_MEM(esb, 0xef, tdat, 1, 2);
  TEST_ERST_OP( 102, erlb, ersb, 0xffffffffffffffaa, tdat, 1 );
  TEST_ERST_OP( 112, erlb, ersb, 0x0000000000000000, tdat2, 1 );

  # #-------------------------------------------------------------
  # # Bypassing tests
  # #-------------------------------------------------------------

  PRESET_MEM(esb, 0xef, tdat, 1, 3);
  TEST_ERST_SRC1_BYPASS( 3, 0, erlb, ersb, 0xffffffffffffffdd, tdat, 0 );
  TEST_ERST_SRC1_BYPASS( 4, 1, erlb, ersb, 0xffffffffffffffcd, tdat2, 0 );
  TEST_ERST_SRC1_BYPASS( 5, 2, erlb, ersb, 0xffffffffffffffcc, tdat3, 0 );
  PRESET_MEM(esb, 0xef, tdat, 1, 3);
  TEST_ERST_SRC1_BYPASS( 103, 0, erlb, ersb, 0xffffffffffffffdd, tdat, 1 );
  TEST_ERST_SRC1_BYPASS( 104, 1, erlb, ersb, 0xffffffffffffffcd, tdat2, 1 );
  TEST_ERST_SRC1_BYPASS( 105, 2, erlb, ersb, 0xffffffffffffffcc, tdat3, 1 );

  PRESET_MEM(esb, 0xef, tdat4, 1, 3);
  TEST_ERST_SRC2_BYPASS( 6, 0, erlb, ersb, 0x33, tdat4, 0 );
  TEST_ERST_SRC2_BYPASS( 7, 1, erlb, ersb, 0x23, tdat5, 0 );
  TEST_ERST_SRC2_BYPASS( 8, 2, erlb, ersb, 0x22, tdat6, 0 );
  PRESET_MEM(esb, 0xef, tdat4, 1, 3);
  TEST_ERST_SRC2_BYPASS( 106, 0, erlb, ersb, 0x33, tdat4, 1 );
  TEST_ERST_SRC2_BYPASS( 107, 1, erlb, ersb, 0x23, tdat5, 1 );
  TEST_ERST_SRC2_BYPASS( 108, 2, erlb, ersb, 0x22, tdat6, 1 );

  PRESET_MEM(esb, 0xef, tdat7, 1, 3);
  TEST_ERST_SRC3_BYPASS( 9, 0, erlb, ersb, 0xffffffffffffffdd, tdat7, 0 );
  TEST_ERST_SRC3_BYPASS( 10, 1, erlb, ersb, 0xffffffffffffffcd, tdat8, 0 );
  TEST_ERST_SRC3_BYPASS( 11, 2, erlb, ersb, 0xffffffffffffffcc, tdat9, 0 );
  PRESET_MEM(esb, 0xef, tdat7, 1, 3);
  TEST_ERST_SRC3_BYPASS( 109, 0, erlb, ersb, 0xffffffffffffffdd, tdat7, 1 );
  TEST_ERST_SRC3_BYPASS( 110, 1, erlb, ersb, 0xffffffffffffffcd, tdat8, 1 );
  TEST_ERST_SRC3_BYPASS( 111, 2, erlb, ersb, 0xffffffffffffffcc, tdat9, 1 );

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
