# rename.sh
# This script is used to make changes to a file
# so that it follows PyMTL and Verilator conventions
# Input:  top.v
# Output: $name.v

name=FunctorUnitVRTL

# rename clk and rst
cat top.v | sed s/ap_clk/clk/g > temp.v
cat temp.v | sed s/ap_rst/reset/g > temp2.v && mv temp2.v temp.v

# add verilator pragmas to ignore bitwdith differences
echo "/* verilator lint_off WIDTH */" > temp2.v
cat temp.v >> temp2.v && mv temp2.v temp.v
#cat temp.v | sed "/^module top (/i \ \/\* verilator lint_off WIDTH \*\/" > temp2.v && mv temp2.v temp.v
echo "/* lint_on */" >> temp.v

# rename module name
cat temp.v | sed "s/^module top (/module $name (/g" > temp2.v && mv temp2.v temp.v

mv temp.v $name.v
