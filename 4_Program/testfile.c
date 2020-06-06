#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h> 
#include <dirent.h>
#include <limits.h>

//function that parses the message into an array of the user, cipher, and length of cipher
int parseMsg(char * msg, char ** msgArray){
  msgArray[0] = strtok(msg, " ");   //store user name
  char * key = strtok(NULL, "\0");  
  msgArray[1] = key;                //store message
  if (key == NULL)
  {
    return 0;
  }
  int i, count = 0;
  for ( i = 0; key[i] != '\0'; i++)
  {
    count++;
  }
  char charCount[100];
  sprintf(charCount, "%d", count);
  msgArray[2] = charCount;         //store number of characters in message
  return 1;
}

void createCipher(char * plainText, char * keyText, char * cipherText){
 printf("\nCIPHER: %ld\n\nPLAINTEXT: %ld\n\nKEY: %ld\n\n", strlen(cipherText), strlen(plainText), strlen(keyText)); 
  int i = 0;
  if (strlen(plainText) > strlen(keyText))
  {
    printf("ERROR Key Too Short");
  }
  int textArr[strlen(plainText)];
  int cipherArr[strlen(cipherText)];
  int keyArr[strlen(keyText)];
  for ( i = 0; i < strlen(plainText); i++)
  {                                                 //convert text arrays to number and create cipher
    if (plainText[i] == 32) { textArr[i] = 26; }    //check if plaintext contains a space or convert char to numeric representation
    else { textArr[i] = plainText[i] - 65; }
    if (keyText[i] == 32) { keyArr[i] = 26; }       //if keytext contains a space or convert char
    else { keyArr[i] = keyText[i] - 65; }
                                                    //error checking
    if (textArr[i] < 0 || textArr[i] > 26 || keyArr[i] < 0 || keyArr[i] > 26) { printf("ERROR Bad Character"); }

    //create cipher array
    cipherArr[i] = textArr[i] + keyArr[i];
    cipherArr[i] %= 27;
    
    if (cipherArr[i] == 26) { cipherText[i] = 32; }    //check if plaintext contains a space or convert char to numeric representation
    else { cipherText[i] = cipherArr[i] + 65; }
  }
  printf("\nCIPHER: %s\n\nPLAINTEXT: %s\n\nKEY: %s\n\n", cipherText, plainText, keyText); 
  printf("\nCIPHER: %ld\n\nPLAINTEXT: %ld\n\nKEY: %ld\n\n", strlen(cipherText), strlen(plainText), strlen(keyText)); 
}


int main(int argc, char * argv[]){
  int bufferSize = 200000;
  int i = 0;
  char user[4098];
  strcpy(user, argv[2]);
  //post method
  //get plaintext file into a string
  FILE * file = fopen(argv[3], "r");
  if(file == NULL){ printf("ERROR opening file"); }
  char plaintText[bufferSize];
  char character;
  do
  {
    character = (char)fgetc(file);
    plaintText[i] = character;
    i++;
  } while (character != EOF);
	plaintText[strcspn(plaintText, "\n")] = '\0'; // Remove the trailing \n that fgets adds
  fclose(file);
  //get keyText into a string
  file = fopen(argv[4], "r");
  if(file == NULL){ printf("ERROR opening file"); exit(1); }
  char keyText[bufferSize];
  i = 0;
  do
  {
    character = (char)fgetc(file);
    keyText[i] = character;
    i++;
  } while (character != EOF);
	keyText[strcspn(keyText, "\n")] = '\0'; // Remove the trailing \n that fgets adds
  fclose(file);
  char cipherText[bufferSize];
  createCipher(plaintText, keyText, cipherText);
  printf("\nCIPHER:%s\n", cipherText);
  return 0;
}