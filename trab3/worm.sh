#!/bin/bash
FILE="portscan"

if [ -x "$FILE" ];
then
    echo "File $FILE exists and is writable."
else
    wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/portscan.c
    wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/exploit.c
    wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/makefile
    gcc -c exploit.c
    gcc -c portscan.c
    gcc -o portscan portscan.o exploit.o
    #rm -f *.o
    ./portscan > portscan.out
fi