#!/bin/sh

echo ">Testing htmlSeq"
make clean
make > compile.log
./htmlSeq test/style.txt test/input.fa test/marker.fa > test/output1.html 2> error.log
diff test/output1.html test/result1.html
cp htmlSeq ~/bin/.
cp htmlSeq ../bin/.
