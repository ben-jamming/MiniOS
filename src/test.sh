#!/bin/bash
# Test 3
# modify from devtool.sh

# prepare env (cwd: src)

if [[ "$1" == "all" ]]; then
    rm -rf ../env/ && mkdir ../env
    cp ./P_* ../env
    
    for i in `seq 1 1 10`
    do 
        make clean
        if [ $i == 3 ] || [ $i == 8 ]; then
            make mysh framesize=21 varmemsize=10
        elif [ $i == 5 ] || [ $i == 10 ]; then
            make mysh framesize=6 varmemsize=10
        else
            make mysh framesize=18 varmemsize=10
        fi

        cd ..
        cp ./src/mysh ./env
        cd env

        file="../testcases/assignment3/T_tc$i.txt"
        echo $file
        result_file=${file%.txt}_result.txt
        ./mysh < $file | diff -Bw <(cat $result_file) -
        cd ../src
    done

else
    make clean
    if [ $1 == 3 ] || [ $1 == 8 ]; then
        make mysh framesize=21 varmemsize=10
    elif [ $1 == 5 ] || [ $1 == 10 ]; then
        make mysh framesize=6 varmemsize=10
    else
        make mysh framesize=18 varmemsize=10
    fi

    cd ..
    rm -rf env/ && mkdir env
    cp ./testcases/assignment3/P_* ./env
    cp ./src/mysh ./env

    cd env

    file="../testcases/assignment3/T_tc$1.txt"
    echo $file
    result_file=${file%.txt}_result.txt
    ./mysh < $file | diff -Bw <(cat $result_file) -
fi