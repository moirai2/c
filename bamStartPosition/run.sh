#!/bin/sh

echo ">Testing bamStartPosition"
make clean
make > compile.log
./bamStartPosition test/input.sam > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp bamStartPosition ~/bin/.
cp bamStartPosition ../bin/.
