#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]){
	srand(time(NULL));												//reset random numbers
	if(argc != 2){														//error checking
		fprintf(stderr, "\nERROR\n");						//print error to STDERR
		exit(1);
	}
	char * allowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";		//string of allowed characters
	int i = 0;
	int keyLength = atoi(argv[1]);						//get key length from user argument
	char key[keyLength+1];										//create key
	for ( i = 0; i < keyLength; i++)
	{																					//create key with random letters
		key[i] = allowedCharacters[rand() % 27];
	}
	key[keyLength] = '\n';										//add new line
	key[keyLength+1] = '\0';									//add null terminator 
	fprintf(stdout, "%s", key);								//print to results STDOUT		
	return 0;
}