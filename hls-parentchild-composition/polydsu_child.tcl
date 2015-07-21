############################################################

set top "Child"

open_project Child.prj

set_top $top

add_files Child.cpp

open_solution "solution1" -reset

set_part {xc7z020clg484-1}
create_clock -period 10

config_rtl -reset state

set_directive_interface -mode ap_ctrl_none $top

set_directive_interface -mode ap_hs $top req
set_directive_interface -mode ap_hs $top resp

set_directive_data_pack $top req
set_directive_data_pack $top resp

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
