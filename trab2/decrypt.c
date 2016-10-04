#include <stdio.h>
#include <string.h>

#if defined(__APPLE__)
#   define COMMON_DIGEST_FOR_OPENSSL
#   include <CommonCrypto/CommonDigest.h>
#   define SHA1 CC_SHA1
#else
#   include <openssl/md5.h>
#   define OPENMP
#endif

static const unsigned char hash[17] = {0x8d, 0x7b, 0x35, 0x6e, 0xae, 0x43, 0xad, 0xcd, 0x6a, 0xd3, 0xee, 0x12, 0x4c, 0x3d, 0xcf, 0x1e, '\0'};

void compute_md5(char *str, unsigned char digest[17]) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
    digest[16] = '\0';
}

int main(int argc, char *argv[]) {

    char input[20] = {0x0b, 0x34, 0x30, 0x20, 0x2f, 0x27, 0x05, 0x22, 0x05, 0x29, 0x21, 0x28, 0x22, 0x26, 0x19, 0x34, 0x23, 0x22, 0x27, 0x2d};
    char key[20]   = "FACIL";
    char output[20];
    unsigned char digest[17];

    compute_md5(key, digest);
    printf("%s\n%s\n", hash, digest);
    if (!strcmp((char *) hash, (char *) digest)) {
        printf("word found: %s\n", key);
    }

    printf("%s\n", input);
    printf("%s\n", key);

    int i;
    for (i=0; i<20; i++) {
        output[i] = input[i] ^ key[i % strlen(key)];
    }

    printf("%s\n", output);

    return 0;
}