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
//"ABCDEFILG";
//"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789";

static bool done = false;

// typedef struct Node *Node;

// struct Node {
//     char *data;
//     struct Node* next;
// };

void compute_md5(char *str, unsigned char digest[16]) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
}

// // To Enqueue an integer
// void Enqueue(char *x, Node front, Node rear, int *queue_size) {
//     Node temp = 
//         (Node)malloc(sizeof(struct Node));
//     temp->data = x; 
//     temp->next = NULL;
//     if (*queue_size == 0) {
//         //front = rear = temp;
//         front = temp;
//         printf("enqueue front: %s\n", front->data);
//         *queue_size += 1;
//         return;
//     }
//     rear->next = temp;
//     rear = temp;

//     *queue_size += 1;
// }

// // To Dequeue an integer.
// void Dequeue(char *x, Node front, Node rear, int *queue_size) {
//     Node temp = front;
//     if(*queue_size == 0) {
//         printf("Queue is Empty\n");
//         return;
//     }
//     if(*queue_size == 1) {
//         front = rear;
//     }
//     else {
//         front = front->next;
//     }
//     x = temp->data;
//     //x temp->data;
//     // temp = temp->next;
//     // free(first);
//     // first = temp;

//     //free(temp);

//     *queue_size -= 1;
// }


// char *BFSbruteForce(char *th_ab) {
//     int queue_size = 0;
//     Node front = NULL;
//     Node rear = NULL;

//     unsigned char digest[17];
//     int tid = omp_get_thread_num();

//     //char *queue[200000];
//     //int queue_size = strlen(th_ab);

//     int i;

//     for (i=0; i<strlen(th_ab); i++) {
//         char *temp = malloc( sizeof(char) );
//         sprintf(temp, "%c", th_ab[i]);
//         // printf("%s\n", temp);

//         Node t = (Node)malloc(sizeof(struct Node));
//         t->data = temp; 
//         t->next = NULL;
//         if (queue_size == 0) {
//             front = rear = t;
//             //front = t;
//             // printf("enqueue front: %s\n", front->data);
//             queue_size += 1;
//         } else {
//             rear->next = t;
//             rear = t;

//             queue_size += 1;
//         }

//         //Enqueue(temp, front, rear, &queue_size);
//         //queue[i] = malloc(sizeof(char));
//         //sprintf(queue[i], "%c", th_ab[i]);
//     }

//     for(i=0; i<strlen(th_ab); i++) {

//         char *temp = malloc( sizeof(char) );
//         sprintf(temp, "%c", th_ab[i]);

//         compute_md5(word, digest);

//         // printf("thread %d 57\n", tid);
//         if (strcmp((char *) hash, (char *) digest) == 0) {
//             //printf("%d %d\n", hash, digest);
//             printf("word found in thread %d: %s\n", tid, word);
//             done = true;
//             return word;
//         }

//     }

//     // printf("front: %s\n", front->data);
//     // printf("%d\n", queue_size);
//     char *word = malloc( 9 * sizeof(char) );
//     char *prev_word = malloc( 9 * sizeof(char) );
//     i = 0;
//     while (queue_size > 0 && !done) {
//         //Dequeue(word, front, rear, &queue_size);

//         Node t = front;
//         if(queue_size == 0) {
//             printf("Queue is Empty\n");
//             break;
//         }
//         if(queue_size == 1) {
//             front = rear;
//         }
//         else {
//             front = front->next;
//         }
//         word = t->data;

//         queue_size -= 1;

//         if (prev_word[0] != word[0]) {
//             int len = strlen(word);
//             printf("Thread %d is testing words of length %d that start with %c\n", tid, len, word[0]);
//         }

//         // printf("thread %d 54\n", tid);
//         compute_md5(word, digest);

//         // printf("thread %d 57\n", tid);
//         if (strcmp((char *) hash, (char *) digest) == 0) {
//             //printf("%d %d\n", hash, digest);
//             printf("word found in thread %d: %s\n", tid, word);
//             done = true;
//             return word;
//         }

//         // printf("thread %d 65\n", tid);
//         if (strlen(word) < MAX_LEN) {
//             int j;
//             for (j=0; j<strlen(alphabet); j++) {
//                 char *temp = malloc( (strlen(word)+1) * sizeof(char) );
//                 sprintf(temp, "%s%c", word, alphabet[j]);
                
//                 Node t = (Node)malloc(sizeof(struct Node));
//                 t->data = temp; 
//                 t->next = NULL;
//                 if (queue_size == 0) {
//                     front = rear = t;
//                     //front = t;
//                     // printf("enqueue front: %s\n", front->data);
//                     queue_size += 1;
//                 } else {
//                     rear->next = t;
//                     rear = t;

//                     queue_size += 1;
//                 }
//                 //Enqueue(temp, front, rear, &queue_size);
                

//                 // printf("thread %d 69\n", tid);
//                 //queue[queue_size+j] = malloc( 9 * sizeof(char) );
//                 //sprintf(queue[queue_size+j], "%s%c", word, alphabet[j]);
//             }
//             //queue_size += j;
//         }
//         //printf("queue_size: %d 74\n", queue_size);
//         i++;
//         prev_word = word;
//         free(t);
//     }

//     return NULL;
// } 

char *bruteForce(char *word, int index, int max_len) {

    unsigned char digest[17];
    int tid = omp_get_thread_num();
    int i;
    for (i=0; i<strlen(alphabet); i++) {
        word[index] = alphabet[i];
        word[index+1] = '\0';

        // if (tid == 0) {
        //     printf("word in thread %d: %s\n", tid, word);
        // }

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

        // if (strcmp((char *) hash, (char *) digest) == 0) {
        //     //printf("%d %d\n", hash, digest);
        //     printf("word found in thread %d: %s\n", tid, word);
        //     done = true;
        //     return word;
        // }

        // if ((index < max_len-1) && !done) {
        //     char *res = bruteForce(word, index+1, max_len);
        //     if (res != NULL) {
        //         return res;
        //     }
        // }
        
    }

    // printf("word not found in thread %d: %s\n", tid, word);
    return NULL;
}

int main(int argc, char *argv[]) {
    // char output[21];

    char *thread_alphabet[8];

    int i;
    // thread_alphabet[0] = "AIQYgow4";
    // thread_alphabet[1] = "AHOVcjqx4";
    // thread_alphabet[2] = "BIPWdkry5";
    // thread_alphabet[3] = "CJQXelsz6";
    // thread_alphabet[4] = "DKRYfmt07";
    // thread_alphabet[5] = "ELSZgnu18";
    // thread_alphabet[6] = "FMTahov29";
    // thread_alphabet[7] = "GNUbipw3";

    // thread_alphabet[0] = "AIQYgow4";
    // thread_alphabet[1] = "BJRZhpx5";
    // thread_alphabet[2] = "CKSaiqy6";
    // thread_alphabet[3] = "DLTbjrz7";
    // thread_alphabet[4] = "EMUcks08";
    // thread_alphabet[5] = "FNVdlt19";
    // thread_alphabet[6] = "GOWemu2";
    // thread_alphabet[7] = "HPXfnv3";

    thread_alphabet[0] = "ABCDEFGH";
    thread_alphabet[1] = "IJKLMNOP";
    thread_alphabet[2] = "QRSTUVWX";
    thread_alphabet[3] = "YZabcdef";
    thread_alphabet[4] = "ghiklmno";
    thread_alphabet[5] = "pqrstuvw";
    thread_alphabet[6] = "xyz01234";
    thread_alphabet[7] = "56789";

    char *words[strlen(alphabet)];

    for (i=0; i<strlen(alphabet); i++) {
        words[i] = malloc(9 * sizeof(char));
        
        // printf("%c\n", alphabet[i]);        
        
        words[i][0] = alphabet[i];
        words[i][1] = '\0';

        // printf("%s\n", words[i]);
    }
    //char *word = malloc(9 * sizeof(char));
    

    // BFSbruteForce(alphabet);

    #pragma omp parallel
    {
        #pragma omp single

        // for (i=0; i<strlen(alphabet); i++) {

        // }

        for (i=0; i<8; i++) {

            //char *word = words[i];
            #pragma omp task
            {
                int tid = omp_get_thread_num();
                // BFSbruteForce(thread_alphabet[tid]);
                // int block = strlen(alphabet)/8;
                // printf("block size: %d\n", block);
                int k;
                for (k=1; (k<=MAX_LEN) && !done; k++) {
                    printf("thread %d is testing words of length %d\n", tid, k);

                    int j;
                    for (j=0; j<strlen(thread_alphabet[tid]) && !done; j++) {
                        char *word = malloc(9 * sizeof(char));
                        sprintf(word, "%c", thread_alphabet[tid][j]);
                        bruteForce(word, 1, k);                        
                    }
                    // for (j = tid*block; j<((tid+1)*block) && j<strlen(alphabet) && !done; j++) {
                    //     char *word = words[j];
                    //     printf("thread %d is responsible for letter %s\n", tid, word);

                    //     bruteForce(word, 1, k);
                        
                    // }
                }
            }
        }
        #pragma omp taskwait
    }

    // bruteForce(word, 0, 8);

    // printf("%s\n", word);

    // char input[20] = {0x0b, 0x34, 0x30, 0x20, 0x2f, 0x27, 0x05, 0x22, 0x05, 0x29, 0x21, 0x28, 0x22, 0x26, 0x19, 0x34, 0x23, 0x22, 0x27, 0x2d};

    // int len = strlen(word);

    // for (int i=0; i<20; i++) {
    //     output[i] = input[i] ^ word[i % len];
    // }

    // output[20] = '\0';

    // printf("%s\n", output);

    return 0;
}

