#!/bin/sh

echo ">Testing trimBaseN"
make clean
make > compile.log
./trimBaseN test/input.fq > test/output1.fq 2> error.log
diff test/output1.fq test/result1.fq
./trimBaseN < test/input.fa > test/output2.fa 2>> error.log
diff test/output2.fa test/result2.fa
cp trimBaseN ~/bin/.
cp trimBaseN ../bin/.