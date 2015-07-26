############################################################
set top "ListInsertHLS"
set prj "list_insert.prj"
open_project $prj
set_top $top

add_files $top\.cpp
add_files -tb $top\.cpp

open_solution "solution1" -reset
config_interface -expose_global

set_part {xc7z020clg484-1}
create_clock -period 5

set_directive_interface -mode ap_ctrl_none $top
set_directive_interface -mode ap_hs $top memreq
set_directive_interface -mode ap_hs $top memresp

set_directive_data_pack $top memreq
set_directive_data_pack $top memresp

csynth_design

#cosim_design -rtl systemc

#export_design -evaluate verilog

#file copy -force $prj\/solution1/.autopilot/db/a.o.2.ll a.o.2.orig.ll

set fout [open ./$top\.v w]
fconfigure $fout -translation binary
set vfiles [glob -nocomplain -type f $prj/solution1/syn/verilog/*.v]
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
