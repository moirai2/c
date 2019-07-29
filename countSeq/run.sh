#!/bin/sh

echo ">Testing countSeq"
make clean
make > compile.log
./countSeq test/input > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./countSeq -n test/input > test/output2.txt 2> error.log
diff test/output2.txt test/result2.txt
./countSeq -n -N "sequence count %d" test/input > test/output3.txt 2> error.log
diff test/output3.txt test/result3.txt
cp countSeq ~/bin/.
cp countSeq ../bin/.

