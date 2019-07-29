#!/bin/sh

echo ">Testing CountSAM"
make clean
make > compile.log
./CountSAM test/input.sam > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp CountSAM ~/bin/.
cp CountSAM ../bin/.
