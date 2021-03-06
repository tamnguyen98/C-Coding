#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int hex2dec (char *hex);
int isValidHex (char *str);

int main (int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Run with the follow:\t./ProgramName <address> <machineCode>  <file>\n");
        return 1;
    }
    char *fileName = "./lab04";
    if (argc == 3)
        printf("No file given, resorting to %s as default you can set a file name by:\n\t./ProgramName <address> <1B machineCode>  <file>\n", fileName);
    else
        fileName = argv[3];

    if (access(fileName, R_OK | W_OK)) {
        // Return 0 if success, so if fails to have access
        fprintf(stderr, "Error: Can't seem to access \"%s\", make sure the file exist and you have R/W permission for it.\n", fileName);
        return 1;
    }

    if (strlen(argv[2])%2 != 0) {
        fprintf(stderr, "Error: Invalid machine code. There are two character per byte (length need to be even).\n");
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
    fseek(file, offset, SEEK_SET);

    // Write all the bytes (Assuming more than one byte is given)
    for (int i = 1; i < strlen(argv[2]); i+=2) {
        char code[2] = {argv[2][i-1], argv[2][i]};
        char byte = (char)hex2dec(code);
        printf("Writing %X to %X\n", byte, offset+i-1);
        fwrite(&byte, 1, 1, file);
    }
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