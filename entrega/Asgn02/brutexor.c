#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

// Import MD5 libraries - they're different for Linux and macOS.
// OpenMP is also only for Linux.
#if defined(__APPLE__)
#   define COMMON_DIGEST_FOR_OPENSSL
#   include <CommonCrypto/CommonDigest.h>
#   define SHA1 CC_SHA1
#else
#   include <openssl/md5.h>
#   include <omp.h>
#   define OPENMP
#endif

#define MAX_LEN 8

static const unsigned char input[20] = {0x0b, 0x34, 0x30, 0x20, 0x2f, 0x27, 0x05, 0x22, 0x05, 0x29, 0x21, 0x28, 0x22, 0x26, 0x19, 0x34, 0x23, 0x22, 0x27, 0x2d};
static const unsigned char hash[17] = {0x8d, 0x7b, 0x35, 0x6e, 0xae, 0x43, 0xad, 0xcd, 0x6a, 0xd3, 0xee, 0x12, 0x4c, 0x3d, 0xcf, 0x1e, '\0'};
static const char alphabet[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789";

static bool done = false;
static int alphabet_len;

static char *key;

/*
    Calculates the MD5 hash of str and puts the result in digest.
    Adds '\0' to end of digest to avoid memory trash.

    Input:
    char *str:       A string
    char digest[16]: The resulting hash
*/
void compute_md5(char *str, unsigned char digest[17]) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
    digest[16] = '\0';
}

/*
    Recursively "digs" through possible words of length max_len.
    Each word has its MD5 calculated and is tested against the
    program's input hash until the password is found.

    Input:
    char *word:     A string with the starting characters
                    (i.e. 'A', then 'AA', then 'AAA' until max_len-1)
    int index:      Current recursion depth (also number of characters in word)
    int max_len:    Number of characters of words to be tested

*/
void brute_force(char *word, int index, int max_len) {

    if (done) {
        return;
    }

    #if defined(OPENMP)
        int tid = omp_get_thread_num();
    #else
        int tid = 0;
    #endif
    
    if (index == max_len) {
        // Bottom of recursion, test word
        unsigned char digest[16];
        compute_md5(word, digest);

        if (!strcmp((char *) hash, (char *) digest)) {
            // Word found, end execution
            printf("word found in thread %d: %s\n", tid, word);
            done = true;
            key = strdup(word);
        }
    } else if (index < max_len) {
        // Word not long enough, go further
        int i;
        for (i=0; i<alphabet_len && !done; i++) {
            word[index] = alphabet[i];
            word[index+1] = '\0';

            brute_force(word, index+1, max_len);
        }
    }
}

int main(int argc, char *argv[]) {

    int i;
    alphabet_len = strlen(alphabet);

    #pragma omp parallel
    {

        #pragma omp single
        {

            // If OpenMP is disabled, number of threads is 1.
            #if defined(OPENMP)
                int num_threads = omp_get_num_threads();
            #else
                int num_threads = 1;
            #endif

            char *thread_alphabet[num_threads];
            int thread_alphabet_len = ((alphabet_len/num_threads)+1);

            // Each thread gets a portion of the alphabet allocated to it. 
            // {
            for (i = 0; i < num_threads; ++i) {
                thread_alphabet[i] = malloc( thread_alphabet_len * sizeof(char) );
                thread_alphabet[i][0] = '\0';
            }

            char temp[2];
            for (i=0; i<alphabet_len; i++) {
                sprintf(temp, "%c", alphabet[i]);
                strcat(thread_alphabet[i % num_threads], temp);
            }
            // }

            for (i=0; i<num_threads; i++) {

                // Invokes tasks for each thread
                #pragma omp task
                {
                    // If OpenMP is disabled, thread number is 0.
                    #if defined(OPENMP)
                        int tid = omp_get_thread_num();
                    #else
                        int tid = 0;
                    #endif   
                    int k;

                    // All words of a certain length (from 1 until MAX_LEN) will be
                    // computed before starting to try longer words
                    for (k=1; (k<=MAX_LEN) && !done; k++) {
                        printf("thread %d is testing words of length %d\n", tid, k);

                        // Each thread is responsible for words starting with the letters
                        // in its block of the alphabet
                        // i.e. thread 0 is responsible for all words beginning with A and C,
                        // while thread 1 is responsible for all words beginning with B and D.
                        int j;
                        for (j=0; j<thread_alphabet_len && !done; j++) {
                            char *word = malloc(9 * sizeof(char));
                            sprintf(word, "%c", thread_alphabet[tid][j]);
                            brute_force(word, 1, k);                        
                        }
                    }
                }
            }
        }
        #pragma omp taskwait
    }

    // Once the key is found, we use XOR with the input to
    // find out the clean string.

    char output[21];

    for (i=0; i<20; i++) {
        output[i] = input[i] ^ key[i % strlen(key)];
    }
    output[20] = '\0';

    printf("%s\n", output);

    return 0;
}

