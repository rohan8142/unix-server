#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#define SIZE 1024

#define PORT 8081

char result[1024];
void processClient(int sd);
void findfile(int,char*);
void dgetfiles(char*, char*, int );
void getfiles(char** , int , int);
void gettargz(char** , int , int);
void concatenate_string(char* s, char* s1)
{   int i;
    int j = strlen(s);
    for (i = 0; s1[i] != '\0'; i++) 
        s[i + j] = s1[i];
    s[i + j] = '\0';
    return; }

int main(int argc, char *argv[]){
int sd, csd, portNumber;
socklen_t len;
struct sockaddr_in servAdd;

char a[3]="000";
int file_disc = open("./counter.txt",O_CREAT| O_RDWR, 0777);
int n=write(file_disc,a,3);
close(file_disc);

if ((sd=socket(AF_INET,SOCK_STREAM,0))<0){
fprintf(stderr, "Cannot create socket\n");
exit(1);
}
servAdd.sin_family = AF_INET;
servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
servAdd.sin_port = htons(PORT);

bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd));

listen(sd, 5);

while(1){
csd=accept(sd,(struct sockaddr*)NULL,NULL);
printf("S$:Got a client\n");
if(fork()==0)
processClient(csd);
close(csd);
}
}




void getfile( int sockfd, char* filename){ // reads backup.tar.gz file and sends it to the client

  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }

  int n;
  char buf[SIZE];
 
    while ((n = fread(buf, 1, SIZE, fp)) > 0) {
    printf("n:%d\n",n);
        if (send(sockfd, buf, n, 0) != n) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }
printf("$S:FILE SENT\n");

  
}

void findfile(int sockfd, char* filename){ // find file logic

  FILE *FileOpen;
  char line[1000];
  char command[100];  
  char data[SIZE] = {0};  
  char *error = "No file found";
    
  //filename[strlen(filename)-1] = '\0';          
 sprintf(command, "find . -name %s -print0 2>/dev/null | xargs -0 stat -c '%%s %%y'", filename);   //gets sizes from the client and runs the command. here find searches  the file and seds it to tar via a pipe               
  FileOpen = popen(command,"r");
  
  if (FileOpen == NULL) {
        printf("Error executing command\n");
    }
    if (fgets(line, sizeof(line), FileOpen) == NULL) {
            if (send(sockfd, error, sizeof(error), 0) == -1) {
        
      perror("[-]Error in error sending data.");
      exit(1);
      
    }
    } else {
       if (send(sockfd, line, sizeof(line), 0) == -1) {
      perror("[-]Error in valid sending data.");
      exit(1);
    }
    }
  
  
                                                                   
  
  printf("S$:DATA SENT TO CLIENT\n");
  pclose(FileOpen);
}


void sgetfiles(int size1, int size2, int socketfd){


FILE *FileOpen;
unlink("backup.tar.gz");
  char line[1000];
  char command[100];  
  char data[SIZE] = {0};  
  char *error = "No file found";  
  //filename[strlen(filename)-1] = '\0'; 
  printf("this is from sgetfiles\n");         
sprintf(command, "find -type f -size +%dc -size -%dc -print0 | tar -czvf backup.tar.gz --null -T -",size1,size2);    // gets dates from the client and runs the command. here find searches  the file and seds it to tar via a pipe          
  FileOpen = popen(command,"r");
  printf("file create\n");
  sleep(5);
  getfile(socketfd,"backup.tar.gz");
  

}

void getfiles(char *files[], int filecount, int socketfd){//we send the input from client and run the linux command to create the tar.z files

FILE *FileOpen;
char filenames[1024]; 
char command[1040];
unlink("backup.tar.gz");
int in;

//printf(" filenames string :%s\n", filenames);
 // printf("this is from getfiles\n");     
  //printf("in value :%s", files[filecount-1]);
  filecount = filecount -1;
  
  //printf("strcmp(): %d", strcmp(files[filecount -1], "-u"));
  
  // different linux command to run for number options entered
  //printf("filecount: %d\n", filecount);
  if(filecount == 1){
     sprintf(command, "find -type f -name %s -print0 | tar czvf backup.tar.gz --null -T -", files[1]);
     //printf("command: %s\n",command);
  }
  
  else if(filecount == 2){
       sprintf(command, "find  -type f \\( -name \"%s\" -o -name \"%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2]);
  }
    else if(filecount == 3){
   sprintf(command, "find  -type f \\( -name \"%s\" -o -name \"%s\" -o -name \"%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2], files[3]);
  }
    else if(filecount == 4){
     sprintf(command, "find  -type f \\( -name \"%s\" -o -name \"%s\" -o -name \"%s\" -o -name \"%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2], files[3], files[4]);
  }
    else if(filecount == 5){
      sprintf(command, "find  -type f \\( -name \"%s\" -o -name \"%s\" -o -name \"%s\" -o -name \"%s\" -o -name \"%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2], files[3], files[4], files[5]);
      }
    else if(filecount == 6){
   sprintf(command, "find  -type f \\( -name \"%s\" -o -name \"%s\" -o -name \"%s\" -o -name \"%s\" -o -name \"%s\" -o -name \"%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2], files[3], files[4], files[5], files[6]);
  }
  

    FILE *fp = popen(command, "r");
  printf("S$:FILE CREATED\n");
  sleep(5); 
  getfile(socketfd,"backup.tar.gz");

}

void gettargz(char *files[], int filecount, int socketfd){ // we send the input from client and run the linux command to create the tar.z files

FILE *FileOpen;
char filenames[1024]; 
char command[1040];
unlink("backup.tar.gz");
int in;

//printf(" filenames string :%s\n", filenames);
  //printf("this is from getfiles\n");     
  char *ex = "c";
 
    
  filecount = filecount -1;
  // different linux command to run for number options entered
 //printf("filecount: %d\n", filecount);
  if(filecount == 1){
   sprintf(command, "find -type f -name \"*.%s\" -print0 | tar czvf backup.tar.gz --null -T -", files[1]);
   //printf("command:%s\n", command);
  }
  
  else if(filecount == 2){
      sprintf(command, "find  -type f \\( -name \"*.%s\" -o -name \"*.%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2]);

  }
    else if(filecount == 3){
 sprintf(command, "find  -type f \\( -name \"*.%s\" -o -name *.\"*.%s\" -o -name *.\"*.%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2], files[3]);
  }
    else if(filecount == 4){
     sprintf(command, "find  -type f \\( -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2], files[3], files[4]);
  }
    else if(filecount == 5){
      sprintf(command, "find  -type f \\( -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2], files[3], files[4], files[5]);
      }
    else if(filecount == 6){
   sprintf(command, "find  -type f \\( -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" -o -name \"*.%s\" \\) -print0 | tar czvf backup.tar.gz --null -T -", files[1], files[2], files[3], files[4], files[5], files[6]);
  }
  

    FILE *fp = popen(command, "r");
  printf("S$:FILE CREATED\n");
  sleep(5); 
  getfile(socketfd,"backup.tar.gz");
  memset(command,0,strlen(command));

}



void dgetfiles(char *date1, char *date2, int socketfd){ // dget files takes dates from client and runs the below linux command


FILE *FileOpen;
unlink("backup.tar.gz"); // this used to remove if there is a copy of this file
  char line[1000];
  char command[100];  
  char data[SIZE] = {0};  
  char *error = "No file found";  
  //filename[strlen(filename)-1] = '\0'; 
  //printf("this is from dgetfiles\n");         
sprintf(command, "find -type f -newermt %s ! -newermt %s -print0 | tar czvf backup.tar.gz --null -T -",date1,date2);// write the linux command into command             
  FileOpen = popen(command,"r"); // tuns the command
  printf("S$:FILE CREATED\n");
  sleep(5);
  getfile(socketfd,"backup.tar.gz"); // send it to getfile to send the files data to client
  

}





void processClient(int sd){
char message[1024];
char *options[10];
int option_iterator =0;
int i;
while(1){
	if(read(sd, message, 1024)<0){   
close(sd);
fprintf(stderr,"Client is dead, wait for a new client\n");
exit(0);
}

	message[strlen(message)-1] = '\0'; 
   char * token = strtok(message, " ");
   // loop through the string to extract all other tokens
   while( token != NULL ) {
      options[option_iterator] = token;
      token = strtok(NULL, " ");
      option_iterator++;

   }


char *opt1 = options[0];
//opt1[strlen(opt1)-1] = '\0';
fprintf(stderr, "Client wants : %s\n", message);
   //calling command function which execute given cmd and return output string wrt that command
      if((i = strcmp(options[0], "findfile") ==0)){
   findfile(sd,options[1]);
}
 
//options[0][0] == 's' 
      else if((i = strcmp(opt1, "sgetfiles") ==0)){
   //printf("this from sgetfiles IF\n");
   sgetfiles(atoi(options[1]),atoi(options[2]), sd);
   
}

      else if((i = strcmp(opt1, "dgetfiles") ==0)){
   //printf("this from dgetfiles IF\n");
   dgetfiles(options[1],options[2], sd);
   
}

      else if((i = strcmp(opt1, "getfiles") ==0)){
   //printf("this from getfiles IF\n");
   getfiles(options,option_iterator,sd);
   }
   else if((i = strcmp(opt1, "gettargz") ==0)){
   //printf("this from gettargz IF\n");
   gettargz(options,option_iterator,sd);
   
}

      else if((i = strcmp(opt1, "quit") ==0)){
      //printf("this is from quit\n");
      exit(0);
}
option_iterator = 0;
memset(message,0,strlen(message)); // clears the message for the next iterations
}
}
