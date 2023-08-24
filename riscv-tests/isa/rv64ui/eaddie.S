#*****************************************************************************
# eaddie.S
#-----------------------------------------------------------------------------
#
# Test eaddie instruction.
#

#include "riscv_test.h"
#include "test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  TEST_IMM_OP_ERD( 2,  eaddie, -32, 0, -32 );
  TEST_IMM_OP_ERD( 3,  eaddie, -48, -32, -16 );
  TEST_IMM_OP_ERD( 4,  eaddie, -56, -48, -8 );
  TEST_IMM_OP_ERD( 5,  eaddie, -56, -56, 0 );
  TEST_IMM_OP_ERD( 6,  eaddie, -48, -56, 8 );
  TEST_IMM_OP_ERD( 7,  eaddie, -32, -48, 16 );
  TEST_IMM_OP_ERD( 8,  eaddie, -0, -32, 32 );

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

RVTEST_DATA_END
