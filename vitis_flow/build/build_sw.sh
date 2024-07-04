#!/bin/bash

BASE_DIR=$(pwd)/..

pushd ${BASE_DIR}/sw/applications/
make clean
make all
popd