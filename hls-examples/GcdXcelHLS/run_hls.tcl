############################################################

set top "GcdXcelHLS"

open_project GcdXcelHLS.prj

set_top $top

add_files GcdXcelHLS.cpp

open_solution "solution1" -reset


set_part {xc7z020clg484-1}
create_clock -period 5

set_directive_interface -mode ap_ctrl_none $top
set_directive_interface -mode ap_hs $top xcelIn
set_directive_interface -mode ap_hs $top xcelOut

set_directive_data_pack $top xcelIn
set_directive_data_pack $top xcelOut

csynth_design

exit
