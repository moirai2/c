#!/bin/sh

echo ">Testing CountUnknownFile"
make clean
make > compile.log
./CountUnknownFile test/input.txt > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp CountUnknownFile ~/bin/.
cp CountUnknownFile ../bin/.
