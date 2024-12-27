#!/bin/bash

CURR_DIR=$(pwd)

rm -rf ${CURR_DIR}/xo
mkdir ${CURR_DIR}/xo

pushd ../hw/mm2s/
rm -rf hls_proj *.xo *.log
vitis_hls -f script.tcl 
cp *.xo ${CURR_DIR}/xo
popd
pushd ../hw/s2mm/
rm -rf hls_proj *.xo *.log
vitis_hls -f script.tcl
cp *.xo ${CURR_DIR}/xo
popd
pushd ../hw/axi_stream/
rm -rf hls_proj *.xo *.log
vitis_hls -f script.tcl
cp *.xo ${CURR_DIR}/xo
popd