#! /bin/bash
#
# Script to build RISC-V ISA simulator, proxy kernel, and GNU toolchain.
# Tools will be installed to $RISCV.

. build.common

echo "Starting RISC-V Toolchain build process"

# Build toolchain with updated xbgas gnu compiler
build_project riscv-openocd --prefix=$RISCV --enable-remote-bitbang --enable-jtag_vpi --disable-werror
build_project riscv-fesvr --prefix=$RISCV
build_project riscv-isa-sim --prefix=$RISCV --with-fesvr=$RISCV CXX=mpicxx CC=mpicc
build_project xbgas-gnu-toolchain --prefix=$RISCV --enable-multilib
CC= CXX= build_project riscv-pk --prefix=$RISCV --host=riscv64-unknown-elf
build_project riscv-tests --prefix=$RISCV/riscv64-unknown-elf

# Build LLVM
cd xbgas-llvm
cmake -S llvm -B build -G Ninja \
  -DCMAKE_INSTALL_PREFIX=$RISCV \
  -DCMAKE_BUILD_TYPE=Release \
	-DBUILD_SHARED_LIBS=True \
	-DLLVM_ENABLE_PROJECTS="clang;lld"  \
	-DLLVM_USE_SPLIT_DWARF=True \
	-DLLVM_OPTIMIZED_TABLEGEN=True \
	-DDEFAULT_SYSROOT=$RISCV/riscv64-unknown-elf \
	-DGCC_INSTALL_PREFIX=$RISCV \
	-DLLVM_TARGETS_TO_BUILD=RISCV \
	-DLLVM_PARALLEL_LINK_JOBS=1 \
	-DLLVM_BUILD_TESTS=True \
	-DLLVM_DEFAULT_TARGET_TRIPLE="riscv64-unknown-elf"
cmake --build build -- install
cd ..

echo -e "\\nRISC-V Toolchain installation completed!"
