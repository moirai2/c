#!/bin/sh

echo ">Testing fillSeq"
make clean
make > compile.log
./fillSeq -n 20 test/input.fq > test/output1.fq 2> error.log
diff test/output1.fq test/result1.fq
./fillSeq -e < test/input.fa > test/output2.fa 2> error.log
diff test/output2.fa test/result2.fa
cp fillSeq ~/bin/.
cp fillSeq ../bin/.
