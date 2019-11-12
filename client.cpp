//
//  client.cpp
//  ee450
//
//  Created by Sam Bruner on 10/13/19.
//  Copyright © 2019 Sam Bruner. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iomanip>
#include <ctype.h>
//#include "client.h"

#define LOCALIP "127.0.0.1" // IP Address of Host
#define AWSPORT 24984 // TCP Port # of AWS server
#define BUFLEN 10 // Length of socket stream buffer

struct sockaddr_in awsAddrTCP;
int aws_TCP_sockfd;

struct sockaddr_in clientAddr;
int client_sockfd;





using namespace std;

void init_TCP(){
    // *** CREATE SOCKET ***
    
     client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
     // *** CONNECT SOCKET ***
     
     // specify AWS address
    
     awsAddrTCP.sin_family = AF_INET;
     //AWS Port #
     awsAddrTCP.sin_port = htons(AWSPORT);
     //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
     //awsAddrTCP.sin_addr.s_addr = htonl(INADDR_LOOPBACK) ;
     awsAddrTCP.sin_addr.s_addr = inet_addr(LOCALIP);
     
     /*
     //DEBUG: check IP
      char *ip = inet_ntoa(awsAddrTCP.sin_addr);
      printf("Debug: Ip Address -> %s\n", ip);
     */

     if ( connect(client_sockfd, (struct sockaddr *) &awsAddrTCP, sizeof(awsAddrTCP)) == -1){
         perror("Error connecting to server");
         exit(EXIT_FAILURE);
     }
     
     
     /*
      Definition of sockaddr_in struct
      
          struct sockaddr_in
          {
            short   sin_family;  must be AF_INET
            u_short sin_port;
            struct  in_addr sin_addr;
            char    sin_zero[8];  Not used, must be zero
          };
      */
}

void sendToAWS(char* mapID, char* vertexIndex, char* fileSize){
    
    string buf;
    
    //store all values in buffer to pass through sock stream
       
       //char buf[BUFLEN];
    
    /*
       strcat(buf, mapID);
       strcat(buf, " ");
       strcat(buf, vertexIndex);
       strcat(buf, " ");
       strcat(buf, fileSize);
       cout << buf << endl;
    */
    /*
    
    if (send(client_sockfd, buf, strlen(buf.c_str()), 0) == -1){
        perror("Error sending data to server socket");
        exit(EXIT_FAILURE);
    }
       */
    
    if (send(client_sockfd, mapID, strlen(mapID), 0) == -1){
        perror("Error sending data to server socket");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 10000000; i++){}
    if (send(client_sockfd, vertexIndex, strlen(vertexIndex), 0) == -1){
        perror("Error sending data to server socket");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 10000000; i++){}
    if (send(client_sockfd, fileSize, strlen(fileSize), 0) == -1){
        perror("Error sending data to server socket");
        exit(EXIT_FAILURE);
    }
     
   

}

void recvFromAWS(){
    
    
}

int main(int argc, const char * argv[]) {
   
   /*
	example client input
	./client <Map ID> <Source Vertex Index> <File Size>
	*/ 

    /* ADD VALID INPUT CHECKING */
    
    
    
    if (argc != 4){
        cout << "Please input in the following format: " << endl << "'./client <Map ID> <Source Vertex Index> <File Size>'" << endl;
        exit(EXIT_FAILURE);
    }
    if (!isalpha(*argv[1])){
        cout << "Map ID must be a letter" << endl;
        exit(EXIT_FAILURE);
    }
    if (!(isdigit(*argv[2]) && isdigit(*argv[3]))){
        cout << "Source vertex index and file size must be numerical digits" << endl;
        exit(EXIT_FAILURE);
    }
    
    init_TCP();
    cout << "The client is up and running." << endl;
    
    
    
    char mapID[BUFLEN];
//    strcpy(mapID,argv[1]);
    sprintf(mapID,"%s",argv[1]);
    
    char vertexIndex[BUFLEN];
//    strcpy(vertexIndex,argv[2]);
    sprintf(vertexIndex,"%s",argv[2]);

    char fileSize[BUFLEN];
//    strcpy(fileSize,argv[3]);
    sprintf(fileSize,"%s",argv[3]);
    
    
    /*
     Sources for setting up socket:
     Eduonix Learning Solutions: https://www.youtube.com/watch?v=LtXEMwSG5-8
     CMU 441 Computer Networks - Course powerpoint
     https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
     */
    
    
    // *** SEND DATA TO AWS ***
    
    // int send(int sockfd, const void *msg, int len, int flags);
    
    sendToAWS(mapID, vertexIndex, fileSize);

    int client_portNum;
    socklen_t len = sizeof(clientAddr);
    if (getsockname(client_sockfd, (struct sockaddr *)&clientAddr, &len) == -1)
        perror("Error getting client port number");
    else
        client_portNum = ntohs(clientAddr.sin_port);
     
    // Print message of data sent to AWS
    cout << "The client has sent query to AWS using TCP over port: " << client_portNum << " start vertex " << vertexIndex << "; map: " << mapID << "; file size: " << fileSize << "." << endl;
    

    
    // *** RECEIVE DATA FROM AWS AND PRINT RESULT ***
    
    /*
    
    cout << "The client has received results from AWS:" << endl;
    cout << "----------------------------------------------" << endl;
    cout << "Destination" <<setw(5) << "Min Length" << setw(7) << "Tt" << setw(6) << "Tp" << setw(5) << "Delay" << endl;
     
    
    Destination     Min Length       Tt      Tp     Delay
    ----------------------------------------------
    1                   10          0.10    0.10    0.20
    2                   20          0.10    0.20    0.30
    7                   21          0.10    0.21    0.31
    …
    ----------------------------------------------
     */
    
    // *** CLOSE CONNECTION ***
    
    close(client_sockfd);
    
    return 0;
}


