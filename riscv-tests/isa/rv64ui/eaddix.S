#*****************************************************************************
# eaddix.S
#-----------------------------------------------------------------------------
#
# Test eaddix instruction.
#

#include "riscv_test.h"
#include "test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  TEST_IMM_OP_ERDS( 2,  eaddix, -32, 0, -32 );
  TEST_IMM_OP_ERDS( 3,  eaddix, -48, -32, -16 );
  TEST_IMM_OP_ERDS( 4,  eaddix, -56, -48, -8 );
  TEST_IMM_OP_ERDS( 5,  eaddix, -56, -56, 0 );
  TEST_IMM_OP_ERDS( 6,  eaddix, -48, -56, 8 );
  TEST_IMM_OP_ERDS( 7,  eaddix, -32, -48, 16 );
  TEST_IMM_OP_ERDS( 8,  eaddix, -0, -32, 32 );

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

RVTEST_DATA_END
