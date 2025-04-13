#!/bin/bash

preset=$1
target=$2
envdir=$(mktemp -d)
python3 -m venv $envdir
source $envdir/bin/activate
[ "${target}" = "docs" ] && pip install -r docs/requirements.txt
cmake --preset ${preset} -DPARS_BUILD_DOCUMENTATION=ON -DPARS_BUILD_TESTS=ON
cmake --build out/build/${preset} -t ${target}
cmake --install out/build/${preset}
