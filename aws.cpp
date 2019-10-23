//
//  aws.cpp
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
//#include "aws.h"
using namespace std;


#define LOCALIP "127.0.0.1" // IP Address of Host
#define TCPPORT 24984 // TCP Port # client connects to
#define UDPPORT 23984 // UDP Port # backend servers connects to
#define SERVERAPORT 21984
#define SERVERBPORT 22984
#define BACKLOG 3 // backlog of pending connections for listen
#define BUFLEN 10

int aws_TCP_sockfd;
char buf [BUFLEN];
char mapID [BUFLEN];
char vertexIndex[BUFLEN];
char fileSize[BUFLEN];
int recvLen1, recvLen2, recvLen3, recvLen4;

struct sockaddr_in awsAddrTCP;
int client_sockfd;
struct sockaddr_in clientAddr;

struct sockaddr_in serverAAddr, serverBAddr;
int aws_UDP_sockfd, serverA_sockfd, severB_sockfd;

int init_TCP(){
    
    // *** 1. CREATE SOCKET ***
       if ( (aws_TCP_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
           cout << "Error creating socket." << endl;
           return EXIT_FAILURE;
       }
       // specify AWS address
       awsAddrTCP.sin_family = AF_INET;
       //AWS Port #
       awsAddrTCP.sin_port = htons(TCPPORT);
       //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
       awsAddrTCP.sin_addr.s_addr = inet_addr(LOCALIP);
       
       // *** 2. BIND SOCKET ***
       
       if (bind(aws_TCP_sockfd, (struct sockaddr *) &awsAddrTCP, sizeof(awsAddrTCP)) == -1 ){
           cout << "Error binding socket." << endl;
           return EXIT_FAILURE;
       }
    // *** 3. LISTEN FOR CONNECTIONS ***
    
    listen(aws_TCP_sockfd, BACKLOG);
    
    return EXIT_SUCCESS;
}



int main (){
    
    // ********* FOR CLIENT SOCKET CONNECTION *********
    
    if (init_TCP() == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // Boot up message
    cout << "The AWS is up and running." << endl;
    
    
    
    
    // *** 4. ACCEPT CONNECTIONS ***
    
    //clientlen from CMU class notes
    socklen_t clientLen = sizeof(clientAddr);
    
//     infinite while loop for AWS to accept TCP client connections
    while (1) {
    
        //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        if ( (client_sockfd = accept(aws_TCP_sockfd,(struct sockaddr *) &clientAddr, &clientLen)) == -1  ){
            cout << "Error accepting socket." << endl;
        }
    
    // *** 5. RECEIVE DATA FROM CLIENT ***
        
        recvLen1 = recv(client_sockfd, buf, BUFLEN, 0);
        if (recvLen1 < 0){
            cout << "Error receiving message from client" << endl;
            return EXIT_FAILURE;
        }
        
        buf[recvLen1] = '\0';
        
//      Separate buffer message into mapID, vertexIndex, and fileSize
        mapID[0] = buf[0];
        mapID[strlen(mapID)] = '\0';
        int i = 2;
        int j = 0;
        while (buf[i] != ' '){
            vertexIndex[j] = buf[i];
            i++;
            j++;
        }
        j=0;
        while (buf[i] != '\0'){
            fileSize[j] = buf[i+1];
            i++;
            j++;
        }
        
        vertexIndex[strlen(vertexIndex)] = '\0';
        fileSize[strlen(fileSize)] = '\0';
        
        // output message for receiving data from client
        cout << "The AWS has received map ID " << mapID << ", start vertex " << vertexIndex << " and file size " << fileSize << " from the client using TCP over port " << TCPPORT << endl;
    } // end of while
    
    
//    ************************** UDP Setup *****************************
    
    // *** 1. CREATE SOCKET ***
    serverA_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // specify AWS address
    
    serverAAddr.sin_family = AF_INET;
    //AWS Port #
    serverAAddr.sin_port = htons(UDPPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverAAddr.sin_addr.s_addr = inet_addr(LOCALIP);
    
    // *** 2. BIND SOCKET ***
    
    if (bind(serverA_sockfd, (struct sockaddr *) &serverAAddr, sizeof(serverAAddr)) == -1 ){
        cout << "Error binding socket." << endl;
    }
    
    return EXIT_SUCCESS;
}

