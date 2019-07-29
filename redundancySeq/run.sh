#!/bin/sh

echo ">Testing redundancySeq"
make clean
make > compile.log
./redundancySeq test/input1.fq test/input2.fq test/input3.fa test/input4.sam > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./redundancySeq -r test/input1.fq test/input2.fq test/input3.fa test/input4.sam > test/output2.txt 2> error.log
diff test/output2.txt test/result2.txt
cp redundancySeq ~/bin/.
cp redundancySeq ../bin/.