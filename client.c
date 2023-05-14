#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SIZE 1024
#define SERVER_PORT 8080
#define MIRROR_PORT 8081

/** Method to check if the file specified is available. returns filename, size, date and number of bytes of the file **/
void findfile(int, char *);
/** Method to use in conjunction with any of the specified commands to read server written data and returns the value to respective method **/
void read_file(int, char *, char);
/** Method to get files from the root directory between size 1 and size 2 **/
void sgetfiles(int, int);
/** Method to get files from the root directory created between date 1 and date 2 **/
void dgetfiles(int, int);
/** Method to get files from the root directory between until 6 specified files **/
void getfiles(int, int, char**);
/** Method to return targz files of specified file type **/
void gettargz(int, int, char**);

int get_client()
{   
	int n,count;
	char a[3];
	int file_disc = open("./counter.txt",O_CREAT| O_RDWR, 0777);
	n=read(file_disc,a,3);
	count = atoi(a);
	count++;
	sprintf(a, "%d", count);  
	lseek(file_disc,0,SEEK_SET);
	n=write(file_disc,a,3);
	printf("Client no: %c \n",a[0]);
	return count;
	close(file_disc);

}

int main(int argc, char *argv[]){
	char message[1024];
	int ssd, portNumber,i,var1;
	// Breaks the user inputs into multiple options
	char *options[10];
	int option_iterator = 0;
	socklen_t len;
	//To get the number of clients
	int counter = get_client();   
	if((ssd = socket(AF_INET, SOCK_STREAM, 0))<0){
	fprintf(stderr, "Cannot create socket\n");
	exit(1);
	}
	// setup server address
	struct sockaddr_in server_addr;
	memset(&server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
	    perror("Invalid server address");
	    return -1;
	}
	// setup mirror address
	struct sockaddr_in mirror_addr;
	memset(&mirror_addr, '0', sizeof(mirror_addr));
	mirror_addr.sin_family = AF_INET;
	mirror_addr.sin_port = htons(MIRROR_PORT);
	if (inet_pton(AF_INET, "127.0.0.1", &mirror_addr.sin_addr) <= 0) {
	    perror("Invalid mirror address");
	    return -1;
	}

	// connect to server or mirror alternatively
	struct sockaddr_in *addr;
	if ((counter < 5 || (counter > 8 && counter % 2 == 1))) {
	    printf("Connecting to server...\n");
	    addr = &server_addr;
	} else if(counter < 9 || (counter > 9 && counter % 2 == 0)) {
	    printf("Connecting to mirror...\n");
	    addr = &mirror_addr;
	}
	if (connect(ssd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
	    perror("Connection failed");
	    return -1;
	}
	while(1){
	fprintf(stderr, "C$:Enter the 'quit' to quit or a message for the server\n");
	fgets(message, 1023,stdin);
	write(ssd, message, strlen(message));
	if(message[0] == 'q'&& message[1] == 'u' && message[2] == 'i' && message[3] == 't'){
	close(ssd);
	exit(0);
	}
	
	 message[strlen(message)-1] = '\0';
	   // Extract the first token
	   char * token = strtok(message, " ");
	   // loop through the string to extract all other tokens
	   while( token != NULL ) {
	      options[option_iterator] = token;
	     token = strtok(NULL, " ");
	      //printf("options in wile:%s\n", options[option_iterator]);
	      option_iterator++;
	   }
	   
	char *opt1 = options[0];
	if((i = strcmp(options[0], "findfile") ==0)){
		if (options[1] == NULL) {
			printf("C$: Error: filename not provided.\n");
			printf("C$: Usage: findfile <filename>\n");
			
	    	}
	   	 // check if file exists
	    	if (access(options[1], F_OK) == -1) {
			//printf("Error: file '%s' not found.\n", options[1]);
	    	}
	   	findfile(ssd,options[1]);
	} 	
	else if((i = strcmp(opt1, "sgetfiles") ==0)){
		if (opt1 == NULL) {
			printf("C$: Error: filename not provided.\n");
			printf("C$: Usage: findfile <filename>\n");
			
		}
		sgetfiles(ssd, option_iterator);
	}
	else if((i = strcmp(opt1, "dgetfiles") ==0)){
		dgetfiles(ssd, option_iterator);
	} 
	else if((i = strcmp(opt1, "getfiles") ==0)){
		getfiles(ssd, option_iterator, options);
	} 
	else if((i = strcmp(opt1, "gettargz") ==0)){
		gettargz(ssd, option_iterator, options);
	}
	else {
	printf("C$: Usage Error: allowed commands: 'findfile filename' | sgetfiles size1 size2 <-u> | dgetfiles date1 date2 <-u> | getfiles file1 file2 file3 file4 file5 file6 <-u > | gettargz <extension list> <-u> \n");  
	printf("Kindly re-enter \n");
	}
	memset(message,0,strlen(message));
	memset(message,0,strlen(message));
	option_iterator = 0;
	}
}

void findfile(int sockfd, char *filename){
  int n;
  //filename[strlen(filename)-1] = '\0';
    char buffer[SIZE];
    char *val[10];
    int i = 0;
    n = recv(sockfd, buffer, SIZE, 0);
       char * token = strtok(buffer, " ");
   // loop through the string to extract all other tokens
   while( token != NULL ) {
   	val[i] = token;
   	i++;
      token = strtok(NULL, " ");
   }
   
   if( i = strcmp(val[0], "No") == 0   ){
   printf("C$:FILE NOT FOUND\n");
   }
   
   else{
   
   printf("C$: FILENAME: %s\n SIZE:%s Bytes\n DATE: %s\n", filename, val[0], val[1]);
   }
   i = 0;
   
   
}


void sgetfiles(int sockfd, int count){
//printf("entered sgetfiels\n");
char f = 'n';
if(count>3)
f = 'y';
read_file(sockfd, "temp.tar.gz", f);
}

void dgetfiles(int sockfd, int count){
//printf("entered dgetfiels\n");
char f = 'n';
if(count>3)
f = 'y';
read_file(sockfd, "temp.tar.gz", f);
}

void getfiles(int sockfd, int count, char *files[]){
//printf("entered getfiels\n");
int i;
char f = 'n';
char t[2] = "-u";
//printf("files[c]: %s\n  i : %d", files[count-1], strcmp(files[count-1],t));
i = strcmp(files[count-1],t);
if (i== 0 )
f = 'y';
read_file(sockfd, "temp.tar.gz", f);
}

void gettargz(int sockfd, int count, char *files[]){
//printf("entered gettargzfiels\n");
int i;
char f = 'n';
char t[2] = "-u";
//printf("files[c]: %s\n  i : %d", files[count-1], strcmp(files[count-1],t));
i = strcmp(files[count-1],t);
if (i== 0 )
f = 'y';
read_file(sockfd, "temp.tar.gz", f);
}



void read_file(int sockfd, char *filename, char un){
  int n;
  unlink("received.tar.gz");
  sleep(5);
  int count = 0;
      char buf[SIZE];
FILE* fp = fopen("received.tar.gz", "wb");
if (fp == NULL) {
    perror("fopen failed");
    exit(EXIT_FAILURE);
}

// Receive the file data in chunks and write it to the file
int bytesReceived = 0;
while ((bytesReceived = recv(sockfd, buf,SIZE, 0)) > 0) {
	count +=1;
    int bytesWritten = fwrite(buf, sizeof(char), bytesReceived, fp);
    if(bytesWritten<SIZE)
    break;
    if (bytesWritten != bytesReceived) {
        perror("C$:fwrite failed");
        exit(EXIT_FAILURE);
    }
}

// Check for errors during file transfer
if (bytesReceived < 0) {
    perror("recv failed");
    exit(EXIT_FAILURE);
}

// Close the file and socket

if(count <=1 && bytesReceived <47){

printf("C$: FILE not found\n");
}
else{
printf("C$:FILE RECIEVED\n");}
fclose(fp);
if(un == 'y'){

system("tar -xzvf received.tar.gz");
}
count = 0;
return;
}
