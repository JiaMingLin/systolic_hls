#!/bin/bash

BASE_DIR=$(pwd)/..

pushd ${BASE_DIR}/sw/
make clean
make all
cp tb_input_features.dat ${BASE_DIR}/build/build_hw/
cp tb_output_predictions.dat ${BASE_DIR}/build/build_hw/
popd