#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>
#define MAX 10
#define PORT 8080
#define SA struct sockaddr

void Sounds(char *buff){ // Sound playing function
	char a[]="aplay -d 1 Simon_";
	char b[]=".wav";
	char str[80]="\0";
	// 3 arrays to create the destination in the folder to play the sound file
	printf("Entering Sound Func %s \n",buff);
	for(int i=0;i<strlen(buff);i++){ // for to concat between the string
		int k=0,j=0;
		while(a[k]!='\0'){ // reach the first empty spot in the array
			str[k]=a[k];
			k++;
		}
		str[k]=buff[i]; // concat the Letter
		k++;
		while(b[j]!='\0'){ // concat b array to the string of the location
			str[k]=b[j];
			j++;
			k++;
		}
		system(str);// play the sound
		bzero(str,80); // reset the string for reach letter
	}
}
void rand_str(char* dest,size_t length){ // random letter function
	char charset[]="RGBY"; // 4 Simon Letters
	srand(time(NULL)); // random function
	size_t index=(double)rand()/RAND_MAX * (sizeof charset-1); // generate random index
	dest[length-1]=charset[index]; // concat the new letter to the string
	dest[length]='\0';
}


void func(int connfd)
{
    char buff[10],ServerMsg[10]; // Buffer between server and client, ServerMsg = temp string to compare
    char GoodJobMsg[]="GoodJob You have finished the game\n";
    int n=0;
    int SeqNum=1; // Sequence Length
    int status=0; // Status of the game [ 0-entry ,1-New Game ,2- Continous Game ] 
    bzero(ServerMsg,10); // Reset the ServerMsg
    // infinite loop for chat
    for (;;) {
        bzero(buff, 10);
        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        if(status==0){ // status = 0 , Checking if player wants to play
		printf("From client: %s\t To client : ", buff);
	}
	if((strncmp(buff,"Y",1)==0)&&(status==0)){ // if player types "Y" he's in
		printf("Client wants to play\n");
		status=1;
		bzero(buff,10);
	}

	if((strncmp(buff,"N",1)==0)&&(status==0)){ // if player types "N" he's out
		write(connfd,"exit",sizeof(4));
		break;
	}

	if(status==1){ // status = 1 , starting a new game, first letter
		bzero(buff,10);
		rand_str(buff,SeqNum); // Generate random letter
		assert(buff[9]=='\0');
		Sounds(buff); // Generate Sounds
		printf("This is the STRING %s\n",buff);
		write(connfd,buff,sizeof(buff));// Write the sequence to client
		strcpy(ServerMsg,buff); // Copy to temp array ServerMsg
		status++; // Progress status
		SeqNum++; // Increase Length
		bzero(buff,10);
	}
	if(status==2 && (strcmp(buff,"")!=0)){ // enter status 2 [ continous game ]
		if(SeqNum>4){ // Last sequence
			if(strcmp(ServerMsg,buff)==-10){ // Comparing the last sequence
                        	write(connfd,GoodJobMsg,sizeof(GoodJobMsg));
                        	write(connfd,"exit",4);
                 	       break;
			}
			else{
				write(connfd,"Incorrect",sizeof("Incorrect"));
                        	write(connfd,"exit",4);
                        	break;
               		}
		 }
		printf("Status -2\n");
		printf("Server - %s\nClient - %s",ServerMsg,buff);
		printf("return %d\n",strcmp(ServerMsg,buff));
		if(strcmp(ServerMsg,buff)==-10){ // Compare the 2 strings
			printf("Entered\n");
			rand_str(buff,SeqNum); // Random function
			assert(buff[9]=='\0');
			write(connfd,"Correct ",8);
			write(connfd,buff,sizeof(buff)); // Input the new string
			Sounds(buff);
			strcpy(ServerMsg,buff); // Copy to temp array
			SeqNum++; // Increase length
		}
		else{
			write(connfd,"Incorrect ",10);
			break;
		}
	}

	bzero(buff,10);

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0 && status>0) { // if Client writes exit, exit
            printf("Server Exit...\n");
	    write(connfd,buff,sizeof(buff));
            break;
        }
    }
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET IPv4 Protocol , SOCK_STREAM = Reliable two-way connection-based byte stream
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr)); // Resets servaddr

    // assign IP, PORT
    servaddr.sin_family = AF_INET; // IPv4 Protocol
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Convert values between host and network byte order
    servaddr.sin_port = htons(PORT); // Convert host port byte order to network byte order

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
	// bind - bind a name to a socket ,SA struct sockaddr
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    // Function for chatting between client and server
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
}
