#include "test_macros.h"

#define SET_REMOTE_ADDR( ereg, addr ) \
    li x1, addr; \
    eaddie ereg, x1, 0; \

#define PRESET_MEM(inst, val, base, size, iter) \
    SET_REMOTE_ADDR( e1, 1 ) \
    la x1, base; \
    addi x6, x1, 0; \
    li x2, MASK_XLEN(val); \
    li x3, 0; \
    eaddie e6, x3, 0; \
    li x4, iter; \
    li x5, size; \
1:  inst x2, 0(x1); \
    inst x2, 0(x6); \
    addi x3, x3, 1; \
    add x1, x1, x5; \
    bne x3, x4, 1b;

#define TEST_EADDI_OP( testnum, result, val, imm )\
  TEST_CASE( testnum, x30, result, \
      li x1, MASK_XLEN(val); \
      eaddie e0, x1, 0; \
      eaddi x30, e0, SEXT_IMM(imm); \
    )

#define TEST_EADDIE_OP( testnum, result, val, imm )\
  TEST_CASE( testnum, x30, result, \
      li  x1, MASK_XLEN(val); \
      eaddie e0, x1, SEXT_IMM(imm); \
      eaddi x30, e0, 0; \
    )

#define TEST_EADDIX_OP( testnum, result, val, imm )\
  TEST_CASE( testnum, x30, result, \
      li  x1, MASK_XLEN(val); \
      eaddie e0, x1, 0; \
      eaddix e30, e0, SEXT_IMM(imm); \
      eaddi x30, e30, 0; \
    )

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
    
#define TEST_ERLD_OP( testnum, inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
    TEST_CASE( testnum, x30, result, \
      la  x1, base; \
      inst x30, x1, e2; \
    )

#define TEST_ERLD_DEST_BYPASS( testnum, nop_cycles, inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    inst x30, x1, e2; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    addi  x6, x30, 0; \
    li  x29, result; \
    bne x6, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b; \

#define TEST_ERLD_SRC1_BYPASS( testnum, nop_cycles, inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst x30, x1, e2; \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_ERLD_SRC2_BYPASS( testnum, nop_cycles, inst, result, base, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    eaddie e1, x4, remote; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst x30, x1, e1; \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b; \

#define TEST_ERLDE_OP( testnum, inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
    TEST_CASE( testnum, x30, result, \
      la  x1, base; \
      inst e30, x1, e2; \
      eaddi x30, e30, 0; \
    )

#define TEST_ERLDE_DEST_BYPASS( testnum, nop_cycles, inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    inst e30, x1, e2; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    eaddi  x6, e30, 0; \
    li  x29, result; \
    bne x6, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b; \

#define TEST_ERLDE_SRC1_BYPASS( testnum, nop_cycles, inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst e30, x1, e2; \
    li  x29, result; \
    eaddi x30, e30, 0; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_ERLDE_SRC2_BYPASS( testnum, nop_cycles, inst, result, base, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    eaddie e1, x4, remote; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst e30, x1, e1; \
    li  x29, result; \
    eaddi x30, e30, 0; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b; \

#define TEST_ERST_OP( testnum, load_inst, store_inst, result, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_CASE( testnum, x30, result, \
      la  x1, base; \
      li  x2, result; \
      store_inst x2, x1, e1; \
      load_inst x30, x1, e1; \
    )

#define TEST_ERST_SRC1_BYPASS( testnum, src1_nops, load_inst, store_inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
    la  x2, base; \
1:  li  x1, result; \
    TEST_INSERT_NOPS_ ## src1_nops \
    store_inst x1, x2, e2; \
    load_inst x30, x2, e2; \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_ERST_SRC2_BYPASS( testnum, src2_nops, load_inst, store_inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
    li  x1, result; \
1:  la  x2, base; \
    TEST_INSERT_NOPS_ ## src2_nops \
    store_inst x1, x2, e2; \
    load_inst x30, x2, e2; \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_ERST_SRC3_BYPASS( testnum, src3_nops, load_inst, store_inst, result, base, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
    la  x2, base; \
    li x1, result; \
1:  eaddie e2, x4, remote; \
    TEST_INSERT_NOPS_ ## src3_nops \
    store_inst x1, x2, e2; \
    load_inst x30, x2, e2; \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b;

#define TEST_ERSTE_OP( testnum, load_inst, store_inst, result, base, remote ) \
    SET_REMOTE_ADDR( e1, remote ) \
    TEST_CASE( testnum, x30, result, \
      la  x1, base; \
      li  x2, result; \
      eaddie e2, x2, 0; \
      store_inst e2, x1, e1; \
      load_inst x30, x1, e1; \
    )

#define TEST_ERSTE_SRC1_BYPASS( testnum, src1_nops, load_inst, store_inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
    la  x2, base; \
    li  x1, result; \
1:  eaddie e1, x1, 0; \
    TEST_INSERT_NOPS_ ## src1_nops \
    store_inst e1, x2, e2; \
    load_inst x30, x2, e2; \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_ERSTE_SRC2_BYPASS( testnum, src2_nops, load_inst, store_inst, result, base, remote ) \
    SET_REMOTE_ADDR( e2, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
    li  x1, result; \
    eaddie e1, x1, 0; \
1:  la  x2, base; \
    TEST_INSERT_NOPS_ ## src2_nops \
    store_inst e1, x2, e2; \
    load_inst x30, x2, e2; \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_ERSTE_SRC3_BYPASS( testnum, src3_nops, load_inst, store_inst, result, base, remote ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
    la  x2, base; \
    li x1, result; \
    eaddie e1, x1, 0; \
1:  eaddie e2, x4, remote; \
    TEST_INSERT_NOPS_ ## src3_nops \
    store_inst e1, x2, e2; \
    load_inst x30, x2, e2; \
    li  x29, result; \
    bne x30, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b;