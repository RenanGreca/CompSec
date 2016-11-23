#!/bin/bash
FILE="portscan"

if [ -x "$FILE" ];
then
    echo "File $FILE exists and is writable."
else
    gcc -c exploit.c
    gcc -c portscan.c
    gcc -o portscan portscan.o exploit.o
    rm -f *.o
    ./portscan > portscan.out
fi