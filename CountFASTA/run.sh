#!/bin/sh

echo ">Testing CountFASTA"
make clean
make > compile.log
./CountFASTA test/input.fa > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp CountFASTA ~/bin/.
cp CountFASTA ../bin/.
