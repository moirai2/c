#!/bin/sh

echo ">Testing splitPairedEndWithBarcodeFile"
make clean
make > compile.log
./splitPairedEndWithBarcodeFile -b AAA,CCC,GGG,TTT test/input1.fq test/input2.fq test/input3.fq test/test1 test/test2 2> error.log
diff test/test1/input1.AAA.fq test/test1/result1.AAA.fq
diff test/test1/input1.CCC.fq test/test1/result1.CCC.fq
diff test/test1/input1.GGG.fq test/test1/result1.GGG.fq
diff test/test1/input1.TTT.fq test/test1/result1.TTT.fq
diff test/test2/input2.AAA.fq test/test2/result2.AAA.fq
diff test/test2/input2.CCC.fq test/test2/result2.CCC.fq
diff test/test2/input2.GGG.fq test/test2/result2.GGG.fq
diff test/test2/input2.TTT.fq test/test2/result2.TTT.fq

cp splitPairedEndWithBarcodeFile ~/bin/.
cp splitPairedEndWithBarcodeFile ../bin/.