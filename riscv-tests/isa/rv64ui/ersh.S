#*****************************************************************************
# ersh.S
#-----------------------------------------------------------------------------
#
# Test ersh instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ERST_OP( 2, erlh, ersh, 0x00000000000000aa, tdat, 0 );
  TEST_ERST_OP( 12, erlh, ersh, 0xffffffffffffaa00, tdat2, 0 );
  TEST_ERST_OP( 13, erlw, ersh, 0xffffffffbeef0aa0, tdat3, 0 );
  PRESET_MEM(esh, 0xbeef, tdat, 2, 3);
  TEST_ERST_OP( 102, erlh, ersh, 0x00000000000000aa, tdat, 1 );
  TEST_ERST_OP( 112, erlh, ersh, 0xffffffffffffaa00, tdat2, 1 );
  TEST_ERST_OP( 113, erlw, ersh, 0xffffffffbeef0aa0, tdat3, 1 );

  # #-------------------------------------------------------------
  # # Bypassing tests
  # #-------------------------------------------------------------

  PRESET_MEM(esh, 0xbeef, tdat, 2, 3);
  TEST_ERST_SRC1_BYPASS( 3, 0, erlh, ersh, 0xffffffffffffccdd, tdat, 0 );
  TEST_ERST_SRC1_BYPASS( 4, 1, erlh, ersh, 0xffffffffffffbccd, tdat2, 0 );
  TEST_ERST_SRC1_BYPASS( 5, 2, erlh, ersh,  0xffffffffffffbbcc, tdat3, 0 );
  PRESET_MEM(esh, 0xbeef, tdat, 2, 3);
  TEST_ERST_SRC1_BYPASS( 103, 0, erlh, ersh, 0xffffffffffffccdd, tdat, 1 );
  TEST_ERST_SRC1_BYPASS( 104, 1, erlh, ersh, 0xffffffffffffbccd, tdat2, 1 );
  TEST_ERST_SRC1_BYPASS( 105, 2, erlh, ersh,  0xffffffffffffbbcc, tdat3, 1 );

  PRESET_MEM(esh, 0xbeef, tdat4, 2, 3);
  TEST_ERST_SRC2_BYPASS( 6, 0, erlh, ersh, 0x2233, tdat4, 0 );
  TEST_ERST_SRC2_BYPASS( 7, 1, erlh, ersh, 0x1223, tdat5, 0 );
  TEST_ERST_SRC2_BYPASS( 8, 2, erlh, ersh, 0x1122, tdat6, 0 );
  PRESET_MEM(esh, 0xbeef, tdat4, 2, 3);
  TEST_ERST_SRC2_BYPASS( 106, 0, erlh, ersh, 0x2233, tdat4, 1 );
  TEST_ERST_SRC2_BYPASS( 107, 1, erlh, ersh, 0x1223, tdat5, 1 );
  TEST_ERST_SRC2_BYPASS( 108, 2, erlh, ersh, 0x1122, tdat6, 1 );

  PRESET_MEM(esh, 0xbeef, tdat7, 2, 3);
  TEST_ERST_SRC3_BYPASS( 9, 0, erlh, ersh, 0xffffffffffffccdd, tdat7, 0 );
  TEST_ERST_SRC3_BYPASS( 10, 1, erlh, ersh, 0xffffffffffffbccd, tdat8, 0 );
  TEST_ERST_SRC3_BYPASS( 11, 2, erlh, ersh,  0xffffffffffffbbcc, tdat9, 0 );
  PRESET_MEM(esh, 0xbeef, tdat7, 2, 3);
  TEST_ERST_SRC3_BYPASS( 109, 0, erlh, ersh, 0xffffffffffffccdd, tdat7, 1 );
  TEST_ERST_SRC3_BYPASS( 110, 1, erlh, ersh, 0xffffffffffffbccd, tdat8, 1 );
  TEST_ERST_SRC3_BYPASS( 111, 2, erlh, ersh,  0xffffffffffffbbcc, tdat9, 1 );

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
