#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1

void compute_md5(char *str, unsigned char digest[16]) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
}

#else
#  include <openssl/md5.h>

void compute_md5(char *str, unsigned char digest[16]) {
    MD5Context ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, str, strlen(str));
    MD5Final(digest, &ctx);
}

#endif

static const unsigned char hash[17] = {0x8d, 0x7b, 0x35, 0x6e, 0xae, 0x43, 0xad, 0xcd, 0x6a, 0xd3, 0xee, 0x12, 0x4c, 0x3d, 0xcf, 0x1e, '\0'};
static unsigned char digest[17];
static const char alphabet[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789";

char *bruteForce(char *word, int index, int max_len) {

    for (int i=11; i<62; i++) {
        word[index] = alphabet[i];
        word[index+1] = '\0';

        compute_md5(word, digest);

        if (!strcmp((char *) hash, (char *) digest)) {
            return word;
        }

        if (index < max_len) {
            char *res = bruteForce(word, index+1, max_len);
            if (res != NULL) {
                return res;
            }
        }
        
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    char output[21];

    char *word = malloc(9 * sizeof(char));

    bruteForce(word, 0, 8);

    printf("%s\n", word);

    char input[20] = {0x0b, 0x34, 0x30, 0x20, 0x2f, 0x27, 0x05, 0x22, 0x05, 0x29, 0x21, 0x28, 0x22, 0x26, 0x19, 0x34, 0x23, 0x22, 0x27, 0x2d};

    int len = strlen(word);

    for (int i=0; i<20; i++) {
        output[i] = input[i] ^ word[i % len];
    }

    output[20] = '\0';

    printf("%s\n", output);

    return 0;
}
