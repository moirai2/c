#!/bin/sh

echo ">Testing CountTEXT"
make clean
make > compile.log
./CountTEXT test/input.txt > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp CountTEXT ~/bin/.
cp CountTEXT ../bin/.
