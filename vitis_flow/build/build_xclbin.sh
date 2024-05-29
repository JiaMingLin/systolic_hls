#!/bin/bash
TARGET=$1
BUILD_FOLDER=${TARGET}
PLATFORM=xilinx_u50_gen3x16_xdma_5_202210_1
XO_DIR=xo

if [ -z ${BUILD_FOLDER} ];
then
    echo "Build target is empty"
    exit 1
fi

rm -rf ${BUILD_FOLDER}
mkdir ${BUILD_FOLDER}

cp xrt.ini ${BUILD_FOLDER}

if [ $TARGET == "hw_emu" ];
then
    echo "hw_emu"
    # g++ -g -std=c++17 -Wall -O0 ./host.cpp -o ./build/hw_emu/app.exe -I$XILINX_XRT/include/ -L$XILINX_XRT/lib -I$XILINX_HLS/include/ -lxrt_coreutil -pthread
    emconfigutil --platform ${PLATFORM}
    v++ -l -g -t hw_emu --platform ${PLATFORM} --profile.data all:all:all --config u50.cfg --report_level estimate ${XO_DIR}/mm2s.xo ${XO_DIR}/add_const.xo ${XO_DIR}/mul_const.xo ${XO_DIR}/s2mm.xo -o ./${TARGET}/vitis_flow.xclbin

elif [ $TARGET == "hw" ];
then
    echo "hw"
    # g++ -g -std=c++17 -Wall -O0 ./host.cpp -o ./build/hw/app.exe -I$XILINX_XRT/include/ -L$XILINX_XRT/lib -I$XILINX_HLS/include/ -lxrt_coreutil -pthread
    v++ -l -g -t hw --platform ${PLATFORM} --profile.data all:all:all --config u50.cfg  --report_level 2 --profile.trace_memory HBM[31] ${XO_DIR}/mm2s.xo ${XO_DIR}/add_const.xo ${XO_DIR}/mul_const.xo ${XO_DIR}/s2mm.xo -o ./${TARGET}/vitis_flow.xclbin
fi