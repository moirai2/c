#!/bin/sh

echo ">Testing dotMatrix"
make clean
make > compile.log
./dotMatrix -e 1 test/reference1.fa test/input1.fa > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp dotMatrix ~/bin/.
cp dotMatrix ../bin/.
