#!/bin/sh

echo ">Testing listDirectory"
make clean
make > compile.log

./listDirectory test > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./listDirectory -O test/output2.txt test 2>> error.log
diff test/output2.txt test/result2.txt
cp listDirectory ~/bin/.
cp listDirectory ../bin/.