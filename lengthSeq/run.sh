#!/bin/sh

echo ">Testing lengthSeq"
make clean
make > compile.log
./lengthSeq test/input1.fq > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./lengthSeq test/input2.fq > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
./lengthSeq -O test/output3.txt test/input3.fa 2>> error.log
diff test/output3.txt test/result3.txt
./lengthSeq -O test/output4.txt test/input4.sam 2>> error.log
diff test/output4.txt test/result4.txt
./lengthSeq -s test/input2.fq test/input3.fa > test/output5.txt 2>> error.log
diff test/output5.txt test/result5.txt
cp lengthSeq ~/bin/.
cp lengthSeq ../bin/.