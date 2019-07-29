#!/bin/sh

echo ">Testing convertSeq"
make clean
make > compile.log
./convertSeq U T < test/input.fa > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./convertSeq -I test/input.fa -O test/output2.txt U T 2> error.log
diff test/output2.txt test/result2.txt
cp convertSeq ~/bin/.
cp convertSeq ../bin/.
