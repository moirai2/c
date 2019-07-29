#!/bin/sh

echo ">Testing colorSeq"
make clean
make > compile.log
./colorSeq test/input1.fq > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./colorSeq test/input2.fq > test/output2.txt 2> error.log
diff test/output2.txt test/result2.txt
./colorSeq test/input3.fa > test/output3.txt 2> error.log
diff test/output3.txt test/result3.txt
./colorSeq test/input4.sam > test/output4.txt 2> error.log
diff test/output4.txt test/result4.txt
cp colorSeq ../bin/.
cp colorSeq ~/bin/.
