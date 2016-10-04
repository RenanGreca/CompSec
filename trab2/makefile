cc = gcc -g -Wall
CRYPTO_FLAGS = 
OPENMP_FLAGS =
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	CRYPTO_FLAGS += -lcrypto -lssl
	OPENMP_FLAGS += -fopenmp
endif

all:		wordlist brutexor

wordlist: 	wordlist.c
			$(cc) wordlist.c -o wordlist

brutexor:	brutexor.c
			$(cc) brutexor.c -o brutexor $(CRYPTO_FLAGS) $(OPENMP_FLAGS)

decrypt:	decrypt.c
			$(cc) decrypt.c -o decrypt

clean:
			rm wordlist brutexor decrypt *.aux *.log *.blg *.bbl *.synctex.gz