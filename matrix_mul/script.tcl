open_project hls_prj
add_files top.cpp
add_files top.h
add_files -tb tb.cpp
open_solution "solution1" -flow_target vivado
set_part {xcu50-fsvh2104-2-e}
create_clock -period 10 -name default
#source "./mm_i_j/solution1/directives.tcl"
#csim_design
#csynth_design
#cosim_design
export_design -format ip_catalog
