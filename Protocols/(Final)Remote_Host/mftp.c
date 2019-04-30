/**
 * Assignment: Final project
 * Class: CS 360
 * Coded by: Tam Nguyen
 * Finished: 4/18/19 ~6hrs
 * Description: Client app for connecting to the server. Available commands are show,get,put,ls,rls,cd, and rcd
 */
#include "mftp.h"

#define max_serv_len 50
void separateInput(char *input, char *cmdBuf, char *pathBuf); // separate and store in buffers
int parseCmd(char *cmd, char *path, int connectionFD);
int requestDataCMD (char cmd, int connectionFD, int *dataConFD, char *servResp, char *path); // send commands that requires data conn
int getAndProcessResponse(int connectionFD, char *servResp, int *dataConFD); // process if we got A/E and what to do with A
int connectToDataCon (int port); // establish and connection to a data connection
void execLS(); // perform ls
void readToMore(int dataFd); // send the message from server to 'more' cmd
int readToFile(int dataFd, char *fileName);
int hasAccessToFile(char *path); // perform two type checks to see if file exist

char *server_host; // So we dont have to keep passing it to print once

/* ----------------------------- Global Function ---------------------------- */

void separateInput(char *input, char *cmdBuf, char *pathBuf) // separate and store in buffers
{
    char *toks = strtok(input, " "); // sep by space
    int i = 0;
    while( toks != NULL ) 
    {
        if (!i)
            strncpy(cmdBuf, toks, 5);
        else if (i == 1)
            strncpy(pathBuf, toks, 300);
        else
            break; // don't crash if there are more than 2 arugments (according to Ben's client)
        i++;
        toks = strtok(NULL, " ");
    }
    return;
}

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
    str[strlen(str)-1] = '\n';
}

int hasAccessToFile(char *path)
{
    char absolute[200] = {'\0'}; // paths need to end with '\0'
    realpath(path, absolute);
    absolute[strlen(absolute)-1] = '\0';
    if (debugMode) printf("File: %s\n",absolute);
    if (access(absolute, R_OK) == 0) // check if file exist
        return 1;
    captureError(0, errno,"Access() check failed");
    int tmpFD = open(absolute,O_RDONLY); // check if we can read it.
    if (tmpFD == -1)
    {
        captureError(0, errno,"open() check failed");
        return 0;
    }
    close(tmpFD);
    return 1;
}

/* ---------------------------------- Main ---------------------------------- */

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Error: Please enter one argument (Host's name/IP).");
        exit (EXIT_FAILURE);
    }
    if (argc == 3 && strcmp(argv[1], "-d\n") != 0) // check for debug flag
    {
        debugMode = 1;
        initConnection(argv[2]);
    }
    else if (argc == 2)
        initConnection(argv[1]);
    else
    {
        perror("Error: To enter flag, enter it as second argument: prgm <-d> host");
        return 1;
    }
    
    return 1;
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

    if (debugMode) printf("\nDebug> Incoming from socket: %s\n", buf);
    return i; // how many was read
}

 void initConnection(char *host)
 {
    int socketfd = socket( AF_INET, SOCK_STREAM, 0); // makes internet socket using tcp protocal
    //                      IPv4    2-way conn  TCP
    struct sockaddr_in servAddr;
    struct hostent* hostEntry;
    struct in_addr **pptr;
    memset( &servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);

    hostEntry = gethostbyname(host);
    if (hostEntry == NULL) {
        perror("Error, no such host\n");
        exit (EXIT_FAILURE);
    }
    
    /* test for error using herror() */
    /* this is magic, unless you want to dig into the man pages */
    pptr = (struct in_addr **) hostEntry->h_addr_list;
    memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr));

    // connect to the server
    if (connect(socketfd, (struct sockaddr *) &servAddr,sizeof(servAddr)) < 0) // acts like open()
    {
        perror("Error: Problem occured in connection");
        exit(EXIT_FAILURE);
    } 

    server_host = host;

    while (1)
    {
        char buff[512] = {'\0'};
        char cmd[5] = {'\0'}, path[300] = {'\0'};
        fprintf(stdout,"MFTP>");
        fgets(buff, 512, stdin);
        stripExtraSpaces(buff); // set all ws to space so strtok can work
        separateInput(buff, cmd, path);
        parseCmd(cmd, path, socketfd);
    }
 }

/* ------------------------------ Core Function ----------------------------- */

 int parseCmd(char *cmd, char *path, int connectionFD)
 {
    char servResp[max_serv_len] = {'\0'};
    int dataConFD;
    if (strcmp("exit\n", cmd) == 0)             // exit
    {
        if(debugMode) printf("Debug> Sent 'Q\\n'. Waiting for Response.\n");
        write(connectionFD, "Q\n", 2);
        if (!getAndProcessResponse(connectionFD, servResp, NULL))
            printf("MFTP> Forcing abort from servert.\n");
        close(connectionFD);
        waitpid(-2, NULL, EXIT_SUCCESS); // wait on all child pid (<-1)
        exit(EXIT_SUCCESS);
        
    }
    else if (strcmp("cd\0", cmd) == 0)          // local cd
        changePath(path);

    else if (strcmp("rcd\0", cmd) == 0)         // server cd
    {
        write(connectionFD, "C", 1); write(connectionFD, path, strlen(path)); // hopefully it includes path
        if(debugMode) printf("Debug> Sent 'C\\n'\n");
        if (getAndProcessResponse(connectionFD, servResp, NULL) && debugMode)
            printf("Debug> Got A from server\n");
    }
    else if (strcmp("ls\n", cmd) == 0)          // local ls
        execLS();

    else if (strcmp("rls\n", cmd) == 0)         // server ls
    {
        if (requestDataCMD('L', connectionFD, &dataConFD, servResp, path))
            readToMore(dataConFD);
        close(dataConFD);
    }
    else if (strcmp("get\0", cmd) == 0)         // get (copy from server)
    {
        if (requestDataCMD('G', connectionFD, &dataConFD, servResp, path))
            readToFile(dataConFD,path);
        close(dataConFD);
    }
    else if (strcmp("show\0", cmd) == 0)       // show (cat server file)
    {
        if (requestDataCMD('G', connectionFD, &dataConFD, servResp, path))
            readToMore(dataConFD);
        close(dataConFD);
    }
    else if (strcmp("put\0", cmd) == 0)        // put (copy to server)
    {
        DIR *tmp;
        if ((tmp = opendir(path)) && readdir(tmp)->d_type == 4) // check if path is a directory
            fprintf(stderr, "Error> path given must be a file; not a directory.\n");
        else if (hasAccessToFile(path))
        {
            closedir(tmp);
            int pathStartIndx = 0;
            for (int i = 0, len = strlen(path); i < len; i++) // get us only the file name
            {
                if (path[i] == '/' && i != len-1)
                    pathStartIndx = i;
            }
            if (path[pathStartIndx] == '\\')
                pathStartIndx++;
            if (requestDataCMD('P', connectionFD, &dataConFD, servResp, path+pathStartIndx))
                trasnferToConnection(dataConFD, path);
            close(dataConFD);
        }
    }
    else
    {
        fprintf(stderr, "Error: invalid command (%s).\n", cmd);
        return -1;
    }
     
 }

int requestDataCMD (char cmd, int connectionFD, int *dataConFD, char *servResp, char *path)
 { // commands that requires data connection
    if (debugMode)
        printf("Key: %c Value: %s\n", cmd, path);
    write(connectionFD, "D\n", 2); // request data connection
    if(debugMode) printf("Debug> Sent 'D\\n'\n");

    getAndProcessResponse(connectionFD, servResp, dataConFD);

    if (cmd == 'L')
        write(connectionFD, "L\n", 2); // no path needed
    else // everything else needs a path
        write(connectionFD, &cmd, 1); write(connectionFD, path, strlen(path)); // hopefully it includes path
    if(debugMode) printf("Debug> Sent '%c\\n'\n", cmd);    
    getAndProcessResponse(connectionFD, servResp, dataConFD);
    if (dataConFD < 0)
        return 0;
    return 1;
 }


int getAndProcessResponse(int connectionFD, char *servResp, int *dataConFD)
{
    if (recvFromFd(connectionFD,servResp, max_serv_len) <= 0)
    {
        fprintf(stderr, "Error: Socket connection problem occured. Aborting.\n");
        exit(EXIT_FAILURE);
    }
    switch (servResp[0])
    {
        case 'E':
            fprintf(stderr, "Error from server: %s\n", servResp+1);
            if (debugMode) printf("Debug> Server Msg: %s\n", servResp);
            return 0;
            break;
        case 'A':
            ; // idk why it forces me to add this
            int dataPort;
            if (servResp[1] == '\0') // check if it's just A
            {
                break;
            }
            else if ((dataPort = atoi(servResp+1)) >  1000) // check to see if we can get port
            {
                *dataConFD = connectToDataCon(dataPort); 
                break;
            }
            else // server aint following protocols
            {
                fprintf(stderr, "Error: Can not convert %s to int to get port #.\n", servResp+1);
                return 0;
            }
            
            break;
        default:
            fprintf(stderr,"Error from Server: Recieved invalid response from server (%s).\n", servResp);
            return 0;
    }
    return 1;
    
}

int connectToDataCon (int port)
{
    if (debugMode) printf("Debug> new port %d.\n", port);
    int datafd = socket( AF_INET, SOCK_STREAM, 0); // makes internet socket using tcp protocal
    //                      IPv4    2-way conn  TCP
    if (datafd < 0)
    {
        captureError(0, errno,"Error: creating data socket.\n");
        return -1;
    }
    struct sockaddr_in servAddr;
    struct hostent* hostEntry;
    struct in_addr **pptr;
    memset( &servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);

    hostEntry = gethostbyname(server_host);
    if (hostEntry == NULL) {
        captureError(0, h_errno,"no such host\n");
        exit (EXIT_FAILURE);
    }
    /* test for error using herror() */
    /* this is magic, unless you want to dig into the man pages */
    pptr = (struct in_addr **) hostEntry->h_addr_list;
    memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr));

    // connect to the server
    if (connect(datafd, (struct sockaddr *) &servAddr,sizeof(servAddr)) < 0) // acts like open()
    {
        //err
        captureError(0, errno, "Problem occured in connection\n");
        return -1;
    }
    return datafd;
}

/* ------------------------------ Cmd Function ------------------------------ */

 void execLS()
 {
     // TO DO: add show
    int childPID = 0;
    if ((childPID = fork()) == 0)
    {
        int fd[2]; // [0] is read, [1] is w
        int tmpPID;
        if (pipe(fd) == -1) // the reason why we're piping it is
        {                   // bc of the 'more' cmd requirement
            captureError(0, errno, "can't make pipe.\n");
            exit(EXIT_FAILURE);
        }
        if ((tmpPID=fork() )== 0)
        {
            close(fd[0]); // close read since we're writing
            dup2(fd[1], STDOUT_FILENO);
            execl("/bin/ls", "ls", "-la", (char *)0); // perform ls -la
            close(fd[1]); // I don't think this'll be touched
        }
        wait(&tmpPID);
        close(fd[1]); // close write
        dup2(fd[0], 0);
        execl("/bin/more", "more", "-20", (char *)0);
        close(fd[0]);
    }
        
    wait(&childPID); // the child pid remains after exec
    return;
 }

int changePath(char *path)
{
    path[strlen(path)-1] = '\0';
    if (chdir(path)) { // If cant enter path
        captureError(0, errno, "Invalid Path");
        return 0;
    }
    return 1;
}


void readToMore(int dataFd)
{
    int childPID = 0;
    if ((childPID = fork()) == 0)
    { // child
        dup2(dataFd, 0);
        execl("/bin/more", "more", "-20", (char *)0);
    }
    wait(&childPID);
    return;
}


int readToFile(int dataFd, char *fileName)
{
    int pathStartIndx = 0;
    for (int i = 0, len = strlen(fileName); i < len; i++) // get us only the file name
    {
        if (fileName[i] == '/' && i != len-1)
            pathStartIndx = i;
    }
    char *outputName = fileName+pathStartIndx;
    outputName[strlen(outputName)-1] = '\0';
    if (access(outputName, F_OK) != -1) // check if file exist
    {
        fprintf(stderr, "Error: File exists.\n");
        close(dataFd);
        return 0;
    }

    if(debugMode) printf("Debug> Saving to %s\n", outputName);

    int fileDest = open(outputName, O_WRONLY| O_CREAT, 0777); // create file if doesnt exist adn write only
    if (fileDest == -1)
    {
        captureError(0, errno,"Can't Create file");
        close(dataFd);
        return -1;
    }

    unsigned char c[512];
    int rCount = 0;
    while ((rCount = read(dataFd, &c, 512)) > 0)
    {
        write(fileDest, &c, rCount);
    }

    close(fileDest);
    return 1;
}

int trasnferToConnection(int connectionFD, char *path)
{
    path[strlen(path)-1] = '\0'; // paths need to end with '\0'
    int childPID = 0;
    if ((childPID = fork()) == 0)
    {
        dup2(connectionFD, 1);
        execl("/bin/cat", "cat", path, (char *)0); // perform ls -l
    }
    wait(&childPID); // the child pid remains after exec
    return 1;
}
