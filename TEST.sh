#!/bin/bash
cd src
make clean



echo '' > ../RESULTS.txt
echo '' > ../out.txt

# Loop through all files in the directory
for file in ../testcases/assignment3/*; do
    # Check if file is a regular file and doesn't end with "result.txt" or "result2.txt"
    if [[ -f "$file" && "$file" != *"result.txt" && "$file" != *"result2.txt" && "$file" ]]; then
        echo "------------------------------------------------------------------------" >> ../RESULTS.txt
        # Extract the base name of the file by removing the extension
        base="${file%.*}"
        # Execute mysh script with file as input and write result of execution to out.txt
        echo ${file}

        # Extract the third line of the file
        line=$(sed -n '3p' "${base}_result.txt")

        # Extract the values of X and Y using awk
        frame_size=$(echo "$line" | awk -F' = ' '{print $2}' | awk -F';' '{print $1}')
        variable_size=$(echo "$line" | awk -F' = ' '{print $3}' | awk -F';' '{print $1}')

        # Print the values of X and Y
        echo "File: $file, Frame Store Size: $frame_size, Variable Store Size: $variable_size"
        make -B mysh framesize=$frame_size varmemsize=$variable_size

        timeout 10s ./mysh < ${file} > ../out.txt
        #Throw an error
        if [ $? -ne 0 ]; then
            echo "Error in test file execution of: $file"
            exit 1
        fi
        #echo "Actual output of $file" >> ../RESULTS.txt
        #cat ../out.txt >> ../RESULTS.txt
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


