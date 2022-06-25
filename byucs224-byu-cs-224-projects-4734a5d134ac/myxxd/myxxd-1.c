//Submitted by Devin Sheldon Jan 23 2022
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define BAD_NUMBER_ARGS 1



/**
 * Parses the command line.
 *
 * argc: the number of items on the command line (and length of the
 *       argv array) including the executable
 * argv: the array of arguments as strings (char* array)
 * bits: the integer value is set to TRUE if bits output indicated
 *       outherwise FALSE for hex output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *bits) {
  if (argc > 2) {
    printf("Usage: %s [-b|-bits]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }

  if (argc == 2 &&
      (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "-bits") == 0)) {
    *bits = TRUE;
  } else {
    *bits = FALSE;
  }

  return stdin;
}

/**
 * Writes data to stdout in hexadecimal.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsHex(unsigned char *data, size_t size) {
	// Offset is taken care of for us
	// Print 1 byte at a time
	// Remember: we're reading in 1 char at a time, 
	// every char is representing in hexadeicmal as 2 #s
  for(int i = 0; i < size; i++){
    printf(" "); // one space between every pair of hex outputs "_AABB_CCDD"
    printf("%02x", data[i]);
    if(!((i+1)>size)){
      printf("%02x", data[i+1]);//FIXED!!
      i++;
    }
  }
  //padding
  if(size<16){
    for(int i = 16-size; i > 0; i--){
      if(i%2==1){
        printf("  "); //two spaces
      }else{
        printf(" "); // one space
        printf("  "); //two spaces
      }
    }
  }
}

/**
 * Writes data to stdout as characters.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsChars(unsigned char *data, size_t size) {
  for(int i = 0; i < size; i++){
    if((data[i]>=32)&&(data[i]<=126)){
      printf("%c", data[i]);
    }else{
      printf(".");
    }
  }
}

void readAndPrintInputAsHex(FILE *input) {
  unsigned char data[16];
  int numBytesRead = fread(data, 1, 16, input);
  unsigned int offset = 0;
  while (numBytesRead != 0) {
    printf("%08x:", offset);
    offset += numBytesRead;
    printDataAsHex(data, numBytesRead);
    printf("  ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, 16, input);
  }
}

void printDataAsBinary(unsigned char *data, size_t size) {
  char byte[8] = {0,0,0,0,0,0,0,0};
  unsigned char temp = 0;

  for(int x = 0; x < size; x++){ //for all 6 chars in data[]
    temp = data[x];
    printf(" ");
    //fills byte[] with binary (representation of the char at data[x])
    for(int i = 7; i >= 0; i--){ //for every char in byte[]
      if (temp % 2 == 1) {
        byte[i] = '1'; //store a 1 for the bit
      } else {
        byte[i] = '0'; //store a 0 for the bit
      }
      temp = temp / 2;
    }  
    //print the whole byte (and reset it for next byte)
    for(int i =0;i<8;i++){
      printf("%c", byte[i]);
      byte[i] = 0; //scrubs byte[] clean for the next char in data[]
    }
  }
  //padding
  if(size<6){
    for(int i = 0; i < 6-size; i++){
      printf(" "); // one space
      printf("        "); //eight spaces
    }
  }
}

void readAndPrintInputAsBits(FILE *input) {
	unsigned char data[6];
  int numBytesRead = fread(data, 1, 6, input);
  unsigned int offset = 0;
  while (numBytesRead != 0) {
    printf("%08x:", offset);
    offset += numBytesRead;
    printDataAsBinary(data, numBytesRead); 
    printf("  ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, 6, input);
  }
}

int main(int argc, char **argv) {
  int bits = FALSE;
  FILE *input = parseCommandLine(argc, argv, &bits);

  if (bits == FALSE) {
    readAndPrintInputAsHex(input);
  } else {
    readAndPrintInputAsBits(input);
  }
  return 0;
}