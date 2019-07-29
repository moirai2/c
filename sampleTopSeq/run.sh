#!/bin/sh

echo ">Testing sampleTopSeq"
make clean
make > compile.log
./sampleTopSeq test/input1.fq > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./sampleTopSeq test/input2.fq > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
./sampleTopSeq test/input3.fa > test/output3.txt 2>> error.log
diff test/output3.txt test/result3.txt
./sampleTopSeq test/input4.sam > test/output4.txt 2>> error.log
diff test/output4.txt test/result4.txt
./sampleTopSeq -r test/input1.fq > test/output5.txt 2>> error.log
diff test/output5.txt test/result5.txt
./sampleTopSeq -R "redundancy	%.2f" test/input1.fq > test/output6.txt 2>> error.log
diff test/output6.txt test/result6.txt
cp sampleTopSeq ~/bin/.
cp sampleTopSeq ../bin/.
