#!/bin/sh

echo ">Testing convertToFastq"
make clean
make > compile.log
./convertToFastq test/input.sam > test/output1.fq 2> error.log
diff test/output1.fq test/result1.fq
cp convertToFastq ~/bin/.
cp convertToFastq ../bin/.
