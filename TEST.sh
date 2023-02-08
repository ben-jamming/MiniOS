#!/bin/bash
cd src
make clean
make
cd ..

echo '' > RESULTS.txt
echo '' > DIRECT_RESULTS.txt

echo "Bad Command:" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/badcommand.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at Bad Command"
exit 1
fi
diff -w out.txt testcases/assignment1/badcommand_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
echo "------------------------" >> RESULTS.txt

echo "Blank Line:" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/blankline.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at Blank Line"
exit 1
fi
diff -w out.txt testcases/assignment1/blankline_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
echo "------------------------" >> RESULTS.txt

echo "Echo :" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/echo.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at Echo"
exit 1
fi
diff -w out.txt testcases/assignment1/echo_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
echo "------------------------" >> RESULTS.txt

echo "Ls:" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/ls.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at ls"
exit 1
fi
diff -w out.txt testcases/assignment1/ls_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
rm -r toto
echo "------------------------" >> RESULTS.txt

echo "Mkdir:" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/mkdir.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at mkdir"
exit 1
fi
diff -w out.txt testcases/assignment1/mkdir_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
rm -r test
echo "------------------------" >> RESULTS.txt

echo "Oneline:" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/oneline.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at one-line"
exit 1
fi
diff -w out.txt testcases/assignment1/oneline_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
echo "------------------------" >> RESULTS.txt

echo "Oneline2:" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/oneline2.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at one-line2"
exit 1
fi
diff -w out.txt testcases/assignment1/oneline2_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
rm -r testdir
echo "------------------------" >> RESULTS.txt

echo "Prompt:" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/prompt.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at prompt"
exit 1
fi
diff -w out.txt testcases/assignment1/prompt_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
echo "------------------------" >> RESULTS.txt 

echo "Set:" >> RESULTS.txt
timeout 10s src/mysh < testcases/assignment1/set.txt > out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution at set"
exit 1
fi
diff -w out.txt testcases/assignment1/set_result.txt >> RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
echo "------------------------" >> RESULTS.txt

echo "Run:" >> RESULTS.txt
cd src
timeout 10s ./mysh < ../testcases/assignment1/run.txt > ../out.txt
if [ $? -ne 0 ]; then
echo "Error in test file execution ar run"
exit 1
fi
diff -w ../out.txt ../testcases/assignment1/run_result.txt >> ../RESULTS.txt
cat out.txt >> DIRECT_RESULTS.txt
cd ..
echo "------------------------" >> RESULTS.txt


cat RESULTS.txt
rm out.txt
exit

