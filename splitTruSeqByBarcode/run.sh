#!/bin/sh

echo ">Testing splitTruSeqByBarcode"
make clean
make > compile.log
./splitTruSeqByBarcode -b AAA,CCC,GGG,TTT -O test test/input1.fq test/input2.fq 2> error.log
diff test/input1.AAA.fq test/result.AAA.fq
diff test/input1.CCC.fq test/result.CCC.fq
diff test/input1.GGG.fq test/result.GGG.fq
diff test/input1.TTT.fq test/result.TTT.fq
./splitTruSeqByBarcode -b AAA,CCC,GGG,TTT -n output -N test/output.NNN.fq -O test test/input2.fq < test/input1.fq 2>> error.log
diff test/output.AAA.fq test/result.AAA.fq
diff test/output.CCC.fq test/result.CCC.fq
diff test/output.GGG.fq test/result.GGG.fq
diff test/output.TTT.fq test/result.TTT.fq
diff test/output.NNN.fq test/result.NNN.fq
cp splitTruSeqByBarcode ~/bin/.
cp splitTruSeqByBarcode ../bin/.