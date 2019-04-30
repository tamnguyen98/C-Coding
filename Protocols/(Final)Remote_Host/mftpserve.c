/**
 * Assignment: Final project
 * Class: CS 360
 * Coded by: Tam Nguyen
 * Finished: 4/17/19 12 AM (~4 days)
 * Description: Server client for mftp. Acts similar to ssh.
 */
#include "mftp.h"

// http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html resource on sockets

struct connectionParams getNewConnection(int port); // to be able to return two values
void forkConnection(int origFD, struct sockaddr_in origAddr, socklen_t origLen); // create the data connection
int openDataConnection(int connectionFd); // open the data connection port and wait till client enters
int getPath (char *msg, int connectionFD, int dataConFD, char *fileName, char *path);
int processMsg(char *msg, int dataConFD, int connectionFD);
void execLS(int fdOutput, int connectionFD);
int readToFile(int dataFd, int connectionFD, char *path);


struct connectionParams { // to store both fd and port 
    int connectionFD;
    int port;
};

/* ---------------------------- Global functions ---------------------------- */

void captureError(int pid, int errnum, char *extraInfo)
{
    if (pid == 0)
    {
        if (extraInfo == NULL )
            fprintf (stderr,"Error (%d): %s\n", errnum, strerror(errnum));
        else
            fprintf (stderr,"Error (%d): %s (%s)\n", errnum, strerror(errnum), extraInfo);
    }
    else
    {
        if (extraInfo == NULL )
            fprintf (stderr,"Child %d: Error (%d): %s\n", pid, errnum, strerror(errnum));
        else
            fprintf (stderr,"Child %d: Error (%d): %s (%s)\n", pid, errnum, strerror(errnum), extraInfo);
    }
    
    
}

void stripExtraSpaces(char* str) 
{
    int i, x;
    for(i=x=0; str[i]; ++i)
        if(!isspace(str[i]) || (i > 0 && !isspace(str[i-1])))
        {
            str[i] = (str[i] == '\t')? ' ':str[i]; // we only want spaces
            str[x++] = str[i];
        }
    
    str[x] = '\0';
}

int hasAccessToFile(char *path)
{
    char absolute[200] = {'\0'}; // paths need to end with '\0'
    realpath(path, absolute);
    if (debugMode) printf("Path: %s\n", absolute);
    if (access(absolute, R_OK) == 0) // check if file exist
        return 1;
    captureError(getpid(), errno,"Access() check failed");
    int tmpFD = open(absolute,O_RDONLY); // check if we can read it.
    if (tmpFD == -1)
    {
        captureError(getpid(), errno,"open() check failed");
        return 0;
    }
    close(tmpFD);
    return 1;
}

/* ---------------------------------- Main ---------------------------------- */

int main(int argc, char **argv) 
{
    if (argc == 2)
        debugMode = strcmp(argv[1], "-d\n");
    initConnection();
    
}

int recvFromFd(int connectionFd, char *buf, int size)
{
    char c;
    int i = 0;
    for (read(connectionFd, &c, 1); i < size && c != '\n'; i++)
    {
        buf[i] = c;
        read(connectionFd, &c, 1);
    }
    buf[i] = '\0';
    stripExtraSpaces(buf);
    if (debugMode) printf("\n>Incoming from socket: %s (len: %d)\n", buf, (int)strlen(buf));
    return strlen(buf)+1;
    //return i; // how many was read
}

void initConnection () 
{
    // make soocket
    struct connectionParams root = getNewConnection(MY_PORT_NUMBER);
    int listenFD = root.connectionFD;
    
    /* Listen and accept connection */
    if (listen(listenFD, 4)) 
        captureError (getpid(), errno, "Error on Listening");

    socklen_t length = sizeof(struct sockaddr_in);
    struct sockaddr_in clientAddr;
    int connectionEstablished = 0; // how many are connected

    while (1) // constantly running
    {
        // accepts return new fd, and all coms should be used on this fd
        int connectfd = accept(listenFD, // waits for a connection to be establish by client
                (struct sockaddr *) &clientAddr, //copy the client address to client addr
                &length);
        if (connectfd < 0) // if client can't connect
            captureError (getpid(), errno, "Occur on connection");
        else
            forkConnection(connectfd, clientAddr, length);
    }
    waitpid(-2, NULL, EXIT_SUCCESS); // wait on all child pid (<-1). never gonna happen
}

/* ---------------------------------- Core Function ---------------------------------- */

void forkConnection(int origFD, struct sockaddr_in origAddr, socklen_t origLen) 
{
    if (debugMode) printf("Debug> Root: New client connection detected.\n");
    int childPID = 0;
    if ((childPID=fork()) == 0) {
        int dataConFD = -1;
        char msg[100] = ""; // buffer for the message sent by client
        int rcvCount = 0; // byte count of message read;
        if (debugMode) printf("Debug> Child created with PID: %d\n", getpid());
        struct hostent* hostEntry = gethostbyaddr(&(origAddr.sin_addr), // get host data info
                            origLen, AF_INET);
        socklen_t length = sizeof(struct sockaddr_in);
        struct sockaddr_in clientAddr;

        printf("Child %d: %s connected\n", getpid(), inet_ntoa(origAddr.sin_addr)); //The hostname may be either a symbolic host name or IPv4 dotted-decimal notation
        while ((rcvCount = recvFromFd(origFD,msg, 100)) > 1) // keep reading any messages sent by client in original connection port
        {
            if (debugMode) printf( "Debug> Child %d Received: %s\n", getpid(), msg);
            if (msg[0] == 'D') // client requested data connection
            {
                // upon D, get a new port available and tell the client to switch to that port.
                
                struct connectionParams dataConnection = getNewConnection(0); // make new data connection
                
                if (listen(dataConnection.connectionFD, sizeof(struct sockaddr_in))) // return 0 on success
                {
                    captureError (getpid(), errno, "Occur on Listen");
                    close(origFD);
                    exit(EXIT_FAILURE);
                }

                char ack[10] = {'\n'};
                sprintf(ack, "A%d\n", dataConnection.port); // making the acknol message

                write(origFD, ack, strlen(ack));
                if (debugMode) printf("Debug> Child %d: Sent %s", getpid(), ack);
                
                if (debugMode)
                    printf("Debug> Child %d:Data connection accepted\n", getpid());
                dataConFD = openDataConnection(dataConnection.connectionFD); // Make a data connection
            }
            else if (msg[0] == 'Q') // request to quit
            {
                printf("Child %d: Client disconnected\n", getpid());
                write(origFD, "A\n", 2);
                if (debugMode) printf("Debug> Child %d: Sent 'A'\n", getpid());

                if (dataConFD > 0)
                    close(dataConFD);
                close(origFD);
                exit(EXIT_SUCCESS);
            }
            else  // some specific commands
            {
                if ((msg[0] == 'G' || msg[0] == 'P' || msg[0] == 'L')// if we get a command that requires Data con
                    && dataConFD < 3) // but we dont have Data con open,
                {
                    write(origFD, "EError: Client did not request for data connection.\n", 52);
                }
                else
                    processMsg(msg, dataConFD, origFD);
                
                if (dataConFD > 2)
                {
                    if (debugMode)
                        printf("Debug> Child %d: closing data connection\n", getpid());
                    close(dataConFD);
                    dataConFD = -1;
                }
                
            }
        }
        fprintf(stderr, "Connection port closed unexpectedly\n");
        close(origFD);
        exit(EXIT_FAILURE);
    }
}

struct connectionParams getNewConnection(int port) // first point in "suggestions"
{
    int listenFD = socket(AF_INET, SOCK_STREAM, 0); // af is the domain. sock is tcp
    if (listenFD < 0) 
    {//err
        captureError (getpid(), errno, "Create socket");
        exit (EXIT_FAILURE);
    }
    
    /* Bind socket to a port */
    struct sockaddr_in targetAddr; // this is only for IPV$ AF_INET, for both compatibility we need to look at sockaddr_storage
    socklen_t servSize = sizeof(targetAddr); 
    memset(&targetAddr, 0, servSize);
    targetAddr.sin_family = AF_INET; // connect in IPV4
    targetAddr.sin_port = htons((port > 1023)? port:0); // see if a port is given (that's not reserved) else find a empty port
    targetAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( bind( listenFD, (struct sockaddr *) &targetAddr, servSize) < 0)  // attempt to bind the socket
    {
        captureError (getpid(), errno, "Bind");
        exit (EXIT_FAILURE);
    }

    if (getsockname(listenFD, (struct sockaddr*) &targetAddr, &servSize) == -1) // attempt to get connection info
    {
        captureError (getpid(), errno, "Get Socket Info");
        exit(EXIT_FAILURE);
    }

    struct connectionParams result = {listenFD, ntohs(targetAddr.sin_port)};
    return result;
}


int openDataConnection(int connectionFd) // open a new data connection
{
    socklen_t length = sizeof(struct sockaddr_in);
    struct sockaddr_in clientAddr;

    int connectfd = accept(connectionFd, // waits for a connection to be establish by client
            (struct sockaddr *) &clientAddr, //copy the client address to client addr
            &length);
    if (connectfd < 0) 
    {
        // err
        captureError(getpid(), errno, "Data Connection");
        return -1;
    }
    if (debugMode)
        printf("Debug> Child %d: Data connection completed\n", getpid());
    return connectfd;
}

/* -------------------------------- Commands -------------------------------- */

// Grab path from client's messaage (help shorten next function)
int getPath (char *msg, int connectionFD, int dataConFD, char *fileName, char *path)
{
    int captureLen = 0;
    if (msg[0] == 'C'||msg[0] == 'G' || msg[0] == 'P')  // these commands requires paths
    {
        // we dont need to worry about no path given because client handles that
        if (msg[1] == '\n') // error catching when [Ben's] client send the path and cmd separately
        {
            if ((captureLen = recvFromFd(connectionFD, fileName, 100)) == -1)
            {
                captureError(getpid(), errno, NULL);
                return 0;
            }
        }
        else if (msg[1] != '\n') // how it should be (i.g G<Path>)
            strcpy(fileName, msg+1);
        
        if (msg[0] != 'P') // Put doesn't need a full path.
        {
            DIR *tmp;
            if ((tmp = opendir(fileName)))
            {
                // time to check if it's a file or a dir
                if (msg[0] ==  'G' && readdir(tmp)->d_type == 4) // see if client wants use to get/show a path
                {                                               // but the path is a directory
                    write(connectionFD,"EError: path is a directory.\n", 29);   // if that's the case send E
                    if (debugMode) printf("Debug> Error: path is a directory.\n");
                    closedir(tmp);
                    return 0;
                }
                else // if we're not recieving G then we're treating it as a file being imported.
                {    // dir check must be done on client side
                    closedir(tmp);
                    if (access(fileName, F_OK) == 0)// if the user gave a valid absolute file path
                        strcpy(path,fileName);
                }
            }
            else
            { // append to current path
                realpath(fileName, path);
                if (debugMode)
                    printf("Debug> Got Path: %s\n", path);
            }
        }
    }
    return 1;
}

int processMsg(char *msg, int dataConFD , int connectionFD)
{
    char path[199] = {'\n'};
    char fileName[100] = {'\n'};

    if (msg == NULL) return -1; // empty string
    
    if (!getPath(msg, connectionFD, dataConFD, fileName, path)) // process the path
        return -1;

    switch (msg[0]) // all following send to data conn
    {
        case 'L': // ls -l
            execLS(dataConFD, connectionFD);
            break;
        // include Paths
        case 'C': // cd 
            if (changePath(path))
            {
                write(connectionFD, "A\n", 2);  
                if (debugMode) printf("Debug> Child %d: Sent 'A'\n", getpid());
            }
            else
            {
                write(connectionFD, "EInvalid Path\n", 14);
                return -2;
            }
            break;
        case 'G': // copy to client and also for show
            if (hasAccessToFile(path))
            {
                write(connectionFD, "A\n", 2); // proceed on
                if (debugMode) printf("Debug> Child %d: Sent 'A'\n", getpid());
                trasnferToConnection(dataConFD, path);
            }
            else
                write(connectionFD, "EError: File doesn't exist.\n", 28);
            break;
        case 'P': // copy to server
            readToFile(dataConFD, connectionFD, fileName);
            break;
        default:
            fprintf(stderr,"Error: recieve invaild command.\n");
            return -2;
            break;
    }
    return 1;
}

int changePath( char *path)
{
    if (chdir(path) == 0) { // If it's not null (path exist)
        printf("Child %d: Changed current dir to %s\n",getpid(), path);
    } else {
        captureError(getpid(), errno, "Invalid Path");
        return 0;
    }
    return 1;
}

void execLS(int dataConFd, int connectionFD)
{
    int childPID = 0;
    if ((childPID = fork()) == 0)
    {
        dup2(dataConFd, 1); // replace stdout with data connection
        execl("/bin/ls", "ls", "-la", (char *)0); // perform ls -la
    }
    waitpid(childPID, NULL, 0); // the child pid remains after exec
    write(connectionFD, "A\n", 2); // let client know we're done
    if (debugMode) printf("Debug> Child %d: Sent 'A'\n", getpid());
    return;
}

int trasnferToConnection(int dataConFd, char *path)
{
    if (debugMode) printf("Child %d: Sending %s content to client.\n", getpid(), path);
    int childPID = 0;
    if ((childPID = fork()) == 0)
    {
        dup2(dataConFd, 1);
        execl("/bin/cat", "cat", path, (char *)0); // perform ls -l
    }
    waitpid(childPID, NULL, 0); // the child pid remains after exec
    return 1;
}


int readToFile(int dataFd, int connectionFD , char *path)
{
    if (path[0] == '\0') // check if it's empty
        return -1;

    int pathStartIndx = 0;
    for (int i = 0, len = strlen(path); i < len; i++) // get us only the file name
    {
        if (path[i] == '/' && i != len-1)
            pathStartIndx = i;
    }

    if (access(path+pathStartIndx, F_OK) != -1) // check if file exist
    {
        fprintf(stderr, "Child %d: Error: File exists.\n", getpid());
        write(connectionFD, "EError: File exists in server.\n", 31);
        return 0;
    }

    int fileDest = open(path+pathStartIndx, O_WRONLY| O_CREAT, 0777); // create file if doesnt exist adn write only
    if (fileDest == -1)
    {
        captureError(getpid(), errno, "");
        write(connectionFD, "EError occured when trying to open/create file.\n", 41);
        return -1;
    }

    unsigned char c[512];
    int rCount = 0;
    write(connectionFD, "A\n", 2); // let client know we're ready
    if (debugMode) printf("Debug> Child %d: Sent 'A'\n", getpid());
    while ((rCount = read(dataFd, &c, 512)) > 0)
    {
        write(fileDest, &c, rCount);
    }

    close(fileDest);
    printf("Child %d: Client sent %s to store.\n", getpid(), path+pathStartIndx);
    if (debugMode)
        printf("Debug> Child %d: Finished reading\n", getpid());
    return 1;
}
