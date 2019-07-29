#!/bin/sh

echo ">Testing qvSeq"
make clean
make > compile.log
./qvSeq test/input1.fq > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./qvSeq test/input2.fq > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
./qvSeq -O test/output3.txt test/input3.sam 2>> error.log
diff test/output3.txt test/result3.txt
cp qvSeq ~/bin/.
cp qvSeq ../bin/.