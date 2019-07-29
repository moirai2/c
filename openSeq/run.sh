#!/bin/sh

echo ">Testing openSeq"
make clean
make > compile.log
./openSeq test/input.fq.gz > test/output1.fq 2> error.log
diff test/output1.fq test/result1.fq
./openSeq < test/input.fq > test/output2.fq 2>> error.log
diff test/output2.fq test/result2.fq
./openSeq -O test/output3.fq test/input.fq 2>> error.log
diff test/output3.fq test/result3.fq
./openSeq -O test/output4.fq < test/input.fq 2>> error.log
diff test/output4.fq test/result4.fq
cp openSeq ~/bin/.
cp openSeq ../bin/.