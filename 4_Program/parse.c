
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void decodeCipher(char * plainText, char * keyText, char * cipherText){
  int i = 0;
  if (strlen(cipherText) > strlen(keyText))
  {
    printf("ERROR Key Too Short");
  }
  int textArr[strlen(cipherText)];
  int cipherArr[strlen(cipherText)];
  int keyArr[strlen(keyText)];
  int len = strlen(cipherText);
  for ( i = 0; i < len; i++)
  {                                                 //convert text arrays to number and create cipher
    if (cipherText[i] == 32) { cipherArr[i] = 26; }    //check if cipher contains a space or convert char to numeric representation
    else { cipherArr[i] = cipherText[i] - 65; }
    if (keyText[i] == 32) { keyArr[i] = 26; }       //if keytext contains a space or convert char
    else { keyArr[i] = keyText[i] - 65; }
                                                    //error checking
    if (cipherArr[i] < 0 || cipherArr[i] > 26 || keyArr[i] < 0 || keyArr[i] > 26) { printf("ERROR Bad Character"); }

    //create text array
    textArr[i] = cipherArr[i] - keyArr[i];
    textArr[i] %= 27;
		if(textArr[i] < 0) { textArr[i] += 27; }

    if (textArr[i] == 26) { plainText[i] = 32; }    //check if plaintext contains a space or convert char to numeric representation
    else { plainText[i] = textArr[i] + 65; }
  }
}

int main(int argc, char * argv[]){
    int bufferSize = 200000;
    char plaintText[bufferSize];
    char keyText[bufferSize];
    char cipherText[bufferSize];
    char character;
    char buffer[3000] = "GHSWIFMXLSFZGLSWC SSADCDTMBHMPFIPTTD";
		//get key file into a string
		FILE * file = fopen(argv[1], "r");
		if(file == NULL){ printf("ERROR opening file"); }
		int i = 0;
		do
		{
			character = (char)fgetc(file);
			keyText[i] = character;
			i++;
		} while (character != EOF);
		keyText[strcspn(keyText, "\n")] = '\0'; // Remove the trailing \n that fgets adds
		buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n 
		fclose(file);
		printf("received: %s\nkeyfile: %s\n\n", buffer, keyText);
		if(strcmp(buffer,"ERROR") == 0){
			printf("CLIENT: ERROR no message for user");
		}
		else { decodeCipher(plaintText, keyText, buffer); }
		fflush(stdout);
		fflush(stdin);
		fprintf(stdout, "%s", plaintText);
		fflush(stdout);
		fflush(stdin);
}