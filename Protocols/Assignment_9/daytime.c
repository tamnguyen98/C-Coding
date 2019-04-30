// takes one arg: server's ip/name
// tcp connect to host 
// print info recieved from server to stdout
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <time.h>


#define MY_PORT_NUMBER 49999

int isValidIP(char *src) {
    struct sockaddr_in addy;
    return inet_pton(AF_INET, src, &addy);
}

// client

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("Please enter one argument (Host's name/IP).");
        exit (EXIT_FAILURE);
    }

    int socketfd = socket( AF_INET, SOCK_STREAM, 0); // makes internet socket using tcp protocal
    //                      IPv4    2-way conn  TCP
    struct sockaddr_in servAddr;
    struct hostent* hostEntry;
    struct in_addr **pptr;
    memset( &servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);

    hostEntry = gethostbyname(argv[1]);
    if (hostEntry == NULL) {
        perror("ERROR, no such host\n");
        exit (EXIT_FAILURE);
    }
    
    /* test for error using herror() */
    /* this is magic, unless you want to dig into the man pages */
    pptr = (struct in_addr **) hostEntry->h_addr_list;
    memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr));

    // connect to the server
    //connect(socketfd, (struct sockaddr *) &servAddr,sizeof(servAddr)); // acts like open()
    if (connect(socketfd, (struct sockaddr *) &servAddr,sizeof(servAddr)) < 0) {
        //err
        perror("Error: Problem occured in connection");
        exit(EXIT_FAILURE);
    } 

    // read data from server
    char t[100] = {0};
    int x;
    if ((x = read(socketfd, t, 100)) > 0) {
        write(1, t, x-1);
    }
    write(1, "\n", 1);

    
    //use read/write to send info
 }