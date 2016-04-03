open_project hls.prj
set_top PopCount

add_files PopCount.cpp
add_files -tb PopCount.cpp

open_solution "solution1" -reset
config_interface -expose_global

set_part {xc7z020clg484-1}
create_clock -period 5

set_directive_interface -mode ap_ctrl_none PopCount

csynth_design

exit
