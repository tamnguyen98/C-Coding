#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <time.h>

// http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

#define MY_PORT_NUMBER 49999

// server
int main() {
    // make soocket
    int listenFD = socket(AF_INET, SOCK_STREAM, 0); // af is the domain. sock is tcp
    if (listenFD < 0) {//err
        perror ("socket");
        exit (EXIT_FAILURE);
    }
    /* Bind socket to a port */
    struct sockaddr_in servAddr;
    socklen_t servSize = sizeof(servAddr);
    memset(&servAddr, 0, servSize);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( bind( listenFD, (struct sockaddr *) &servAddr, servSize) < 0) {
        perror("bind failed");
        exit (EXIT_FAILURE);
    }
    /* Listen and accept connection */
    if (listen(listenFD, 1)) { // set a connection one level deep
        // error
        perror("Problem during Listen");
    }
    socklen_t length = sizeof(struct sockaddr_in);
    struct sockaddr_in clientAddr;

    
    int hasExited = 0;
    while (!hasExited) {
        // accepts return new fd, and all coms should be used on this fd
        int connectfd = accept(listenFD, // waits for a connection to be establish by client
                (struct sockaddr *) &clientAddr, //copy the client address to client addr
                &length);
        if (connectfd < 0) {
            // err
            perror("Error occur upon connection");
            exit(EXIT_FAILURE);
        }

        int childPID = fork();

        if (childPID == 0) {
            // child
            struct hostent* hostEntry = gethostbyaddr(&(clientAddr.sin_addr), // get host data info
                                    sizeof(struct in_addr), AF_INET);

            time_t thisTime;
            time(&thisTime);
            char *dateNTime = ctime(&thisTime); // get the date and time;
            write(0, hostEntry->h_name, strlen(hostEntry->h_name));
            write(0, " Connected\n", strlen(" Connected\n"));
            write(connectfd, dateNTime, strlen(dateNTime));
            close(connectfd); // close the connection
            exit(EXIT_SUCCESS);
        } else {
            waitpid(-1, NULL, EXIT_SUCCESS); // wait on all child pid
            close(connectfd); // close it agai to make sure
        }
    }
    
}
