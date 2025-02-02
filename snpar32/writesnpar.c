/***************************************************************\
 *
 *              Copyright (c) SVFI Automation automation systems
 *
 * Program:     writesnpar
 * File:        writesnpar.c
 * Author :     Georges Sancosme <georges@sancosme.net>
 * Functions:   main
 *
 * Description: This code takes three command line parameters: an integer for glSN and a 10-byte string for gucAddr, and writes them to the file passed as parameter
 * 
 * To write to snpar.par:
 * 
 * Example usage
 * Command to write data:
 * 
 * ./writesnpar <filename> <12345> <ABCDEFGHIJ>
 * 
 * 12345: The value for glSN.
 * ABCDEFGHIJ: The 10-character ASCII string for gucAddr.
 * 
 * Contents of <filename> after execution:
 * 
 * The binary value of glSN (e.g., 12345).
 * The 10 ASCII characters (A through J), each stored as a byte.
 * 
 * Points to note:
 * Each ASCII character in gucAddr is transformed into its raw value (e.g., 'A' becomes 0x41, 'B' becomes 0x42, etc.).
 * This behavior is useful if you want to write ASCII characters to the file while storing them in binary hexadecimal form.
 * 
 * v0.3 Changes made:
 * 
 * Added a parameter for the file:
 * The file path is passed as the first argument on the command line.
 * 
 * Reading and displaying after writing:
 * The file is reopened after writing to check its contents.
 * The values ​​of glSN and gucAddr are displayed again, in hexadecimal and ASCII.
 * 
 * Improved ASCII display:
 * Non-printable characters are replaced by dots (.).
 * 
 * Example of use
 * Command to write and display the contents:
 * 
 * ./writesnpar file.snpar 12345 ABCDEFGHIJ
 * 
 * Output (for example):
 * 
 * The data has been written to the file 'file.snpar' successfully.
 * Contents of file 'file.snpar':
 * glSN: 12345
 * gucAddr (hex): 41 42 43 44 45 46 47 48 49 4a
 * gucAddr (ASCII): ABCDEFGHIJ
 * 
 * Points to note:
 * 
 * This program ensures that written data is immediately verified by reading it back.
 * The user can specify a custom file path.
 * The format of the displays makes the output readable and suitable for different types of data.
 * 
 * Environment: SUSE LINUX C - (Tumbleweed)
 *
 * Modification history:
 *
 * Rev      Date        Brief Description
 * 0.1      20250118    Initial version
 * 0.2      20250118    Added glSN and gucAddr
 * 0.3      20250118    Added parameter for file to output, reading and displaying after writing to check content
 *
\***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    FILE *ofd;
    long glSN;
    unsigned char gucAddr[10];

    // Checking the number of arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <file_path> <glSN> <gucAddr (10 hex characters)>\n", argv[0]);
        return 1;
    }

    // Retrieve file path
    char *filename = argv[1];

    // Converting the second argument to long
    glSN = atol(argv[2]);

    // Validation and conversion of the third argument (ASCII version)
//    if (strlen(argv[3]) != 10) { // gucAddr must contain exactly 10 ASCII characters
//        fprintf(stderr, "gucAddr must contain exactly 10 ASCII characters\n");
//        return 1;
//    }
//
    //  Convert each ASCII character to its hexadecimal value (1 byte per character)
//    for (int i = 0; i < 10; i++) {
//        gucAddr[i] = (unsigned char)argv[3][i];
//    }

    // Validation and conversion of the third argument (Hex version)
    if (strlen(argv[3]) != 20) { // Each byte must be represented by 2 hexadecimal characters
        fprintf(stderr, "gucAddr must be exactly 10 bytes (20 hex characters)\n");
        return 1;
    }
    for (int i = 0; i < 10; i++) {
        char hexByte[3] = { argv[3][i * 2], argv[3][i * 2 + 1], '\0' };
        gucAddr[i] = (unsigned char)strtol(hexByte, NULL, 16);
    }

    // Open file in binary write mode
    ofd = fopen(filename, "wb");
    if (ofd == NULL) {
        perror("Error opening file for writing");
        return 1;
    }

    // Write glSN
    if (fwrite(&glSN, sizeof(long), 1, ofd) != 1) {
        fprintf(stderr, "Error writing glSN\n");
        fclose(ofd);
        return 1;
    }

    // Write gucAddr
    if (fwrite(gucAddr, sizeof(unsigned char), 10, ofd) != 10) {
        fprintf(stderr, "Error writing gucAddr\n");
        fclose(ofd);
        return 1;
    }

    fclose(ofd);

    printf("Data was written to file '%s' successfully.\n", filename);

    // Reading and displaying the contents of the generated file
    FILE *ifd;
    long read_glSN;
    unsigned char read_gucAddr[10];
    size_t iTot;

    // Open file in binary reading mode
    ifd = fopen(filename, "rb");
    if (ifd == NULL) {
        perror("Error opening file for reading");
        return 1;
    }

    // Read glSN
    iTot = fread(&read_glSN, sizeof(long), 1, ifd);
    if (iTot != 1) {
        fprintf(stderr, "Error reading glSN\n");
        fclose(ifd);
        return 1;
    }

    // read gucAddr
    iTot = fread(read_gucAddr, sizeof(unsigned char), 10, ifd);
    if (iTot != 10) {
        fprintf(stderr, "Error reading gucAddr\n");
        fclose(ifd);
        return 1;
    }

    fclose(ifd);

    // Show read values
    printf("Contenu du fichier '%s':\n", filename);
    printf("glSN: %ld\n", read_glSN);

    // Displaying gucAddr in hexadecimal
    printf("gucAddr (hex): ");
    for (int i = 0; i < 10; i++) {
        printf("%02x ", read_gucAddr[i]);
    }
    printf("\n");

    // Displaying gucAddr in ASCII
    printf("gucAddr (ASCII): ");
    for (int i = 0; i < 10; i++) {
        if (isprint(read_gucAddr[i])) {
            putchar(read_gucAddr[i]);
        } else {
            putchar('.');
        }
    }
    printf("\n");

    return 0;
}
