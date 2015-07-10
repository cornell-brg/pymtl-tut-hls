############################################################

set top "PolyDsuDispatch"

open_project PolyDsuDispatch.prj

set_top $top

add_files PolyDsuDispatch.cpp

open_solution "solution1" -reset

set_part {xc7z020clg484-1}
create_clock -period 10

config_rtl -reset state

set_directive_interface -mode ap_ctrl_none $top

set_directive_interface -mode ap_hs $top cfgreq
set_directive_interface -mode ap_hs $top cfgresp
#set_directive_interface -mode ap_hs $top xcelreq
set_directive_interface -mode ap_hs $top polydsureq
set_directive_interface -mode ap_hs $top memreq
set_directive_interface -mode ap_hs $top memresp

set_directive_data_pack $top cfgreq
set_directive_data_pack $top cfgresp
#set_directive_data_pack $top xcelreq
set_directive_data_pack $top polydsureq
set_directive_data_pack $top memreq
set_directive_data_pack $top memresp
set_directive_data_pack $top dsTable.table

csynth_design

# concatenate all the verilog files into a single file
set fout [open ./$top.v w]
fconfigure $fout -translation binary
set vfiles [glob -nocomplain -type f "$top.prj/solution1/syn/verilog/*.v"]
foreach vfile $vfiles {
  set fin [open $vfile r]
  fconfigure $fin -translation binary
  fcopy $fin $fout
  close $fin
}

# add verilator lint off and on pragmas
seek $fout 0
puts -nonewline $fout "/* verilator lint_off WIDTH */\n\n//"
seek $fout -1 end
puts $fout "\n\n/* lint_on */"

close $fout

exit
