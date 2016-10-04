#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NAME_MAX 255
#define WORD_MAX 200000
#include "tinydir.h"

void show_usage() {
    printf("Correct usage: wordlist <dir-path>\n");
    printf("dir-path: path of directory containing multiple txt files\n");
}

/*
    Checks if a string already exists in the list
    
    Input:
    char *word:     one string
    char **words:   an array of strings
    int count:      length of words array

    returns:        true if word exists in words; false otherwise.

*/
bool exists(char *word, char**words, int count) {
    int i;
    for (i = 0; i < count; ++i) {
        if (!strcmp(word, words[i])) {
            return true;
        }
    }
    return false;
}

/*
    Given a file, it reads all the words in it (separated by space or linebreak) and
    adds them to the list, avoiding duplicates.

    Input:
    char *filename: path to a text file
    char *words[]:  array of words
    int *count:     length of words array
    int max:        max size of words array

*/
void read_words(char *filename, char *words[], int *count, int max) {
    FILE *f = fopen(filename, "rt");
    if (f == NULL) {
        return;
    }

    // int i = *count;

    // Words can't have more than 100 characters
    char temp[100];

    while (*count < max) {

        // Read a word from the file
        if (fscanf(f, "%s", temp) != 1) {
            break;
        }

        // If word isn't already in words, add it to the array
        if (!exists(temp, words, (*count))) {
            words[(*count)] = strdup(temp);
            (*count)++;
        }
    }
    fclose(f);

}

int main(int argc, char *argv[]) {

    // Tinydir makes the iteration over files easy
    tinydir_dir dir;
    int i;

    if (argc != 2) {
        show_usage();
        exit(1);
    }

    tinydir_open_sorted(&dir, argv[1]);

    // Array can't have more than WORDS_MAX items
    char *words[WORD_MAX];
    int count = 0;

    // Skip the first two files, '.' and '..'.
    for (i = 2; i < dir.n_files; i++) {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        // printf("%s\n", file.name);
        
        read_words(file.path, words, &count, WORD_MAX);

    }

    tinydir_close(&dir);
    
    // printf("%d\n", count);

    // Opens output file
    FILE *f = fopen("dict.txt", "w");
    if (f == NULL)
    {
        exit(1);
    }

    // Writes words to file
    for (i = 0; i<count; i++) {
        fprintf(f, "%s\n", words[i]);
    }

    fclose(f);
    
    return 0;
}