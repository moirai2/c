#!/bin/sh

echo ">Testing splitByBarcode"
make clean
make > compile.log
./splitByBarcode -b AAA,CCC,GGG,TTT -N test/input1.nomatch.fq -O test test/input1.fq 2> error.log
diff test/input1.AAA.fq test/result1.AAA.fq
diff test/input1.CCC.fq test/result1.CCC.fq
diff test/input1.GGG.fq test/result1.GGG.fq
diff test/input1.TTT.fq test/result1.TTT.fq
diff test/input1.nomatch.fq test/result1.nomatch.fq
./splitByBarcode -t -b AAA,CCC,GGG,TTT -n output2 -N test/output2.nomatch.fq -O test test/input2.fq 2> error.log
diff test/output2.AAA.fq test/result2.AAA.fq
diff test/output2.CCC.fq test/result2.CCC.fq
diff test/output2.GGG.fq test/result2.GGG.fq
diff test/output2.TTT.fq test/result2.TTT.fq
diff test/output2.nomatch.fq test/result2.nomatch.fq
./splitByBarcode -t -b AAA -n output3 -N test/output3.nomatch.fq -O test test/input3.fq 2> error.log
diff test/output3.AAA.fq test/result3.AAA.fq
diff test/output3.nomatch.fq test/result3.nomatch.fq
./splitByBarcode -t -b '' -n output4 -N test/output4.nomatch.fq -O test test/input4.fq 2> error.log
diff test/output4.nobarcode.fq test/result4.nobarcode.fq
diff test/output4.nomatch.fq test/result4.nomatch.fq
./splitByBarcode -B test/barcode.txt -f filename -b barcode -e regular_expression -n output5 -N test/output5.nomatch.fq -O test test/input5.fq 2> error.log
diff test/output5.AAA.fq test/result5.AAA.fq
diff test/output5.CCC.fq test/result5.CCC.fq
diff test/output5.nomatch.fq test/result5.nomatch.fq
./splitByBarcode -B test/barcode.txt -f filename -b barcode -e regular_expression -n output6 -N test/output6.nomatch.fq -O test test/input6.fq 2> error.log
diff test/output6.GGG.fq test/result6.GGG.fq
diff test/output6.TTT.fq test/result6.TTT.fq
diff test/output6.nomatch.fq test/result6.nomatch.fq
cp splitByBarcode ~/bin/.
cp splitByBarcode ../bin/.