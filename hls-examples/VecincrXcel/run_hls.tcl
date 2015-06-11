############################################################

set top "VecincrXcelHLS"

open_project VecincrXcelHLS.prj

set_top $top

add_files VecincrXcelHLS.cpp

open_solution "solution1" -reset

set_part {xc7z020clg484-1}
create_clock -period 5

set_directive_interface -mode ap_ctrl_none $top

set_directive_interface -mode ap_hs $top xcelreq
set_directive_interface -mode ap_hs $top xcelresp
set_directive_interface -mode ap_hs $top memreq
set_directive_interface -mode ap_hs $top memresp

set_directive_data_pack $top xcelreq
set_directive_data_pack $top xcelresp
set_directive_data_pack $top memreq
set_directive_data_pack $top memresp

csynth_design

exit
