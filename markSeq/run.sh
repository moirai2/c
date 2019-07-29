#!/bin/sh

echo ">Testing markSeq"
make clean
make > compile.log
./markSeq test/reference1.fa test/input1.fa > test/output1.fa 2> error.log
diff test/output1.fa test/result1.fa
./markSeq -f '#' -r '=' -I test/output1.fa test/reference2.fa test/input1.fa > test/output2.fa 2>> error.log
diff test/output2.fa test/result2.fa
./markSeq -f '#' -r '=' -I stdin test/reference2.fa test/input1.fa < test/output1.fa > test/output3.fa 2>> error.log
diff test/output3.fa test/result3.fa
./markSeq -m 1 test/reference3.fa test/input1.fa > test/output4.fa 2> error.log
diff test/output4.fa test/result4.fa
./markSeq -m 1 test/reference4.fa test/input2.fa > test/output5.fa 2> error.log
diff test/output5.fa test/result5.fa
./markSeq -m 1 test/reference4.fa test/input3.fa > test/output6.fa 2> error.log
diff test/output6.fa test/result6.fa
cp markSeq ~/bin/.
cp markSeq ../bin/.