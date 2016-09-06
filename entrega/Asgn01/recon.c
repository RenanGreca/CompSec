#include <stdio.h>
// Standard library (atoi, malloc)
#include <stdlib.h>
// String functionalities
#include <string.h>
// For the timeout functionality
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
// read and close socket
#include <unistd.h>
// To detect types of errors
#include <errno.h>
// For the timestamp
#include <time.h>

#define IP_LENGTH 15
#define BUFFER_SIZE 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void show_usage() {
    printf("Correct usage: recon <ip> [port]\n");
    printf("ip: IP address or range in format X.X.X.X-X\n");
    printf("port: Port or range in format Y-Y (if empty all will be used)\n");
}

int main(int argc, char *argv[]) {

    // Example run: ./ipSweep 192.168.1.1-10 1-1023

    // Prepare variables for TCP connection

    // Check number of argumetns
    int opt;
    if ((argc < 2) || (argc > 3)) {
        show_usage();
        exit(1);
    }

    time_t raw_time = time(NULL);

    printf("Varredura iniciada em %s\n", ctime(&raw_time));
    printf("IP: %s\n", argv[1]);
    printf("Portas: %s\n", argv[2]);
    printf("---\n");

    char *ipstring = argv[1]; // IP
    char *token;

    char *iprange[2];

    // Separate range of IP addresses
    int i = 0;
    while ((token = strsep(&ipstring, "-")) != NULL) {
        iprange[i] = token;
        i++;
    }
    // iprange[0] contains the first IP address
    // iprange[1] contains the last part of the last IP address

    // Isolate subnet from IP
    i = 0;
    char *subnet = malloc(100*sizeof(char));
    subnet[0] = '\0';
    while (((token = strsep(&iprange[0], ".")) != NULL) && i<3 ) {        
        strcat(subnet, token);
        strcat(subnet, ".");
        i++;
    }
    
    // Get int values for min and max IP
    int ip_min = atoi(token);
    int ip_max;
    if (iprange[1] != NULL) {
        ip_max = atoi(iprange[1]);
    }
    if ( ip_max < ip_min ) {
        // No IP range, just one
        ip_max = ip_min;
    }
    // If no port is defined, check all(?) of them
    int port_min = 1;
    int port_max = 65535;

    if (argc == 3) {
        char *portstring = argv[2]; // Port
        char *portrange[2];

        // Separate ports by '-'
        i = 0;
        while ((token = strsep(&portstring, "-")) != NULL) {
            portrange[i] = token;
            i++;
        }

        port_min = atoi(portrange[0]);
        if (portrange[1] != NULL) {
            port_max = atoi(portrange[1]);
        }
        if ( port_max < port_min ) {
            // Just one port
            port_max = port_min;
        }
    }

    // ip holds the complete IP address, ips is just the final number
    char *ip = malloc(IP_LENGTH*sizeof(char));
    char *ips = malloc(3*sizeof(char));
    for (int ipaddr=ip_min; ipaddr<=ip_max; ipaddr++) {
        
        strcpy(ip, subnet);

        sprintf(ips, "%d", ipaddr);
        strncat(ip, ips,
                 sizeof(ip) - strlen(ip) - 1);

        for (int port=port_min; port<=port_max; port++) {
            // At this point, ip and port are complete

            // Prepare socket shenanigans
            int sockfd, portno, n;
            char buffer[BUFFER_SIZE];
            struct sockaddr_in serv_addr;
            struct hostent *server;

            sockfd = socket(AF_INET, SOCK_STREAM, 0);

            server = gethostbyname(ip);
            if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                continue;
            }

            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, 
                 (char *)&serv_addr.sin_addr.s_addr,
                 server->h_length);
            serv_addr.sin_port = htons(port);
        
            if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
                close(sockfd);

                if (errno == 51) { 
                    // Error 51 means network unreachable, IP inactive
                    port = port_max;
                } // Other errors can be port inactive or operation timeout
                continue;
                
            }

            // Use poll.h to avoid long timeout
            struct pollfd fd;
            int ret;

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
                    }
                    break;
            }

            close(sockfd);
        }

    }

}