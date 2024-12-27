open_project hls_prj
add_files ./src/top.cpp
add_files ./src/top.h
add_files -tb ./src/tb.cpp
open_solution -reset -flow_target vitis "mm2s"
set_part {xcu50-fsvh2104-2-e}
create_clock -period 3.33 -name default
set_top mm2sTop
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format xo -output mm2s.xo
exit