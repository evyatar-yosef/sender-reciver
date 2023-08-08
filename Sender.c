#include "header.h"
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <netinet/tcp.h>
#include <sys/time.h>



int main()
{
    //### Reading the file and put it in an array ###//

    int characters = -1; // stands for the size of characters in the file
    char ch;
    FILE * file;
    file = fopen("test.txt", "r");

    while ((ch = fgetc(file)) != EOF)
    {
        characters++;    // counting the number of characters in the file
    }

    FILE * file1;
    file1 = fopen("test.txt", "r");
    
    //### Spliting The file into two arrays ###//

    char arr1[characters/2];
    char arr2[characters/2];
    int arrOneSize = 500000;
    int arrTwoSize = 500000;
    int bytesSent2;

    for (size_t i = 0; i < characters; i++)
    {
        if (i< characters/2)
        {
            arr1[i] = fgetc(file1);
        }
        else
        {
            arr2[i-characters/2] = fgetc(file1);
        }
    }
    //### CREATES THE SOCKET ###//
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Could not create socket\n");       // Vailidation of the Socket creations..
    }
    else if (sockfd == 0)
    {
        printf("Created socket succesfully\n");
    }

    struct sockaddr_in serverAddress; // new struct decleration of sockaddr_in type
    struct sockaddr_in * pserver_addr = &serverAddress; // Pointer Decleration
    memset(&serverAddress,0, sizeof(serverAddress)); // zerod the last 8 bits so it will match the suckaddr struct

    serverAddress.sin_family = AF_INET; // value = AF_INET. match to the sa_family of suckaddr struct
    serverAddress.sin_port = htons(20000); // switch from host byte order to network(Big endian) byte order.
    int convert = inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr); // converting the IPv4 from text to binary
	if (convert <= 0)                                                       // Checking if the conversion worked
	{
		printf("inet_pton() failed\n"); 
		return -1;
	}
    
    //### CONNECTING TO THE SERVER ###///

    int sockcon = connect(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)); 

      if (sockcon == -1)                                                                      // Vailidation of the connections between
    {                                                                                         //  theserver      
        printf("connect() failed with error code\n\n");
    }
    else if (sockcon == 0)
    {
        printf("Connected to server succesfully!\n\n");
    }
    int userChoise = 1;
    char *exitMessage = "1111";
    int exitMessageLen = strlen(exitMessage) + 1;
    char *stayMessage = "0000";
    int stayMessageLen = strlen(exitMessage) + 1;
    int i = 0;
    int bytesSent;


    while (userChoise == 1)
    {  
        int pointer = 0;
        printf("Sending the first half of the file...\n\n");

         while (arrOneSize > 0)
         {

            bytesSent = send(sockfd, arr1 + pointer, 5000, 0);
            arrOneSize -= bytesSent;
            pointer += 5000;
         }
        
        pointer = 0;
        if ( bytesSent == 0 || bytesSent == -1)
    {
        printf("didnt send\n\n");
    }

        if ( bytesSent > 0)
    {
        printf("Send succesfully!\n\n");
    }
        
        char bufferReply[12] = {'\0'};
        char sample[] = "10010101110";
        int bytesReceived = recv(sockfd, bufferReply, 12, 0);
        if (strcmp(sample, bufferReply) == 0)
        {
            printf("The server send an Authentication!\n\n");

            setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, "reno", 4);
            printf ("Chanaging the CC Algorithm to reno...\n\n");

        while (arrTwoSize > 0)
        {
            bytesSent2 = send(sockfd, arr2 + pointer, 5000, 0); // Send the second part of the file
            arrTwoSize -= bytesSent2;
            pointer += 5000;
        }

            printf ("Sending the second part of the file...\n\n");
        }
        else("NO ACK");
        
        printf("Do you want to send the file again? \npress 1 if yes, any other key for no\n\n");
        userChoise = 0;
        scanf("%d", &userChoise);
        
        if(userChoise == 1)
        {
            int x = send(sockfd, stayMessage, 5, 0);
            printf("Sending the server that I wanna stay and keep sending messages... \n\n");
            setsockopt(sockfd,SOL_SOCKET, TCP_CONGESTION, "cubic", 5);  // Change the CC Algorithm
            printf ("Chanaging the CC Algorithm to cubic...\n\n");
            arrOneSize = 500000;
            arrTwoSize = 500000;
        }

        if (userChoise != 1)
        {
            int x = send(sockfd, exitMessage, exitMessageLen, 0);
            printf("Sending the server an Exit message and closing the socket! \n\n");
            break;
        }
        
    }
    
    printf("Disconnected from the server.\n");
    close(sockfd);

    return 1;
}