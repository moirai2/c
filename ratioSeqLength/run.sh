#!/bin/sh

echo ">Testing ratioSeqLength"
make clean
make > compile.log
./ratioSeqLength test/dir2 test/dir1 > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./ratioSeqLength -l 'A,B' test/dir2 test/dir1 > test/output2.txt 2> error.log
diff test/output2.txt test/result2.txt
./ratioSeqLength -t 'diff' -l 'A,B' test/dir2 test/dir1 > test/output3.txt 2> error.log
diff test/output3.txt test/result3.txt
cp ratioSeqLength ~/bin/.
cp ratioSeqLength ../bin/.
