#!/bin/sh

echo ">Testing uniqSeq"
make clean
make > compile.log
./uniqSeq -s test/input1.fq > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./uniqSeq -s test/input2.fq > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
./uniqSeq -s test/input3.fa > test/output3.txt 2>> error.log
diff test/output3.txt test/result3.txt
./uniqSeq -s test/input4.sam > test/output4.txt 2>> error.log
diff test/output4.txt test/result4.txt
cp uniqSeq ~/bin/.
cp uniqSeq ../bin/.