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

int main(int argc, char * argv[]){
  char message[] = "arslan";
  char * msgArr[3] = {NULL};
  int req = parseMsg(message, msgArr);
	char path[1080];
	sprintf(path, "./%s/", msgArr[0]);							//path to user folder
  //if(!req){
  /************************************************************************
   * GET request handler
 * ***********************************************************************/
/*     DIR * dir;
    FILE * file;
    char oldestCiper[1080];
    struct dirent * entry;
    struct stat stats;
    time_t oldestFile = INT_MAX;
    dir = opendir(path);
    if(dir == NULL) { printf("ERROR user not found");
    exit(1); }		//check if  requested user has any files
    while((entry = readdir(dir))){
      stat(entry->d_name, &stats);
      if((entry->d_name[0] == 'c')){
        //if this is the most recent directory, record its name
        printf("found cipher text\n\n");
        if (stats.st_mtime < oldestFile) {
          printf("found oldest file: %s\n\n", entry->d_name);
          sprintf(oldestCiper, "%s", entry->d_name); 
          oldestFile = stats.st_mtime;
        }
      }
    }																							//check if user has any files
    if(oldestFile == INT_MAX){ printf("ERROR user has no file"); }
    strcat(path, oldestCiper);
    //file = fopen(path, "r");
    closedir(dir);
    return 0;
  }
	//store message in a file in the users directory
	sprintf(path, "%s", msgArr[0]);
  mkdir(path, 0777);								//create a directory for the user
  strcat(path, "/cipherText");
  FILE *file = fopen(path, "w+");		//create cipherText file in user directory
	if (file == NULL)
	{
		printf("\n\nFile Error\n\n");
	}
  int msgLength = atoi(msgArr[2]);	
  char parsedMsg[msgLength+1];
  strcpy(parsedMsg, msgArr[1]);
  strcat(parsedMsg, "\n\0");
  fprintf(file, "%s", parsedMsg);		//print message in cipherText file
	 */
	//print path to STDOUT
  char cwd[4097];
  getcwd(cwd, sizeof(cwd));
	strcat(cwd, "/");
	strcat(cwd, &path[2]);
	fprintf(stdout, "%s", cwd);
  return 0;
}