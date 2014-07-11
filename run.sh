while read tcase; do
echo run "case#" $tcase
cp ../testcase/$tcase/iimage.bin iimage.bin
cp ../testcase/$tcase/dimage.bin dimage.bin
make clean > null
make > null
./single_cycle > null
echo .
echo Compare
diff ../testcase/$tcase/snapshot.rpt snapshot.rpt
diff ../testcase/$tcase/error_dump.rpt error_dump.rpt
echo End
done < ../testcase_list
