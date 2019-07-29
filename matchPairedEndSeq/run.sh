#!/bin/sh

echo ">Testing matchPairedEndSeq"
make clean
make > compile.log
./matchPairedEndSeq test/original5.fq test/original3.fq test/filtered5.fq test/filtered3.fq test/matched5.fq test/matched3.fq 2> error.log
diff test/matched5.fq test/result5.fq
diff test/matched3.fq test/result3.fq
cp matchPairedEndSeq ~/bin/.
cp matchPairedEndSeq ../bin/.