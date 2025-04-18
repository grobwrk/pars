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

cmake --preset "${preset}" -DPARS_BUILD_TESTS=ON -DPARS_BUILD_COVERAGE=ON -DPARS_BUILD_EXAMPLES=OFF
cmake --build "${out_build}"

rm -rf "$out_coverage"
mkdir -p "$out_coverage/data"

index="${out_coverage}/index"
printf "# Coverage\n\n- [tests](tests/index.html)\n" >>"${index}.md"

for test in "${out_build}"/test/test-*; do
	test=$(basename "${test}")
	if [ "${test#*.}" = "cmake" ]; then continue; fi
	profile_file="${out_coverage}/data/${test}.profraw"
	LLVM_PROFILE_FILE=${profile_file} "${out_build}/test/${test}"
	llvm-profdata merge "${out_coverage}/data/${test}.profraw" -o "${out_coverage}/data/${test}.profdata"
	mkdir "${out_coverage}/${test}"
	./opt/llvmcov2html/bin/llvmcov2html "${out_coverage}/${test}" "${out_build}/test/${test}" "${out_coverage}/data/${test}.profdata"
	printf "\n- [%s](%s/index.html)\n" "${test}" "${test}" >>"${index}.md"
done

mkdir "${out_coverage}/tests"
llvm-profdata merge "${out_coverage}"/data/*.profraw -o "${out_coverage}/data/tests.profdata"
./opt/llvmcov2html/bin/llvmcov2html "${out_coverage}/tests" "${out_build}/test/tests" "${out_coverage}/data/tests.profdata"

pandoc "${index}.md" -o "${index}.html"

ls -laR "${out_coverage}"
