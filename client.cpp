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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iomanip>
//#include "client.h"

#define LOCALIP "127.0.0.1" // IP Address of Host
#define AWSPORT 24984 // TCP Port # of AWS server
#define BUFLEN 10 // Length of socket stream buffer

using namespace std;


int main(int argc, const char * argv[]) {
   
   /*
	example client input
	./client <Map ID> <Source Vertex Index> <File Size (bits)>
	*/ 

    cout << "The client is up and running." << endl;
    
    /* ADD VALID INPUT CHECKING */
    
    char mapID[BUFLEN];
    strcpy(mapID,argv[1]);
    
    char vertexIndex[BUFLEN];
    strcpy(vertexIndex,argv[2]);
    
    char fileSize[BUFLEN];
    strcpy(fileSize,argv[3]);
    
    char buf[BUFLEN];
    strcat(buf, mapID);
    /*
     Sources for setting up socket:
     Eduonix Learning Solutions: https://www.youtube.com/watch?v=LtXEMwSG5-8
     CMU 441 Computer Networks - Course powerpoint
     https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
     */
    
    // *** CREATE SOCKET ***
   
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // *** CONNECT SOCKET ***
    
    // specify AWS address
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    //AWS Port #
    servAddr.sin_port = htons(AWSPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    //servAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK) ;
    servAddr.sin_addr.s_addr = inet_addr(LOCALIP);
    
    /*
    //DEBUG: check IP
     char *ip = inet_ntoa(servAddr.sin_addr);
     printf("Debug: Ip Address -> %s\n", ip);
    */

    if ( connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1){
        cout << "Error connecting to server" << endl;
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
    
    // *** SEND DATA TO AWS ***
    
    // int send(int sockfd, const void *msg, int len, int flags);
    if (send(sockfd, mapID, strlen(mapID), 0) == -1){
        cout << "Error sending data to server socket" << endl;
    }
    if (send(sockfd, vertexIndex, strlen(vertexIndex), 0) == -1){
        cout << "Error sending data to server socket" << endl;
    }
    if (send(sockfd, fileSize, strlen(fileSize), 0) == -1){
        cout << "Error sending data to server socket" << endl;
    }
    
    // Print message of data sent to AWS
    cout << "The client has sent query to AWS using TCP over port: " << AWSPORT << " start vertex " << vertexIndex << "; map: " << mapID << "; file size: " << fileSize << "." << endl;
    

    
    // *** RECEIVE DATA FROM AWS AND PRINT RESULT ***
    
    
    
    cout << "The client has received results from AWS:" << endl;
    cout << "----------------------------------------------" << endl;
    cout << "Destination" <<setw(5) << "Min Length" << setw(7) << "Tt" << setw(6) << "Tp" << setw(5) << "Delay" << endl;
     
    /*
    Destination     Min Length       Tt      Tp     Delay
    ----------------------------------------------
    1                   10          0.10    0.10    0.20
    2                   20          0.10    0.20    0.30
    7                   21          0.10    0.21    0.31
    …
    ----------------------------------------------
     */
    
    // *** CLOSE CONNECTION ***
    
    close(sockfd);
    
    return 0;
}


