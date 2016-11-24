#!/bin/bash
FILE="portscan"

if [ -x "$FILE" ];
then
    echo "File $FILE exists and is writable."
else
    wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/portscan.c
    wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/exploit.c
    sleep 10
    gcc -o portscan portscan.c exploit.c
    #rm -f *.o
    portscan > portscan.out
fi