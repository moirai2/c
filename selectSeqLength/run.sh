#!/bin/sh

echo ">Testing selectSeqLength"
make clean
make > compile.log
./selectSeqLength -n 4 test/input.fq > test/output1.fq 2> error.log
diff test/output1.fq test/result1.fq
./selectSeqLength -n 4-6 test/input.fa > test/output2.fa 2> error.log
diff test/output2.fa test/result2.fa
cp selectSeqLength ~/bin/.
cp selectSeqLength ../bin/.