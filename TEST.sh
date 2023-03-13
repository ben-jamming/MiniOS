#!/bin/bash
cd src
make clean
make


echo '' > ../RESULTS.txt
echo '' > ../out.txt

# Loop through all files in the directory
for file in ../testcases/assignment2/*; do
    # Check if file is a regular file and doesn't end with "result.txt" or "result2.txt"
    if [[ -f "$file" && "$file" != *"result.txt" && "$file" != *"result2.txt" && "$file" ]]; then
        echo "------------------------------------------------------------------------" >> ../RESULTS.txt
        # Extract the base name of the file by removing the extension
        base="${file%.*}"
        # Execute mysh script with file as input and write result of execution to out.txt
        if [[ -f "$file" && "$file" != "../testcases/assignment2/T_background.txt" && "$file" != "../testcases/assignment2/T_RR30.txt" ]]; then

            echo ${file}
            timeout 10s ./mysh < ${file} > ../out.txt
        fi

        #Throw an error
        if [ $? -ne 0 ]; then
            echo "Error in test file execution of: $file"
            exit 1
        fi
        echo "Actual output of $file" >> ../RESULTS.txt
        cat ../out.txt >> ../RESULTS.txt
        # Check if the corresponding result files exist
        if [[ -f "${base}_result.txt" ]]; then
            echo "difference between expected output of $file and actual in ${base}_result.txt:" >> ../RESULTS.txt
            #Write the difference between out.txt and file_result.txt to RESULTS.txt
            diff -w ../out.txt "${base}_result.txt" >> ../RESULTS.txt
        fi

        if [[ -f "${base}_result2.txt" ]]; then
            echo "difference between expected output of $file and actual in ${base}_result2.txt:\n" >> ../RESULTS.txt
            diff -w ../out.txt "${base}_result2.txt" >> ../RESULTS.txt
        fi
        
    fi
done

#cat RESULTS.txt
rm ../out.txt
exit


