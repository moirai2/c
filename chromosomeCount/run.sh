#!/bin/sh

echo ">Testing chromosomeCount"
make clean
make > compile.log
./chromosomeCount test/input.sam > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
cp chromosomeCount ~/bin/.
cp chromosomeCount ../bin/.
