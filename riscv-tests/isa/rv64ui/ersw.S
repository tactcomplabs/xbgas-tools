#*****************************************************************************
# ersw.S
#-----------------------------------------------------------------------------
#
# Test ersw instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ERST_OP( 2, erlw, ersw, 0x0000000000aa00aa, tdat, 0 );
  PRESET_MEM(esw, 0xdeadbeef, tdat4, 4, 1);
  TEST_ERST_OP( 102, erlw, ersw, 0x0000000000aa00aa, tdat, 1 );

  # #-------------------------------------------------------------
  # # Bypassing tests
  # #-------------------------------------------------------------

  PRESET_MEM(esw, 0xdeadbeef, tdat, 4, 3);
  TEST_ERST_SRC1_BYPASS( 3, 0, erlw, ersw, 0xffffffffaabbccdd, tdat, 0 );
  TEST_ERST_SRC1_BYPASS( 4, 1, erlw, ersw, 0xffffffffdaabbccd, tdat2, 0 );
  TEST_ERST_SRC1_BYPASS( 5, 2, erlw, ersw, 0xffffffffddaabbcc, tdat3, 0 );
  PRESET_MEM(esw, 0xdeadbeef, tdat, 4, 3);
  TEST_ERST_SRC1_BYPASS( 103, 0, erlw, ersw, 0xffffffffaabbccdd, tdat, 1 );
  TEST_ERST_SRC1_BYPASS( 104, 1, erlw, ersw, 0xffffffffdaabbccd, tdat2, 1 );
  TEST_ERST_SRC1_BYPASS( 105, 2, erlw, ersw, 0xffffffffddaabbcc, tdat3, 1 );

  PRESET_MEM(esw, 0xdeadbeef, tdat4, 4, 3);
  TEST_ERST_SRC2_BYPASS( 6, 0, erlw, ersw, 0x00112233, tdat4, 0 );
  TEST_ERST_SRC2_BYPASS( 7, 1, erlw, ersw, 0x30011223, tdat5, 0 );
  TEST_ERST_SRC2_BYPASS( 8, 2, erlw, ersw, 0x33001122, tdat6, 0 );
  PRESET_MEM(esw, 0xdeadbeef, tdat4, 4, 3);
  TEST_ERST_SRC2_BYPASS( 106, 0, erlw, ersw, 0x00112233, tdat4, 1 );
  TEST_ERST_SRC2_BYPASS( 107, 1, erlw, ersw, 0x30011223, tdat5, 1 );
  TEST_ERST_SRC2_BYPASS( 108, 2, erlw, ersw, 0x33001122, tdat6, 1 );

  PRESET_MEM(esw, 0xdeadbeef, tdat7, 4, 3);
  TEST_ERST_SRC3_BYPASS( 9, 0, erlw, ersw, 0xffffffffaabbccdd, tdat7, 0 );
  TEST_ERST_SRC3_BYPASS( 10, 1, erlw, ersw, 0xffffffffdaabbccd, tdat8, 0 );
  TEST_ERST_SRC3_BYPASS( 11, 2, erlw, ersw, 0xffffffffddaabbcc, tdat9, 0 );
  PRESET_MEM(esw, 0xdeadbeef, tdat7, 4, 3);
  TEST_ERST_SRC3_BYPASS( 109, 0, erlw, ersw, 0xffffffffaabbccdd, tdat7, 1 );
  TEST_ERST_SRC3_BYPASS( 110, 1, erlw, ersw, 0xffffffffdaabbccd, tdat8, 1 );
  TEST_ERST_SRC3_BYPASS( 111, 2, erlw, ersw, 0xffffffffddaabbcc, tdat9, 1 );

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
