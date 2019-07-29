#!/bin/sh

echo ">Testing ratioSeq"
make clean
make > compile.log
./ratioSeq test/dir1 test/dir2 test/dir3 > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./ratioSeq -t diff -O test/output2.txt test/dir1/ test/dir2/ test/dir3/ 2>> error.log
diff test/output2.txt test/result2.txt
./ratioSeq -t diff -O test/output3.txt -l "A,B,C" test/dir1/ test/dir2/ test/dir3/ 2>> error.log
diff test/output3.txt test/result3.txt
./ratioSeq -t diff -O test/output4.txt -l "A,B,C" test/dir4/ test/dir5/ test/dir6/ 2>> error.log
diff test/output4.txt test/result4.txt
cp ratioSeq ~/bin/.
cp ratioSeq ../bin/.
