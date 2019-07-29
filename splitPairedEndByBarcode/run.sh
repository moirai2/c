#!/bin/sh

echo ">Testing splitPairedEndByBarcode"
make clean
make > compile.log
./splitPairedEndByBarcode -b AAA,CCC,GGG,TTT test/input1read1.fq test/input1read2.fq test/test1read1 test/test1read2 2> error.log
diff test/test1read1/input1read1.AAA.fq test/test1read1/result1.AAA.fq
diff test/test1read1/input1read1.CCC.fq test/test1read1/result1.CCC.fq
diff test/test1read1/input1read1.GGG.fq test/test1read1/result1.GGG.fq
diff test/test1read1/input1read1.TTT.fq test/test1read1/result1.TTT.fq
diff test/test1read2/input1read2.AAA.fq test/test1read2/result1.AAA.fq
diff test/test1read2/input1read2.CCC.fq test/test1read2/result1.CCC.fq
diff test/test1read2/input1read2.GGG.fq test/test1read2/result1.GGG.fq
diff test/test1read2/input1read2.TTT.fq test/test1read2/result1.TTT.fq

./splitPairedEndByBarcode -b AAA,CCC,GGG,TTT -r test/input1read1.fq test/input1read2.fq test/test2read1 test/test2read2 2> error.log
diff test/test2read1/input1read1.AAA.fq test/test2read1/result2.AAA.fq
diff test/test2read1/input1read1.CCC.fq test/test2read1/result2.CCC.fq
diff test/test2read1/input1read1.GGG.fq test/test2read1/result2.GGG.fq
diff test/test2read1/input1read1.TTT.fq test/test2read1/result2.TTT.fq
diff test/test2read2/input1read2.AAA.fq test/test2read2/result2.AAA.fq
diff test/test2read2/input1read2.CCC.fq test/test2read2/result2.CCC.fq
diff test/test2read2/input1read2.GGG.fq test/test2read2/result2.GGG.fq
diff test/test2read2/input1read2.TTT.fq test/test2read2/result2.TTT.fq

./splitPairedEndByBarcode -b AAA,CCC,GGG,TTT -r -t test/input2read1.fq test/input2read2.fq test/test3read1 test/test3read2 2> error.log
diff test/test3read1/input2read1.AAA.fq test/test3read1/result3.AAA.fq
diff test/test3read1/input2read1.CCC.fq test/test3read1/result3.CCC.fq
diff test/test3read1/input2read1.GGG.fq test/test3read1/result3.GGG.fq
diff test/test3read1/input2read1.TTT.fq test/test3read1/result3.TTT.fq
diff test/test3read2/input2read2.AAA.fq test/test3read2/result3.AAA.fq
diff test/test3read2/input2read2.CCC.fq test/test3read2/result3.CCC.fq
diff test/test3read2/input2read2.GGG.fq test/test3read2/result3.GGG.fq
diff test/test3read2/input2read2.TTT.fq test/test3read2/result3.TTT.fq

./splitPairedEndByBarcode -b '' -r -t test/input2read1.fq test/input2read2.fq test/test4read1 test/test4read2 2> error.log
diff test/test4read1/input2read1.nobarcode.fq test/test4read1/result4.nobarcode.fq
diff test/test4read2/input2read2.nobarcode.fq test/test4read2/result4.nobarcode.fq

./splitPairedEndByBarcode -r -B test/barcode.txt -f filename -b barcode -e regular_expression -M test/test5read1/nobarcode.fq -N test/test5read2/nobarcode.fq  test/input1read1.fq test/input1read2.fq test/test5read1 test/test5read2 2> error.log
diff test/test5read1/input1read1.AAA.fq test/test5read1/result5read1.AAA.fq
diff test/test5read1/input1read1.CCC.fq test/test5read1/result5read1.CCC.fq
diff test/test5read2/input1read2.AAA.fq test/test5read2/result5read2.AAA.fq
diff test/test5read2/input1read2.CCC.fq test/test5read2/result5read2.CCC.fq
diff test/test5read1/nobarcode.fq test/test5read1/result5.nobarcode.fq
diff test/test5read2/nobarcode.fq test/test5read2/result5.nobarcode.fq

cp splitPairedEndByBarcode ~/bin/.
cp splitPairedEndByBarcode ../bin/.