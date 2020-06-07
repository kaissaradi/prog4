#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

//function that parses the request into an array of the user, cipher, and length of cipher
//returns 0 if the client made a GET request, returns 1 if the client made a POST request
int parseRequest(char * msg, char ** msgArray){
  msgArray[0] = strtok(msg, " ");   //store user name
  char * key = strtok(NULL, "\0");
  msgArray[1] = key;                //store message
  int i, count = 0;
  if (key == NULL) { return 0; }		//GET request

  for ( i = 0; key[i] != '\0'; i++) { count++; }	//count key length
  char charCount[100];
  sprintf(charCount, "%d", count);
  msgArray[2] = charCount;         //POST request
  return 1;
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, i, status;
	socklen_t sizeOfClientInfo;
	int bufferSize = 200000;
	char buffer[bufferSize];
	char cipherMessage[bufferSize];
	struct sockaddr_in serverAddress, clientAddress;
  pid_t pid;

	//error checking for arguments
	if (argc < 2) { fprintf(stderr,"ERROR:: Too Few Arguments\n"); exit(1); }
	if (argc == 3 && (strcmp(argv[2], "&") == 0)) { fprintf(stderr,"ERROR:: Invalid Argument: %s\n", argv[2]); exit(1); }
	if (argc > 3) { fprintf(stderr,"ERROR:: Too Many Arguments\n"); exit(1); }

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
  while(1){
    fflush(stdin);
    fflush(stdout);
    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
    if (establishedConnectionFD < 0) error("ERROR on accept");
    pid = fork();
    if(pid < 0) { error("ERROR forking"); }
    if(pid == 0){
      sleep(2);
      // Get the message from the client and parse it
      memset(buffer, '\0', bufferSize);
      charsRead = recv(establishedConnectionFD, buffer, (bufferSize-1), 0); // Read the client's message from the socket
      if (charsRead < 0) error("ERROR reading from socket");
      char * msgArr[3] = {NULL};			//create an array of strings to hold the message from the buffer
      int req = parseRequest(buffer,msgArr);				//parse buffer request
      char path[1080];
      sprintf(path, "./%s/", msgArr[0]);							//path to user folder
      if (req == 0){																	//check for get request
        /************************************************************************
         * GET request handler
       * ***********************************************************************/
        DIR * dir;
        FILE * file;
        char oldestCiper[1080];
        struct dirent * entry;
        struct stat stats;
        time_t oldestFile = INT_MAX;
        dir = opendir(path);
        if(!dir) { error("ERROR user not found"); }		//check if  requested user has any files
        while(entry = readdir(dir)){
          stat(entry->d_name, &stats);
          if((entry->d_name[0] == 'c')){
            //if this is the most recent directory, record its name
            if (stats.st_mtime < oldestFile) {
              sprintf(oldestCiper, "%s", entry->d_name);
              oldestFile = stats.st_mtime;
            }
          }
        }																							//check if user has any files
        if(oldestFile == INT_MAX){ error("ERROR user has no file"); }
        strcat(path, oldestCiper);
        file = fopen(path, "r");
        if (file == NULL) { ("ERROR opening file"); }
        char character;
        i = 0;
        do
        {
          character = (char)fgetc(file);
          cipherMessage[i] = character;
          i++;
        } while (character != EOF);
        fclose(file);
        closedir(dir);
        // Send a Success message back to the client
        charsRead = send(establishedConnectionFD, cipherMessage, bufferSize, 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");
        fflush(stdin);
        fflush(stdout);
      }
      else
      {
        /************************************************************************
         * POST request handler
       * ***********************************************************************/
        //store message in a file in the users directory
        mkdir(path, 0777);								//create a directory for the user
        strcat(path, "cipherText");
        FILE *file = fopen(path, "w+");		//create cipherText file in user directory
        if (file == NULL) { ("ERROR opening file"); }
        int msgLength = atoi(msgArr[2]);
        char parsedMsg[msgLength+1];
        strcpy(parsedMsg, msgArr[1]);
        strcat(parsedMsg, "\n\0");
        fprintf(file, "%s", parsedMsg);		//print message in cipherText file
        fclose(file);

        //print path to STDOUT
        char cwd[4097];
        getcwd(cwd, sizeof(cwd));
        strcat(cwd, "/");
        strcat(cwd, msgArr[0]);
        strcat(cwd, "/");
        fprintf(stdout, "%s", cwd);
        // Send a Success message back to the client
        charsRead = send(establishedConnectionFD, "\n\0", 2, 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");
        fflush(stdin);
        fflush(stdout);
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
      }
    }
  }
  close(listenSocketFD); // Close the listening socket
	return 0;
}
