#include <stdio.h>
#include <stdlib.h>
// #include <dirent.h>
#define NAME_MAX 255
#define WORD_MAX 200000
#include "tinydir.h"

void ReadWords(const char *filename, char *words[], int *count, int max_number_of_words) {
    FILE *f = fopen(filename, "rt"); // checking for NULL is boring; i omit it
    int i;
    char temp[100]; // assuming the words cannot be too long

    for (i = *count; i < max_number_of_words; ++i)
    {
        // Read a word from the file
        if (fscanf(f, "%s", temp) != 1)
            break;
        // note: "!=1" checks for end-of-file; using feof for that is usually a bug

        // Allocate memory for the word, because temp is too temporary
        //printf("%s\n", temp);
        words[i] = strdup(temp);
    }
    fclose(f);

    *count = i;
}

int main(int argc, char *argv[]) {

    printf("Hello, world\n");
    tinydir_dir dir;
    int i;
    tinydir_open_sorted(&dir, "/Users/renangreca/OneDrive/repos/compsec/trab2/legiaourbana");

    char *words[WORD_MAX];
    int count = 0;

    for (i = 0; i < dir.n_files; i++)
    {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        printf("%s\n", file.name);
        
        ReadWords(file.path, words, &count, WORD_MAX);

    }

    tinydir_close(&dir);
    
    printf("%d\n", count);

    FILE *f = fopen("dict.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    /* print some text */
    for (int i = 0; i<count; i++) {
        fprintf(f, "%s\n", words[i]);
    }

    fclose(f);
    
}