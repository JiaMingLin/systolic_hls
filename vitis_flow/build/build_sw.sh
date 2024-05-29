# if [ $TARGET == "sw_emu" ];
# then
#     echo "sw"
#     g++ -g -std=c++17 -Wall -O0 ./host.cpp -o ./build/hw_emu/app.exe -I$XILINX_XRT/include/ -L$XILINX_XRT/lib -I$XILINX_HLS/include/ -lxrt_coreutil -pthread
#     emconfigutil --platform ${PLATFORM}
#     v++ -t sw_emu -c -k vadd --platform ${PLATFORM} --config u50.cfg -I./src ./src/vadd.cpp -o sw_emu/vadd.xo 
#     v++ -l -t sw_emu --platform ${PLATFORM} --config u50.cfg ./sw_emu/vadd.xo -o ./sw_emu/vadd.xclbin


if [ $TARGET == "hw_emu" ];
then
    echo "hw_emu"
    # g++ -g -std=c++17 -Wall -O0 ./host.cpp -o ./build/hw_emu/app.exe -I$XILINX_XRT/include/ -L$XILINX_XRT/lib -I$XILINX_HLS/include/ -lxrt_coreutil -pthread
    emconfigutil --platform ${PLATFORM}
    v++ -l -g -t hw_emu --platform ${PLATFORM} --profile.data all:all:all --config u50.cfg --report_level estimate ./hw/mm2s/mm2s.xo ./hw/module_add_const/add_const.xo ./hw/module_mul_const/mul_const.xo ./hw/s2mm/s2mm.xo -o ./build/hw_emu/vitis_flow.xclbin

elif [ $TARGET == "hw" ];
then
    echo "hw"
    # g++ -g -std=c++17 -Wall -O0 ./host.cpp -o ./build/hw/app.exe -I$XILINX_XRT/include/ -L$XILINX_XRT/lib -I$XILINX_HLS/include/ -lxrt_coreutil -pthread
    v++ -l -g -t hw --platform ${PLATFORM} --profile.data all:all:all --config u50.cfg  --report_level 2 --profile.trace_memory HBM[31] ./hw/mm2s/mm2s.xo ./hw/module_add_const/add_const.xo ./hw/module_mul_const/mul_const.xo ./hw/s2mm/s2mm.xo -o ./build/hw/vitis_flow.xclbin
fi