#!/bin/sh

mkdir -p bin
argument=$1
for program in compareBAM convertToFasta convertToFastq splitPairedEndWithBarcodeFile splitPairedEndByBarcode sampleTopSeq bamMarkBestMap convertSeq dotMatrix CountUnknownFile trimBaseN sampleSeq htmlSeq markSeq bamStartPosition chromosomeCount redundancySeq bamInsertSize colorSeq matchPairedEndSeq ratioSeqLength topUniqSeq ratioSeq fillSeq selectSeqLength uniqText sortText htmlImageDirectory listDirectory grepSeq simpleSeq openSeq splitTruSeqByBarcode checkFastqFormat removeN splitByBarcode CountTEXT CountFASTA CountFASTQ CountSAM CountBAM CountUnknownFile subtractSeq baseSeq countSeq lengthSeq qvSeq uniqSeq
do
cd ${program}
if [ "${argument}" = "compile" ]
then
make clean
make
cp ${program} ../bin
elif [ "${argument}" = "clean" ]
then
make clean
rm Makefile.KgL
rm compile.log
rm error.log
else
./run.sh
fi
cd ..
done
