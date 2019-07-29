#!/bin/sh

echo ">Testing simpleSeq"
make clean
make > compile.log
./simpleSeq test/input.fa > test/output1.txt 2> error.log
diff test/output1.txt test/result1.txt
./simpleSeq < test/input.fq > test/output2.txt 2>> error.log
diff test/output2.txt test/result2.txt
./simpleSeq < test/input.sam > test/output3.txt 2>> error.log
diff test/output3.txt test/result3.txt
./simpleSeq test/input.fa.gz > test/output4.txt 2>> error.log
diff test/output4.txt test/result4.txt
./simpleSeq test/input.fq.gz > test/output5.txt 2>> error.log
diff test/output5.txt test/result5.txt
./simpleSeq < test/input2.sam > test/output6.txt 2>> error.log
diff test/output6.txt test/result6.txt
./simpleSeq < test/input.txt > test/output7.txt 2>> error.log
diff test/output7.txt test/result7.txt
./simpleSeq test/input3.sam > test/output8.txt 2>> error.log
diff test/output8.txt test/result8.txt
cp simpleSeq ~/bin/.
cp simpleSeq ../bin/.