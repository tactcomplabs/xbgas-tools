#*****************************************************************************
# eaddix.S
#-----------------------------------------------------------------------------
#
# Test eaddix instruction.
#

#include "riscv_test.h"
#include "test_macros.h"
#include "xbgas_test_macros.h"

RVTEST_RV64U
RVTEST_CODE_BEGIN

  TEST_EADDIX_OP( 2, -32, 0, -32 );
  TEST_EADDIX_OP( 3, -48, -32, -16 );
  TEST_EADDIX_OP( 4, -56, -48, -8 );
  TEST_EADDIX_OP( 5, -56, -56, 0 );
  TEST_EADDIX_OP( 6, -48, -56, 8 );
  TEST_EADDIX_OP( 7, -32, -48, 16 );
  TEST_EADDIX_OP( 8, -0, -32, 32 );

  TEST_CASE( 9, x30, 19, \
    li x1, 19; \
    eaddie e1, x1, 0; \
    moveee e30, e1; \
    eaddi x30, e30, 0; \
  )

  TEST_PASSFAIL

RVTEST_CODE_END

  .data
RVTEST_DATA_BEGIN

  TEST_DATA

RVTEST_DATA_END
