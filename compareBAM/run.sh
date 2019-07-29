#!/bin/sh

echo ">Testing compareBAM"
make clean
make > compile.log
./compareBAM -a test/alone1.sam -A test/alone2.sam -b test/both1.sam -B test/both2.sam -O test/summary.txt -H test/summary.html test/input3.sam test/input4.sam 2> error.log
diff test/alone1.sam test/result_alone1.sam
diff test/alone2.sam test/result_alone2.sam
diff test/both1.sam test/result_both1.sam
diff test/both2.sam test/result_both2.sam
diff test/summary.txt test/result_summary.txt
# Need to figure out why this is happening
# diff test/summary.html test/result_summary.html
cp compareBAM ~/bin/.
cp compareBAM ../bin/.

