all: portscan 

portscan: portscan.o exploit.o
	gcc -o portscan portscan.o exploit.o

portscan.o: portscan.c
	gcc -c portscan.c

exploit.o: exploit.c
	gcc -c exploit.c

clean:
	rm exploit.o portscan.o portscan *.aux *.log *.blg *.bbl *.synctex.gz