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
#define UDPPORT 23984 // UDP Port # backend servers connects to
#define SERVERAPORT 21984
#define BACKLOG 3 // backlog of pending connections for listen
#define BUFLEN 10 // Length of socket stream buffer

char buf [BUFLEN];
char mapID [BUFLEN];
char vertexIndex[BUFLEN];
char fileSize[BUFLEN];
int recvLen1;

struct sockaddr_in awsAddrUDP;
int aws_UDP_sockfd;

struct sockaddr_in serverAAddr;
int serverA_sockfd;

int init_UDP(){
    // *** 1. CREATE SOCKET ***
    if ( (serverA_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
        cout << "Error creating UDP socket." << endl;
        return EXIT_FAILURE;
    }
    
    // specify ServerA address
    
    serverAAddr.sin_family = AF_INET;
    //AWS Port #
    serverAAddr.sin_port = htons(SERVERAPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverAAddr.sin_addr.s_addr = inet_addr(LOCALIP);
    
    // *** 2. BIND SOCKET ***
    
    if (bind(serverA_sockfd, (struct sockaddr *) &serverAAddr, sizeof(serverAAddr)) == -1 ){
        cout << "Error binding UDP socket." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}






int main (){

    if (init_UDP() == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    
    // ServerA boot up message
    cout << "The Server A is up and running using UDP on port " << SERVERAPORT << "." << endl;
    
//    recv Map ID & start Node
//    int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);
    
    while(1){
    socklen_t awsLen = sizeof(awsAddrUDP);
    recvLen1 = recvfrom(serverA_sockfd, buf, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen);
    
    cout << buf << endl;
        
    recvLen1 = recvfrom(serverA_sockfd, buf, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen);
        
    cout << buf << endl;
    }
//    read map
    
    
    
    
    

    
    

    
    
    
    return EXIT_SUCCESS;
}



/*
 <Map ID 1>
 <Propagation speed>
 <Transmission speed>
 <Vertex index for one end of the edge> <Vertex index for the other end> <Distance between the two vertices>
 … (Specification for other edges)
 <Map ID 2>
 */
