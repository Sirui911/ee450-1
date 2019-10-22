//
//  serverA.cpp
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
//#include "serverA.h"
using namespace std;

#define LOCALIP "127.0.0.1" // IP Address of Host
#define TCPPORT 24984 // TCP Port # client connects to
#define UDPPORT 23984 // UDP Port # backend servers connects to
#define SERVERAPORT 21984
#define SERVERBPORT 22984
#define BACKLOG 3 // backlog of pending connections for listen
#define BUFLEN 10 // Length of socket stream buffer



int main (){
    
    // ********* UDP AWS SOCKET CONNECTION *********
    // *** 1. CREATE SOCKET ***
      
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    char buf [BUFLEN + 1];
    char mapID [10];
    char vertexIndex[10];
    char fileSize[10];
          
    int recvLen1, recvLen2, recvLen3, recvLen4;
    
    // Boot up message
    cout << "The AWS is up and running." << endl;
    
    // specify serverA address
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    //serverA Port #
    servAddr.sin_port = htons(SERVERAPORT);
    //serverA IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    servAddr.sin_addr.s_addr = inet_addr(LOCALIP);
    
    // *** 2. BIND SOCKET ***
    
    if (bind(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1 ){
        cout << "Error binding socket." << endl;
    }
    
    
    
    // infinite while loop for AWS to accept TCP client connections
    while (1) {
    
    
    // *** 3. RECEIVE DATA FROM CLIENT ***
        
        recvLen1 = recv(new_sockfd, mapID, sizeof(mapID), 0);

        
         cout << "DEBUG RECVLEN:" << recvLen1 << " " << recvLen2 << " " << recvLen3 << endl;
    }
    return 0;
}



/*
 <Map ID 1>
<Propagation speed>
<Transmission speed>
<Vertex index for one end of the edge> <Vertex index for the other end> <Distance between the two vertices>
… (Specification for other edges)
<Map ID 2>
 */
