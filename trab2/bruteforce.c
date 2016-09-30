#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>

#include <string.h>

#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
#  include <openssl/md5.h>
#endif

#define MAX_LEN 8

static const unsigned char hash[17] = {0x8d, 0x7b, 0x35, 0x6e, 0xae, 0x43, 0xad, 0xcd, 0x6a, 0xd3, 0xee, 0x12, 0x4c, 0x3d, 0xcf, 0x1e, '\0'};
static const char alphabet[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789";

static bool done = false;

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
    for (i=0; i<strlen(alphabet); i++) {
        word[index] = alphabet[i];
        word[index+1] = '\0';

        if (!done) {
            if (index < max_len-1) {
                char *res = bruteForce(word, index+1, max_len);
                if (res != NULL) {
                    return res;
                }
            } else {
                compute_md5(word, digest);
                if (strcmp((char *) hash, (char *) digest) == 0) {
                    //printf("%d %d\n", hash, digest);
                    printf("word found in thread %d: %s\n", tid, word);
                    done = true;
                    return word;
                }
            }
        }

    }

    return NULL;
}

int main(int argc, char *argv[]) {
    char *thread_alphabet[8];

    int i;
    thread_alphabet[0] = "ABCDEFGH";
    thread_alphabet[1] = "IJKLMNOP";
    thread_alphabet[2] = "QRSTUVWX";
    thread_alphabet[3] = "YZabcdef";
    thread_alphabet[4] = "ghiklmno";
    thread_alphabet[5] = "pqrstuvw";
    thread_alphabet[6] = "xyz01234";
    thread_alphabet[7] = "56789";


    #pragma omp parallel
    {
        #pragma omp single

        for (i=0; i<8; i++) {

            #pragma omp task
            {
                int tid = omp_get_thread_num();
                int k;
                for (k=1; (k<=MAX_LEN) && !done; k++) {
                    printf("thread %d is testing words of length %d\n", tid, k);

                    int j;
                    for (j=0; j<strlen(thread_alphabet[tid]) && !done; j++) {
                        char *word = malloc(9 * sizeof(char));
                        sprintf(word, "%c", thread_alphabet[tid][j]);
                        bruteForce(word, 1, k);                        
                    }
                }
            }
        }
        #pragma omp taskwait
    }

    return 0;
}

