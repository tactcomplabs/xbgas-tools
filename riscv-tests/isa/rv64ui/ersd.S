#*****************************************************************************
# ersd.S
#-----------------------------------------------------------------------------
#
# Test ersd instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  #-------------------------------------------------------------
  # Basic tests
  #-------------------------------------------------------------

  TEST_ERST_OP( 2, erld, ersd, 0x00aa00aa00aa00aa, tdat, 0 );
  PRESET_MEM(esd, 0xdeadbeefdeadbeef, tdat, 8, 1);
  TEST_ERST_OP( 102, erld, ersd, 0x00aa00aa00aa00aa, tdat, 1 );

  # #-------------------------------------------------------------
  # # Bypassing tests
  # #-------------------------------------------------------------

  PRESET_MEM(esd, 0xdeadbeefdeadbeef, tdat, 8, 3);
  TEST_ERST_SRC1_BYPASS( 3, 0, erld, ersd, 0xabbccdd, tdat, 0 );
  TEST_ERST_SRC1_BYPASS( 4, 1, erld, ersd, 0xaabbccd, tdat2, 0 );
  TEST_ERST_SRC1_BYPASS( 5, 2, erld, ersd, 0xdaabbcc, tdat3, 0 );
  PRESET_MEM(esd, 0xdeadbeefdeadbeef, tdat, 8, 3);
  TEST_ERST_SRC1_BYPASS( 103, 0, erld, ersd, 0xabbccdd, tdat, 1 );
  TEST_ERST_SRC1_BYPASS( 104, 1, erld, ersd, 0xaabbccd, tdat2, 1 );
  TEST_ERST_SRC1_BYPASS( 105, 2, erld, ersd, 0xdaabbcc, tdat3, 1 );

  PRESET_MEM(esd, 0xdeadbeefdeadbeef, tdat4, 8, 3);
  TEST_ERST_SRC2_BYPASS( 6, 0, erld, ersd, 0xabbccdd, tdat4, 0 );
  TEST_ERST_SRC2_BYPASS( 7, 1, erld, ersd, 0xaabbccd, tdat5, 0 );
  TEST_ERST_SRC2_BYPASS( 8, 2, erld, ersd, 0xdaabbcc, tdat6, 0 );
  PRESET_MEM(esd, 0xdeadbeefdeadbeef, tdat4, 8, 3);
  TEST_ERST_SRC2_BYPASS( 106, 0, erld, ersd, 0xabbccdd, tdat4, 1 );
  TEST_ERST_SRC2_BYPASS( 107, 1, erld, ersd, 0xaabbccd, tdat5, 1 );
  TEST_ERST_SRC2_BYPASS( 108, 2, erld, ersd, 0xdaabbcc, tdat6, 1 );

  PRESET_MEM(esd, 0xdeadbeefdeadbeef, tdat7, 8, 3);
  TEST_ERST_SRC3_BYPASS( 9, 0, erld, ersd, 0xabbccdd, tdat7, 0 );
  TEST_ERST_SRC3_BYPASS( 10, 1, erld, ersd, 0xaabbccd, tdat8, 0 );
  TEST_ERST_SRC3_BYPASS( 11, 2, erld, ersd, 0xdaabbcc, tdat9, 0 );
  PRESET_MEM(esd, 0xdeadbeefdeadbeef, tdat7, 8, 3);
  TEST_ERST_SRC3_BYPASS( 109, 0, erld, ersd, 0xabbccdd, tdat7, 1 );
  TEST_ERST_SRC3_BYPASS( 110, 1, erld, ersd, 0xaabbccd, tdat8, 1 );
  TEST_ERST_SRC3_BYPASS( 111, 2, erld, ersd, 0xdaabbcc, tdat9, 1 );

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

tdat:
tdat1:  .dword 0xdeadbeefdeadbeef
tdat2:  .dword 0xdeadbeefdeadbeef
tdat3:  .dword 0xdeadbeefdeadbeef
tdat4:  .dword 0xdeadbeefdeadbeef
tdat5:  .dword 0xdeadbeefdeadbeef
tdat6:  .dword 0xdeadbeefdeadbeef
tdat7:  .dword 0xdeadbeefdeadbeef
tdat8:  .dword 0xdeadbeefdeadbeef
tdat9:  .dword 0xdeadbeefdeadbeef
tdat10: .dword 0xdeadbeefdeadbeef

RVTEST_DATA_END
