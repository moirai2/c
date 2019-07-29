#!/bin/sh

echo ">Testing checkFastqFormat"
make clean
make > compile.log
./checkFastqFormat test/input1.fq > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./checkFastqFormat < test/input2.fq > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
cp checkFastqFormat ~/bin/.
cp checkFastqFormat ../bin/.
