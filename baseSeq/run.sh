#!/bin/sh

echo ">Testing baseSeq"
make clean
make > compile.log
./baseSeq test/input1.fq > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./baseSeq test/input2.fq > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
./baseSeq -O test/output3.txt test/input3.fa 2>> error.log
diff test/output3.txt test/result3.txt
./baseSeq -O test/output4.txt test/input4.sam 2>> error.log
diff test/output4.txt test/result4.txt
cp baseSeq ~/bin/.
cp baseSeq ../bin/.
