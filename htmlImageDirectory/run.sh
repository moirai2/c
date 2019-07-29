#!/bin/sh

echo ">Testing htmlImageDirectory"
make clean
make > compile.log
./htmlImageDirectory -r test . > test/output1.html 2> error.log
./htmlImageDirectory -r test -l . > test/output2.html 2>> error.log
./htmlImageDirectory -r test -l -W 300 . > test/output3.html 2>> error.log
./htmlImageDirectory -r test -l -W 300 -H 300 . > test/output4.html 2>> error.log
./htmlImageDirectory -r test -f . > test/output5.html 2>> error.log
./htmlImageDirectory -r test -f -d "<BR>" . > test/output6.html 2>> error.log
./htmlImageDirectory -r test -l -f -d "<P>" . > test/output7.html 2>> error.log
diff test/output1.html test/result1.txt
diff test/output2.html test/result2.txt
diff test/output3.html test/result3.txt
diff test/output4.html test/result4.txt
diff test/output5.html test/result5.txt
diff test/output6.html test/result6.txt
diff test/output7.html test/result7.txt
cp htmlImageDirectory ~/bin/.
cp htmlImageDirectory ../bin/.