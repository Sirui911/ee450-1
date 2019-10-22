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

int main (){
    
    // ********* FOR CLIENT SOCKET CONNECTION *********
    // *** 1. CREATE SOCKET ***
      
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    char buf [BUFLEN];
    char mapID [BUFLEN];
    char vertexIndex[BUFLEN];
    char fileSize[BUFLEN];
          
    int recvLen1, recvLen2, recvLen3, recvLen4;
    
    // Boot up message
    cout << "The AWS is up and running." << endl;
    
    // specify AWS address
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    //AWS Port #
    servAddr.sin_port = htons(TCPPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    servAddr.sin_addr.s_addr = inet_addr(LOCALIP);
    
    // *** 2. BIND SOCKET ***
    
    if (bind(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1 ){
        cout << "Error binding socket." << endl;
    }
    
    
    // *** 3. LISTEN FOR CONNECTIONS ***
    
    listen(sockfd, BACKLOG);
    
    
    // *** 4. ACCEPT CONNECTIONS ***
    int new_sockfd;
    struct sockaddr_in clientAddr;
    //clientlen from CMU class notes
    socklen_t clientLen = sizeof(clientAddr);
    // infinite while loop for AWS to accept TCP client connections
    while (1) {
    
        //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        if ( (new_sockfd = accept(sockfd,(struct sockaddr *) &clientAddr, &clientLen)) == -1  ){
            cout << "Error accepting socket." << endl;
        }
    
    // *** 5. RECEIVE DATA FROM CLIENT ***
        
        recvLen1 = recv(new_sockfd, mapID, sizeof(mapID), 0);
        recvLen2 = recv(new_sockfd, vertexIndex, sizeof(vertexIndex), 0);
        recvLen3 = recv(new_sockfd, fileSize, sizeof(fileSize), 0);
        
         cout << "DEBUG RECVLEN:" << recvLen1 << " " << recvLen2 << " " << recvLen3 << endl;
        /*
        recvLen4 = recv(new_sockfd, buf, BUFLEN, 0);
        
        buf[recvLen4] = '\0';
        cout << recvLen4 << endl;
        for (int i = 0; i<recvLen4; i++)
        {
            printf("%s", buf);
        }
         */
        mapID[recvLen1] = '\0';
        vertexIndex[recvLen2] = '\0';
        fileSize[recvLen3] = '\0';
        
        // output message for receiving data from client
//        cout << buf << endl;
        cout << "The AWS has received map ID " << mapID << ", start vertex " << vertexIndex << " and file size " << fileSize << " from the client using TCP over port " << TCPPORT << endl;
    }
    return 0;
}
