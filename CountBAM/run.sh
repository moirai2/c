#!/bin/sh

echo ">Testing CountBAM"
make clean
make > compile.log

./CountBAM test/input.bam > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp CountBAM ~/bin/.
cp CountBAM ../bin/.
