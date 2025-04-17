#!/bin/sh

set -e

preset=$1

[ ! -d opt/llvmcov2html ] && mkdir -p opt/llvmcov2html

if [ ! -f opt/llvmcov2html/bin/llvmcov2html ]; then
	cd opt/llvmcov2html
	git clone https://github.com/neumannt/llvmcov2html.git .
	make
	cd ../..
fi

out_build="out/build/${preset}"
out_coverage="out/coverage/${preset}"

mkdir -p "$out_coverage/data"

cmake --preset "${preset}" -DPARS_BUILD_TESTS=ON -DPARS_BUILD_COVERAGE=ON
cmake --build "${out_build}"
LLVM_PROFILE_FILE="${out_coverage}/data/tests.profraw" "${out_build}/test/tests"
llvm-profdata merge -sparse "${out_coverage}/data/tests.profraw" -o "${out_coverage}/data/tests.profdata"
opt/llvmcov2html/bin/llvmcov2html "${out_coverage}" "${out_build}/test/tests" "${out_coverage}/data/tests.profdata"

ls -laR "${out_coverage}"
