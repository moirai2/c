#!/bin/sh

echo ">Testing uniqText"
make clean
make > compile.log

./uniqText test/input.txt > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./uniqText -c test/input.txt > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
cp uniqText ~/bin/.
cp uniqText ../bin/.
