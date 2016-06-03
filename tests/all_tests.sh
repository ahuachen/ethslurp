clear
rm -fR ~/.ethslurp.test # clean up prior tests
rm -fR ./files/files_*/*

echo "test1: default behaviour should show help"                              >proposed/test1.txt
cat api_key | ../ethslurp -t                                                 >>proposed/test1.txt
find ~/.ethslurp.test                                                        >>proposed/test1.txt 2>/dev/null # should be empty
./copyTestFolder 1

echo
echo "Running ethslurp test suite..."
echo

echo "test2: slurp on known dead account"                                | tee proposed/test2.txt
../ethslurp -t 0x713B73c3994442b533e6A083eC968e40606810Ec                    >>proposed/test2.txt
cat ~/.ethslurp.test/config.dat | grep -v api_key                            >>proposed/test2.txt
./copyTestFolder 2

echo "test3: rerun flag"                                                 | tee proposed/test3.txt
../ethslurp -t -r                                                            >>proposed/test3.txt
./copyTestFolder 3

echo "test4: test prettyPrint flag"                                      | tee proposed/test4.txt
../ethslurp -t -r -f                                                         >>proposed/test4.txt ; ./strip_confirms 4;
./copyTestFolder 4
sleep 1s

echo "test5: test tab delimited flag"                                    | tee proposed/test5.txt
../ethslurp -t -v -r -f:txt                                                  >>proposed/test5.txt
./copyTestFolder 5

echo "test6: test comma seperated value flag"                            | tee proposed/test6.csv
../ethslurp -t -v -r -f:csv                                                  >>proposed/test6.csv
./copyTestFolder 6

echo "test7: test html flag"                                             | tee proposed/test7.html
../ethslurp -t -v -r -f:html                                                 >>proposed/test7.html
./copyTestFolder 7
sleep 1s

echo "test8: extract income records only"                                | tee proposed/test8.txt
../ethslurp -t -v -i -f 0x682e426ea761db77bacd5acdce33ca122175daea           >>proposed/test8.txt
./copyTestFolder 8

echo "test9: extract expense records only"                               | tee proposed/test9.txt
../ethslurp -t -v -e -f 0x682e426ea761db77bacd5acdce33ca122175daea           >>proposed/test9.txt
./copyTestFolder 9
sleep 1s

cat ~/.ethslurp.test/config.dat | grep -v fmt_fieldList >file
mv -f file ~/.ethslurp.test/config.dat
echo "fmt_fieldList=timeStamp|date|from|nonce|value|ether|input|inputLen|gasUsed|gas|hitLimit|gasPrice|blockNumber|transactionIndex|cumulativeGasUsed|blockHash|hash" >>~/.ethslurp.test/config.dat
echo "test10: test date option fail"                                     | tee proposed/test10_fail.txt
../ethslurp -t -d -r -f:txt 0x713B73c3994442b533e6A083eC968e40606810Ec       >>proposed/test10_fail.txt
echo "test10: test date option less than"                                | tee proposed/test10_lessthan.txt
../ethslurp -t -d::20160519 -r -f:txt                                        >>proposed/test10_lessthan.txt
echo "test10: test date option greater than"                             | tee proposed/test10_greaterthan.txt
../ethslurp -t -d:20160510123300 -r -f:txt                                   >>proposed/test10_greaterthan.txt
echo "test10: test date option range"                                    | tee proposed/test10_range.txt
cat ~/.ethslurp.test/config.dat | grep -v fmt_fieldList >file
mv -f file ~/.ethslurp.test/config.dat
echo "[DISPLAY_STR]"                               >~/.ethslurp.test/0x682e426ea761db77bacd5acdce33ca122175daea.dat
echo "fmt_fieldList=from|value|ether|blockNumber" >>~/.ethslurp.test/0x682e426ea761db77bacd5acdce33ca122175daea.dat
cat ~/.ethslurp.test/*.dat | grep -v api_key                                 >>proposed/test2.txt
../ethslurp -t -d:20160510:20160519151605 -r -f:txt                          >>proposed/test10_range.txt
./copyTestFolder 10
rm ~/.ethslurp.test/0x682e426ea761db77bacd5acdce33ca122175daea.dat

echo "test11: test open config file"                                     | tee proposed/test11.txt
../ethslurp -t -o -r -f                                                      >>proposed/test11.txt
./copyTestFolder 11

cat ~/.ethslurp.test/config.dat | grep -v fmt_fieldList >file
mv -f file ~/.ethslurp.test/config.dat
echo "test12: test block number option fail"                             | tee proposed/test12_fail.txt
../ethslurp -t -b -r -f                                                      >>proposed/test12_fail.txt
echo "test12: test block number option less than"                        | tee proposed/test12_lessthan.txt
../ethslurp -t -b::1491788 -r -f                                             >>proposed/test12_lessthan.txt
echo "test12: test block number option greater than"                     | tee proposed/test12_greaterthan.txt
../ethslurp -t -b:1491788 -r -f                                              >>proposed/test12_greaterthan.txt
echo "test12: test block number option range"                            | tee proposed/test12_range.txt
../ethslurp -t -b:1491752:1546617 -r -f                                      >>proposed/test12_range.txt
./copyTestFolder 12
sleep 1s

echo "fmt_custom_file=file:custom_format_file.html" >>~/.ethslurp.test/config.dat
echo "fmt_custom_record=fmt_html_record" >>~/.ethslurp.test/config.dat
echo "fmt_custom_field=fmt_html_field" >>~/.ethslurp.test/config.dat

echo "test14: test custom display strings, file: format and fmt_ format" | tee proposed/test14.txt
echo "[FIELD_STR]"    >>~/.ethslurp.test/config.dat
echo "timeStamp=date" >>~/.ethslurp.test/config.dat
cat ~/.ethslurp.test/config.dat | grep -v api_key                            >>proposed/test14.txt
../ethslurp -t -r -f:custom                                                  >>proposed/test14.txt
./copyTestFolder 14

echo "test17: test chunked commands (test,fmt,slurp,re-run)"             | tee proposed/test17.txt
../ethslurp -t -fsr                                                          >>proposed/test17.txt

echo "test15: test list and cache clear options"                         | tee proposed/test15.txt
ls ~/.ethslurp.test/slurps                                                   >>proposed/test15.txt
../ethslurp -t -l                                                            >>proposed/test15.txt
../ethslurp -t -c                                                            >>proposed/test15.txt
find ~/.ethslurp.test                                                        >>proposed/test15.txt

echo "Bring down EEAR contract just to keep track of it"                      | tee proposed/eear.txt
cat api_key |../ethslurp -t -f:txt 0xbdace480953fc8260184bd625389ea20bbd0da9f     >>proposed/eear.txt

if [ $# -ne 0 ]
  then
#
# With a cleared cache we first copy an old bin file from May 2016 and then 
# read and possibly update from the blockchain (set api_key first but ignore it)
cat api_key | ../ethslurp -t                                                            >proposed/test16.txt
echo "test16: test updating from cache"                                            | tee proposed/test16.txt
cp -p cache/0x41f274c0023f83391de4e0733c609df5a124c3d4.bin ~/.ethslurp.test/slurps     >>proposed/test16.txt
../ethslurp -t -f:csv 0x41f274c0023f83391de4e0733c609df5a124c3d4                       >>proposed/test16.txt
    # ./perf_tests
fi

# now diff to see if anything changed
echo "----------------------------"
echo " Doing a 'diff.' If anything shows below, you "
echo " either broke something or you have to update gold."
echo "----------------------------"
diff -r proposed gold
echo
