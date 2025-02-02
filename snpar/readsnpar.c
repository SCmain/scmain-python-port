/***************************************************************\
 *
 *              Copyright (c) SVFI Automation automation systems
 *
 * Program:     readsnpar
 * File:        readsnpar.c
 * Author :     Georges Sancosme <georges@sancosme.net>
 * Functions:   main
 *
 * Description: This code reads the values ​​of glSN and gucAddr from the binary file snpar.par and displays them.
 *  File passed as argument:
 *      The program expects a file path as a parameter, which allows to read any file containing data in the expected format.
 *      If the argument is missing, an error is displayed with the correct usage.
 *   
 *  Validation of file opening:
 *   
 *      Uses fopen(argv[1], "rb") to open the file passed as parameter.
 *
 *  Display structure:
 *      glSN is displayed as a long integer.
 *      gucAddr is displayed as hexadecimal values ​​(useful for inspecting binary data).
 *
 *  Example of use
 *      Command to read the file:
 *
 *  ./snread /root/controller/scmain/snpar.par
 *
 *  This program remains compatible with all files containing data in the expected format: a long trail of 10 bytes.
 *  If the file is malformed or does not contain enough data, appropriate error messages will be displayed.
 *
 *  This flexibility makes the program useful for reading different files containing structured data.
 *
 *  Printing gucAddr in hexadecimal:
 *      Each byte is printed using printf("%02x ").
 *
 *  Printing gucAddr in ASCII:
 *      Printable characters are printed directly.
 *      Non-printable characters (e.g., unreadable bytes) are replaced with a dot (.) to avoid erroneous characters in the console.
 *
 *  Using isprint:
 *      The isprint function checks whether a character is printable (such as readable letters, numbers, or symbols).
 *
 *  Output (for example):
 *
 *  If the file contains:
 *
 *      glSN = 12345
 *      gucAddr = "ABCDEFGHIJ"
 *
 *  The output will be:
 *
 *      glSN: 12345
 *      gucAddr (hex): 41 42 43 44 45 46 47 48 49 4a
 *      gucAddr (ASCII): ABCDEFGHIJ
 *
 *  If the file contains non-printable characters in gucAddr:
 *
 *      glSN: 12345
 *      gucAddr (hex): 41 42 43 00 45 46 47 48 49 1a
 *      gucAddr (ASCII): ABC.EFGHI. *
 *
 *   This program remains robust for different types of content in gucAddr.
 *   Non-readable characters are represented by dots, making it easier to read binary data.
 *
 * Environment: SUSE LINUX C - (Tumbleweed)
 *
 * Modification history:
 *
 * Rev      Date        Brief Description
 * 0.1      20250118    Initial version
 * 0.2      20250118    Added filename as argument
 * 0.3      20250118    Added output in human readable format for hex
 *
\***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    FILE *ifd;
    long glSN;
    unsigned char gucAddr[10];
    size_t iTot;

    // Checking the number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    // Open file in binary reading mode
    ifd = fopen(argv[1], "rb");
    if (ifd == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read glSN
    iTot = fread(&glSN, sizeof(long), 1, ifd);
    if (iTot != 1) {
        fprintf(stderr, "Error reading glSN\n");
        fclose(ifd);
        return 1;
    }

    // Read gucAddr
    iTot = fread(gucAddr, sizeof(unsigned char), 10, ifd);
    if (iTot != 10) {
        fprintf(stderr, "Error reading gucAddr\n");
        fclose(ifd);
        return 1;
    }

    fclose(ifd);

    // Display values
    printf("glSN: %ld\n", glSN);

    // Displaying gucAddr in hexadecimal
    printf("gucAddr (hex): ");
    for (int i = 0; i < 10; i++) {
        printf("%02x ", gucAddr[i]); // Display each byte in hexadecimal
    }
    printf("\n");

    // Displaying gucAddr as an ASCII string
    printf("gucAddr (ASCII): ");
    for (int i = 0; i < 10; i++) {
        if (isprint(gucAddr[i])) {
            putchar(gucAddr[i]); // Show printable characters
        } else {
            putchar('.'); // Replaces non-printable characters with a period
        }
    }
    printf("\n");

    return 0;
}
