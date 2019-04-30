#include <arpa/inet.h>
#include <dirent.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#define MY_PORT_NUMBER 49999
int debugMode = 0;

void captureError(int pid, int errnum, char *extraInfo);
void stripExtraSpaces(char* str);
void initConnection();
int recvFromFd(int connectionFd, char *buf, int size);
int changePath(char *path);
int hasAccessToFile(char *path);
int trasnferToConnection(int fdOutput, char *path);
