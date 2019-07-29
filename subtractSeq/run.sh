#!/bin/sh

echo ">Testing subtractSeq"
make clean
make > compile.log
./subtractSeq test/input1.fa test/diff1.fa > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./subtractSeq test/input2.fq test/diff2.fq > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
./subtractSeq -O test/output3-1.txt -M test/output3-2.txt test/input3.sam test/diff3.sam 2>> error.log
diff test/output3-1.txt test/result3-1.txt
diff test/output3-2.txt test/result3-2.txt
cp subtractSeq ~/bin/.
cp subtractSeq ../bin/.