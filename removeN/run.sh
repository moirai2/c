#!/bin/sh

echo ">Testing removeN"
make clean
make > compile.log
./removeN test/input.fq > test/output1.fq 2> error.log
diff test/output1.fq test/result1.fq
./removeN -N test/remove2.fq test/input.fq > test/output2.fq 2>> error.log
diff test/output2.fq test/result2.fq
diff test/remove2.fq test/result2remove.fq
cp removeN ~/bin/.
cp removeN ../bin/.