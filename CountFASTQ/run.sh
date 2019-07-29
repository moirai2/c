#!/bin/sh

echo ">Testing CountFASTQ"
make clean
make > compile.log
./CountFASTQ test/input.fq > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp CountFASTQ ~/bin/.
cp CountFASTQ ../bin/.
