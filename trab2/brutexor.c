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

void compute_md5(char *str, unsigned char digest[16]) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
}

char *bruteForce(char *word, int index, int max_len) {

    unsigned char digest[17];
    int tid = omp_get_thread_num();
    int i;

    // printf("%s\n", word);

    compute_md5(word, digest);
    if (strcmp((char *) hash, (char *) digest) == 0) {
        //printf("%d %d\n", hash, digest);
        printf("word found in thread %d: %s\n", tid, word);
        done = true;
        key = strdup(word);
        return word;
    }

    if (index < max_len) {
        for (i=0; i<alphabet_len; i++) {
            word[index] = alphabet[i];
            word[index+1] = '\0';

            if (!done) {
                char *res = bruteForce(word, index+1, max_len);
                if (res != NULL) {
                    return res;
                }
            } else {
                return NULL;
            }

        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    int i;
    alphabet_len = strlen(alphabet);

    #pragma omp parallel
    {

        #pragma omp single
        {

            int user_num_threads = atoi(argv[1]);

            int num_threads = omp_get_num_threads();

            if (user_num_threads < num_threads) {
                num_threads = user_num_threads;
            }

            char *thread_alphabet[num_threads];
            int word_len = ((alphabet_len/num_threads)+1);

            for (i = 0; i < num_threads; ++i) {
                thread_alphabet[i] = malloc( word_len * sizeof(char) );
                thread_alphabet[i][0] = '\0';
            }

            char temp[2];
            for (i=0; i<alphabet_len; i++) {
                sprintf(temp, "%c", alphabet[i]);
                strcat(thread_alphabet[i % num_threads], temp);
            }

            for (i=0; i<num_threads; i++) {

                #pragma omp task
                {
                    int tid = omp_get_thread_num();
                    int k;
                    for (k=1; (k<=MAX_LEN) && !done; k++) {
                        printf("thread %d is testing words of length %d\n", tid, k);

                        int j;
                        for (j=0; j<word_len && !done; j++) {
                            char *word = malloc(9 * sizeof(char));
                            sprintf(word, "%c", thread_alphabet[tid][j]);
                            bruteForce(word, 1, k);                        
                        }
                    }
                }
            }
        }
        #pragma omp taskwait
    }

    char output[20];

    for (i=0; i<20; i++) {
        output[i] = input[i] ^ key[i % strlen(key)];
    }

    printf("%s\n", output);

    return 0;
}

