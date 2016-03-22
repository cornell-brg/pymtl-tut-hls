open_project hls.prj
set_top pc

add_files pc.cpp
add_files -tb pc.cpp

open_solution "solution1" -reset
config_interface -expose_global

set_part {xc7z020clg484-1}
create_clock -period 5

set_directive_interface -mode ap_ctrl_none pc

set_directive_interface -mode ap_hs pc x
set_directive_interface -mode ap_hs pc num

csynth_design

exit
