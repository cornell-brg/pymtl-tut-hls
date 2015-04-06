name=FindIfUnitVRTL
#cat top.v | sed s/ap_clk/clock/ | sed s/ap_rst/reset/ | sed s/module top/module $name/ > $name.v
cat top.v | sed s/ap_clk/clk/g > temp.v
cat temp.v | sed s/ap_rst/reset/g > temp2.v && mv temp2.v temp.v
cat temp.v | sed "s/module top/module $name/g" > temp2.v && mv temp2.v temp.v
cat temp.v | sed "/^module /i \ \/\* verilator lint_off WIDTH \*\/" > temp2.v && mv temp2.v temp.v
echo "/* lint_on */" >> temp.v
mv temp.v $name.v
