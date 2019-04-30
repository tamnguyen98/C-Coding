#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/* CS 360 Assignment 2
*  By: Tam Nguyen
*  School: WSU-V
*  Overall: 
*  Write a program (in C) targeted at the Linux platform which checks whether a specified word, 
*  passed as an argument to the executable, exists in a dictionary or not. The specified dictionary is webster and on the website.
*
*  Requirements:
*  You may only use the i/o methods covered in class to interface with the file. 
*  This includes lseek, read, write, open, and close. However, feel free to use other libraries for debugging (such as printf) or for string comparison (such as strcmp).
*/


#define filePath "webster"
#define wordLen 16 // Very important to set this value to the correct length!
// The length is determine by the dictionary (short words are padded by space up to wordLen)

extern int errno; // For error outputs by some functions

int ok(int fd, char *word);
void formatArg(char *word, char *argv);

int main (int argc, char **argv) {
    if (argc == 1)  {// if they didnt gie us a word to look up
        fprintf(stderr,"Please give a word to look up. (0)\nCommand: ./ok <word>\n");
        return -1;
    }
    int file = open(filePath, O_RDONLY);
    if (file != -1) {
        // file exist
        char word[wordLen];
        formatArg(word, argv[1]);
        ok(file, word);
    }
    else
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    close(file);
    return 0;
}

// Main search function for the HW using binary search
int ok(int fd, char *word) {
    int bot = 0;
    int top = (lseek(fd, 0, SEEK_END)/wordLen)+1; // Get how many lines in the dictionary
    while  (top > bot) {
        int mid = ((bot+top)/2);
        char dictWord [wordLen]; // Buffer for the dictionary

        if (lseek(fd, mid*wordLen, SEEK_SET) == -1) 
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        if (read(fd, dictWord, wordLen-1) == -1) 
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));

        int cmpVal = strcmp(dictWord, word);
        if (cmpVal == 0) {
            printf("yes\n");
            return 1;
        }
        else if (cmpVal > 0) // If word is smaller than dictWord
            top = mid;
        else if (cmpVal < 0)
            bot = mid+1;
    }
    printf("no");
    return 0;
}

// Get the argument word to the correct dictionary format.
// Addtionally, function will truncate words to wordLen
void formatArg(char *word, char *argv) {
    for (int i = 0; i < wordLen; i++) {
        if (i < strlen(argv))
            word[i] = argv[i]|32; // Convert to lower case
        else if (i == wordLen-1)
            word[i] = '\0';
        else
            word[i] = ' ';
    }
}
