#!/bin/sh

echo ">Testing bamMarkBestMap"
make clean
make > compile.log
./bamMarkBestMap -f test/input.sam > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp bamMarkBestMap ~/bin/.
cp bamMarkBestMap ../bin/.
