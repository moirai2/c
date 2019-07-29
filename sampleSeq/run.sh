#!/bin/sh

echo ">Testing sampleSeq"
make clean
make > compile.log
./sampleSeq -s 2 test/input.fq.gz > test/output1.fq 2> error.log
diff test/output1.fq test/result1.fq
./sampleSeq -s 2 test/input.fq > test/output2.fq 2>> error.log
diff test/output2.fq test/result2.fq
./sampleSeq -s 0 test/input.fq > test/output3.fq 2>> error.log
diff test/output3.fq test/result3.fq
./sampleSeq -s 3 test/input.fq test/input.fq.gz > test/output4.fq 2>> error.log
diff test/output4.fq test/result4.fq
cp sampleSeq ~/bin/.
cp sampleSeq ../bin/.