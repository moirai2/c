#!/bin/sh

echo ">Testing sortText"
make clean
make > compile.log

./sortText test/input.txt > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt

cp sortText ~/bin/.
cp sortText ../bin/.
