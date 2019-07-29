#!/bin/sh

echo ">Testing bamInsertSize"
make clean
make > compile.log
./bamInsertSize -l 100 test/input.sam > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./bamInsertSize -i -l 100 test/input.sam > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
cp bamInsertSize ~/bin/.
cp bamInsertSize ../bin/.
