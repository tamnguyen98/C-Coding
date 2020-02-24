#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int hex2dec (char *hex);
int isValidHex (char *str);

int main (int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Run with the follow:\t./ProgramName <address> <1B machineCode>  [optional: file]\n");
        return 1;
    }
    char *fileName = (argc == 4)? argv[3]:"./lab04";
    if (access(fileName, R_OK | W_OK)) {
        // Return 0 if success, so if fails to have access
        fprintf(stderr, "Error: Can't seem to access \"%s\", make sure the file exist and you have R/W permission for it.\n", fileName);
        return 1;
    }

    // check which format user input
    if (!isValidHex(argv[1])) {
        fprintf(stderr, "Error: Invalid address. Example of valid: 0x560 or 560\n");
        return 1;
    }
    else if (!isValidHex(argv[2])) {
        fprintf(stderr, "Error: Invalid machine code. Please enter hex-value machine code.\n\tExample: 75 or 0x75\n");
        return 1;
    } 
    
    FILE *file = fopen(fileName, "r+");
    int offset = hex2dec(argv[1]);
    char byte = (char)hex2dec(argv[2]);

    fseek(file, offset, SEEK_SET);
    printf("Writing %X to %X\n", byte, argv[1]);
    fwrite(&byte, 1, 1, file);
    // fread(&byte, 1,1, file);
    // printf("%d:%02X\n", offset, byte);
    fclose(file);
    return 0;
}

int hex2dec (char *hex) {
    return (int)strtol(hex, NULL, (strlen(hex) > 2 && hex[1] == 'x')?
                                    0 : 16);
}

int isValidHex (char *str) {
    int len = strlen(str);

    // Check if x**** or *x***. Valid is 0x***
    if (len > 2 && (tolower(str[0]) == 'x' ||  (str[0] != '0' && str[1] == 'x')))
        return 0;

    for (int i = (len > 2)? 2:0; i < len; i++) {
        if (!isxdigit(str[i]))
            return 0;
    }
    return 1;
}