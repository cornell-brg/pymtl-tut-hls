############################################################

set top "IteratorTranslationUnitHLS"

open_project IteratorTranslationUnitHLS.prj

set_top $top

add_files IteratorTranslationUnitHLS.cpp

open_solution "solution1" -reset

set_part {xc7z020clg484-1}
create_clock -period 5

set_directive_interface -mode ap_ctrl_none $top

set_directive_interface -mode ap_hs $top cfgreq
set_directive_interface -mode ap_hs $top cfgresp
set_directive_interface -mode ap_hs $top xcelreq
set_directive_interface -mode ap_hs $top xcelresp
set_directive_interface -mode ap_hs $top memreq
set_directive_interface -mode ap_hs $top memresp
#set_directive_interface -mode ap_hs $top debug

set_directive_data_pack $top cfgreq
set_directive_data_pack $top cfgresp
set_directive_data_pack $top xcelreq
set_directive_data_pack $top xcelresp
set_directive_data_pack $top memreq
set_directive_data_pack $top memresp

csynth_design

exit
