#include "test_macros.h"

#define SET_REMOTE_ADDR( ereg, addr ) \
    li x1, addr; \
    eaddie ereg, x1, 0;

#define PRESET_MEM(inst, val, base, size, iter) \
    SET_REMOTE_ADDR(e1, 1); \
    la x1, base; \
    li x2, val; \
    li x3, 0; \
    li x4, iter; \
    li x5, size; \
1:  inst x2, 0(x1); \
    addi x3, x3, 1; \
    add x1, x1, x5; \
    bne x3, x4, 1b; \


#define TEST_ELD_OP( testnum, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_LD_OP( testnum, inst, result, offset, base )

#define TEST_ELD_DEST_BYPASS( testnum, nop_cycles, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_LD_DEST_BYPASS( testnum, nop_cycles, inst, result, offset, base ) 

#define TEST_ELD_SRC1_BYPASS( testnum, nop_cycles, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_LD_SRC1_BYPASS( testnum, nop_cycles, inst, result, offset, base )

#define TEST_ELE_OP( testnum, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_CASE( testnum, x30, result, \
      la  x1, base; \
      inst e30, offset(x1); \
      eaddi x30, e30, 0; \
    )

#define TEST_ELE_DEST_BYPASS( testnum, nop_cycles, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    inst e30, offset(x1); \
    TEST_INSERT_NOPS_ ## nop_cycles \
    eaddi  x6, e30, 0; \
    li  x29, result; \
    bne x6, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b; \

#define TEST_ELE_SRC1_BYPASS( testnum, nop_cycles, inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst e30, offset(x1); \
    li  x29, result; \
    eaddi x30, e30, 0; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_EST_OP( testnum, load_inst, store_inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_ST_OP( testnum, load_inst, store_inst, result, offset, base )

#define TEST_EST_SRC12_BYPASS( testnum, src1_nops, src2_nops, load_inst, store_inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
    TEST_ST_SRC12_BYPASS( testnum, src1_nops, src2_nops, load_inst, store_inst, result, offset, base )

#define TEST_EST_SRC21_BYPASS( testnum, src1_nops, src2_nops, load_inst, store_inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
    TEST_ST_SRC21_BYPASS( testnum, src1_nops, src2_nops, load_inst, store_inst, result, offset, base )

#define TEST_ESE_OP( testnum, load_inst, store_inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_CASE( testnum, x30, result, \
      la  x1, base; \
      li  x2, result; \
      eaddie e2, x2, 0; \
      store_inst e2, offset(x1); \
      load_inst x30, offset(x1); \
    )

#define TEST_ESE_SRC12_BYPASS( testnum, src1_nops, src2_nops, load_inst, store_inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  li  x1, result; \
    eaddie e1, x1, 0; \
    TEST_INSERT_NOPS_ ## src1_nops \
    la  x2, base; \
    TEST_INSERT_NOPS_ ## src2_nops \
    store_inst e1, offset(x2); \
    load_inst x30, offset(x2); \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b; \

#define TEST_ESE_SRC21_BYPASS( testnum, src1_nops, src2_nops, load_inst, store_inst, result, offset, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x2, base; \
    TEST_INSERT_NOPS_ ## src1_nops \
    li  x1, result; \
    eaddie e1, x1, 0; \
    TEST_INSERT_NOPS_ ## src2_nops \
    store_inst e1, offset(x2); \
    load_inst x30, offset(x2); \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b;
    