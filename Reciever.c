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
    int stayConnnect = 0;
    //char temp[1000];
    char buffer[5000];
    double firstPartTime[20], secondPartTime[20];
    int counter = 0;
    int exitProgram = 0;

    int arrOneSize = 500000;
    int arrTwoSize = 500000;
    int choise = 4;

    int server_sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_sock < 0)
    {
        printf("Could not create socket\n");
        exit(1);
    }
        printf("Created socket succesfully\n");
    

    struct sockaddr_in serverAddress; // new struct decleration of sockaddr_in type
    struct sockaddr_in * pserver_addr = &serverAddress; // Pointer Decleration
    memset(pserver_addr,0, sizeof(serverAddress)); // zerod the last 8 bits so it will match the suckaddr struct

    serverAddress.sin_family = AF_INET; // value = AF_INET. match to the sa_family of suckaddr struct
    serverAddress.sin_port = htons(20000); // switch from host byte order to network(Big endian) byte order.
    int convert = inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);
	if (convert <= 0)
	{
		printf("inet_pton() failed\n");
		exit(1);
	}

    int bindNum = bind(server_sock, (struct sockaddr *)&serverAddress , sizeof(serverAddress));
    if (bindNum == -1)
    {
        printf("Could not bind\n");
        exit(1);
    }
    else if (bindNum == 0)
    {
        printf("Binding succesfully to port number: %d\n", 200000);
    }

    if (listen(server_sock, 400) == -1)
	{
	printf("listen() failed with error code\n");
    exit(1);
    }
    printf("Listening...\n");

    
    //### struct for the client address, size and the accept function which makes the first connection with the sender ###//

    struct sockaddr_in clientAddress;  
    socklen_t clientAddressLen = sizeof(clientAddress);

    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);
    int clientSocket = accept(server_sock, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (clientSocket == -1)
    	{
           printf("listen failed with error code\n");
           return -1;
    	}
    	printf("A new client connection accepted\n");

    //### struct for getting the time of each operation ###//

    struct timeval currentFirstTime ,lastFirstTime, currentSecondTime ,lastSecondTime;

    //### gets the packages of the sender, store them, and calculate the time it took to recieve them ###//
    int xy = 0;
    while (exitProgram == 0)
    {    
            printf("here!!\n");
            gettimeofday(&currentFirstTime, NULL);
            while(arrOneSize > 0)
            {
                printf("here\n");
                bzero(buffer, 5000);
                int recieve1 = recv(clientSocket, buffer, sizeof(buffer), 0);
                arrOneSize = arrOneSize - recieve1;
                xy += recieve1;
                printf("the size is: %d\n",xy);
                printf("arr1: %d\n",arrOneSize);
            }
            gettimeofday(&lastFirstTime, NULL);
            printf("time diffrence is:: %f", time_diff(&currentFirstTime, &lastFirstTime));
            firstPartTime[counter] = time_diff(&currentFirstTime, &lastFirstTime);

            printf("The server recieved the first half of the file succesfully!\n\n");
            char *Authentication = "10010101110";

            int bytesSent = send(clientSocket, Authentication, 12, 0);

            if ( bytesSent > 0)
            {
            printf("send ACK to the client \n\n");
            }

            setsockopt(server_sock, IPPROTO_TCP, TCP_CONGESTION, "reno", 4);
            printf ("Chanaging the CC Algorithm to reno...\n\n");

            gettimeofday(&currentSecondTime, NULL);
            while(arrTwoSize > 0)
            {
                bzero(buffer, 5000);
                int recieve2 = recv(clientSocket, buffer, sizeof(buffer), 0);
                arrTwoSize = arrTwoSize - recieve2;
                printf("the size is: %d\n",recieve2);
                printf("arr2: %d\n",arrTwoSize);
            }

            //### Computing the time the proccess took and putting in an arrayy ###//

            printf("The server recieved the second half of the file succesfully!\n\n");
            gettimeofday(&lastSecondTime, NULL);
            printf("time diffrence is:: %0.8f", time_diff(&currentSecondTime, &lastSecondTime));
            secondPartTime[counter] = time_diff(&currentSecondTime, &lastSecondTime);
            counter++;

            printf("Waiting for the client choose if he wants to continue or not...\n\n");
            char exitOrNot[4] = {'\0'};
            char *sample = "1111";
           
            //### getting the exit/continue message ###// 
            
            while (choise > 0)
            {
            int recieveChoose = recv(clientSocket, exitOrNot, 5, 0);

            if ( exitOrNot[0] == sample[0] && exitOrNot[1] == sample[1] && exitOrNot[2] == sample[2] && exitOrNot[3] == sample[3])
            {
                sleep(2);
                exitProgram = 1;
                choise = -1;
                printf("The client sent an exit message, closing the socket!\n\n");
                break;
            }
            else
            {
                arrOneSize = 500000;
                arrTwoSize = 500000;
                setsockopt(server_sock,SOL_SOCKET, TCP_CONGESTION, "cubic", 5);  // Change the CC Algorithm
                printf ("Chanaging the CC Algorithm to cubic...\n\n");
                break;
           }

    }

    //###  Printing times ###//
    int printer = 0;
    double averageFirstSent = 0;
    double averageSecondSent = 0;
    printf("//###  Printing times ###//\n\n");
    printf("Times of first file:\n");
    while (printer < counter)
    {
        printf("Sent number %d:(MiliSeconds) %f\n", printer+1, firstPartTime[printer]);   
        averageFirstSent+=firstPartTime[printer];
        printer++;  
    }
    printer = 0;
    printf("Times of second file:\n");
    
    while (printer < counter)
    {
        printf("Sent number %d:(MiliSeconds) %f\n", printer+1, secondPartTime[printer]); 
        averageSecondSent+=secondPartTime[printer];
        printer++; 

float time_diff(struct timeval *start, struct timeval *end)
{
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}
    }
    
    printf("\nThe average time for the first sent is: %f\n\n", averageFirstSent/printer);
    printf("The average time for the second sent is: %f\n\n", averageSecondSent/printer);

    printf("[+]Client disconnected.\n\n");
    return 1;
}
}