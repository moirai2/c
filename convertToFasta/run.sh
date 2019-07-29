#!/bin/sh

echo ">Testing convertToFasta"
make clean
make > compile.log
./convertToFasta test/input.sam > test/output1.fa 2> error.log
diff test/output1.fa test/result1.fa
cp convertToFasta ~/bin/.
cp convertToFasta ../bin/.
