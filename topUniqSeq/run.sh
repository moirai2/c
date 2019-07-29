#!/bin/sh

echo ">Testing topUniqSeq"
make clean
make > compile.log

./topUniqSeq -n 15 test/input1.txt > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./topUniqSeq test/input1.txt > test/output1.txt 2>> error.log
diff test/output2.txt test/result2.txt

./topUniqSeq -n 'AAA,AAC' test/input1.txt > test/output2.txt 2> error.log
diff test/output2.txt test/result2.txt

cp topUniqSeq ~/bin/.
cp topUniqSeq ../bin/.