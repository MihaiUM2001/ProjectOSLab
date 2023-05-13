#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Error: file does not exist!\n\n"
    exit 1
fi

file=$1
out=$(gcc -Wall -o ${file%.c} $file 2>&1)
warning=$(echo "$out" | grep "warning:" | wc -l)
error=$(echo "$out" | grep "error:" | wc -l)
echo -e "$warning, $error, $file"