#*****************************************************************************
# eaddi.S
#-----------------------------------------------------------------------------
#
# Test eaddi instruction.
#

#include "riscv_test.h"
#include "test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  TEST_IMM_OP_ERS( 2,  eaddi, -32, 0, -32 );
  TEST_IMM_OP_ERS( 3,  eaddi, -48, -32, -16 );
  TEST_IMM_OP_ERS( 4,  eaddi, -56, -48, -8 );
  TEST_IMM_OP_ERS( 5,  eaddi, -56, -56, 0 );
  TEST_IMM_OP_ERS( 6,  eaddi, -48, -56, 8 );
  TEST_IMM_OP_ERS( 7,  eaddi, -32, -48, 16 );
  TEST_IMM_OP_ERS( 8,  eaddi, -0, -32, 32 );

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

RVTEST_DATA_END
