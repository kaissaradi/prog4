#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
//function to convert message into cipher to be sent to server
void createCipher(char * plainText, char * keyText, char * cipherText){
 printf("\nCIPHER: %ld\n\nPLAINTEXT: %ld\n\nKEY: %ld\n\n", strlen(cipherText), strlen(plainText), strlen(keyText)); 
  int i = 0;
  if (strlen(plainText) > strlen(keyText))
  {
    error("ERROR Key Too Short");
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
    if (textArr[i] < 0 || textArr[i] > 26 || keyArr[i] < 0 || keyArr[i] > 26) { error("ERROR Bad Character"); }

    //create cipher array
    cipherArr[i] = textArr[i] + keyArr[i];
    cipherArr[i] %= 27;
    
    if (cipherArr[i] == 26) { cipherText[i] = 32; }    //check if plaintext contains a space or convert char to numeric representation
    else { cipherText[i] = cipherArr[i] + 65; }
  }
  printf("\nCIPHER: %s\n\nPLAINTEXT: %s\n\nKEY: %s\n\n", cipherText, plainText, keyText); 
  printf("\nCIPHER: %ld\n\nPLAINTEXT: %ld\n\nKEY: %ld\n\n", strlen(cipherText), strlen(plainText), strlen(keyText)); 
}


int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	int getBool, postBool, i = 0;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	int bufferSize = 200000;
	char buffer[bufferSize];
	//check arguments for error and request method
	if (argc < 5) { fprintf(stderr,"ERROR: Too Few Arguments\n"); exit(0); } 
	if (argc > 6) { fprintf(stderr,"ERROR: Too Many Arguments\n"); exit(0); } 
	if (strcmp(argv[1],"post") == 0){
		portNumber = atoi(argv[5]); // Get the port number, convert to an integer from a string
		postBool = 1;
	} 
	else if (strcmp(argv[1],"get") == 0){
		portNumber = atoi(argv[4]);
		getBool = 1;
	} 
	else { error("ERROR: Invalid First Argument"); }
	
	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	char user[4098];
	strcpy(user, argv[2]);
	// make POST request
	if (postBool)
	{
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
		//post method
		//get plaintext file into a string
		FILE * file = fopen(argv[3], "r");
		if(file == NULL){ error("ERROR opening file"); }
		char plaintText[bufferSize];
		char character;
		i = 0;
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
		if(file == NULL){ error("ERROR opening file"); exit(1); }
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
		sprintf(buffer, "%s %s", user, cipherText);
	}
	//make POST request
	if(getBool){ 
		sprintf(buffer, "%s", user); 
	}
	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	if (getBool)
	{
		// Get return message from server
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0) error("CLIENT: ERROR reading from socket");
		printf("%s", buffer);
	}
	
	close(socketFD); // Close the socket
	return 0;
}
