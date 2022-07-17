#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
void func(int sockfd)
{
    char buff[MAX]; // buffer that transfers messages
    char GoodJobMsg[]="GoodJob You have finished the game\n";
    int n,i,k=1,exitparam=0;// i = countdown , k = server sequence length, exitparam = exit connection

    for (;;) { // always connected for loop
        bzero(buff, sizeof(buff)); // Reset buffer - all 0's
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n') // Input from Client
            ;
        write(sockfd, buff, sizeof(buff)); // Sending buffer to server side
	system("clear"); // Clear the terminal
	if (strncmp(buff, "exit", 4)==0){ // Check if server sends exit to terminate the connection
		printf("Client Exit...1\n");
   	        break;
	}
        bzero(buff, sizeof(buff)); // Reset buffer - all 0's
        read(sockfd, buff, sizeof(buff)); // Read Server's Message
        if (strncmp(buff, "exit", 4)==0) { // If server sent "exit>
            printf("Client Exit...3\n");
            break;
        }

	printf("From Server :\n");
	i=k;
	while(i>=0){ // Countdown Loop
		if(strncmp(buff,GoodJobMsg,strlen(GoodJobMsg))==0){ // If Client Finished the game
			printf("%s",buff);
			exitparam=1; // exitparam == 1 means to terminate the connection
			break;
		}
		else if(strncmp(buff,"Incorrect",strlen("Incorrect"))==0){ // If Client's Input was Incorrect , terminate
                        printf("%s",buff);
                        exitparam=1;
                        break;
                }
		else if(i==0)
                        printf("\033[A\33[2K\r\n"); // Clears the last line of output
		else
			printf("\033[A\33[2KT\r%s T minus %d seconds..\n",buff,i); // Clears and writes a new output for countdown
		i--;
		sleep(1); // 1 second sleep
	}
	k++; // increasing the length of the sequence
	if(exitparam==1){ // if exitparam equals 1 , exit
		printf("\nClient Exit...2\n");
		break;
	}
	printf("\nNow you enter the sequence\n");
    }
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET IPv4 Protocol , SOCK STREAM - Reliable two-way connection-based byte stream
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr)); // Resets servaddr
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET; // IPv4 Protocol
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Specific IP for the connection
    servaddr.sin_port = htons(PORT); // Convert host port byte order to network byte order
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
   
    // function for chat
    func(sockfd);
   
    // close the socket
    close(sockfd);
}
