#include "test_macros.h"

#define SET_REMOTE_ADDR( ereg, addr ) \
    li x1, addr; \
    eaddie ereg, x1, 0;

#define TEST_ELD_OP( testnum, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_LD_OP( testnum, inst, result, offset, base )

#define TEST_ELD_DEST_BYPASS( testnum, nop_cycles, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_LD_DEST_BYPASS( testnum, nop_cycles, inst, result, offset, base ) 

#define TEST_ELD_SRC1_BYPASS( testnum, nop_cycles, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_LD_SRC1_BYPASS( testnum, nop_cycles, inst, result, offset, base ) 
