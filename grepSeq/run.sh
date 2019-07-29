#!/bin/sh

echo ">Testing grepSeq"
make clean
make > compile.log
./grepSeq AAA < test/input.fq > test/output1.fq 2> error.log
diff test/output1.fq test/result1.fq
./grepSeq -v AAA < test/input.fq > test/output2.fq 2>> error.log
diff test/output2.fq test/result2.fq
./grepSeq -i 2 3 < test/input.fq > test/output3.fq 2>> error.log
diff test/output3.fq test/result3.fq
./grepSeq -i 2 3 < test/input.fa > test/output4.fa 2>> error.log
diff test/output4.fa test/result4.fa
cp grepSeq ~/bin/.
cp grepSeq ../bin/.
