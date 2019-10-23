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

void init_UDP(){
    // *** 1. CREATE SOCKET ***
    if ( (serverA_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }
    
    // specify ServerA address
    
    serverAAddr.sin_family = AF_INET;
    //AWS Port #
    serverAAddr.sin_port = htons(SERVERAPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverAAddr.sin_addr.s_addr = inet_addr(LOCALIP);
    
    // *** 2. BIND SOCKET ***
    
    if (bind(serverA_sockfd, (struct sockaddr *) &serverAAddr, sizeof(serverAAddr)) == -1 ){
        perror("Error binding UDP socket");
        exit(EXIT_FAILURE);
    }
}

void recvFromAWS(){
    socklen_t awsLen = sizeof(awsAddrUDP);
//    recv map ID
    if ((recvLen1 = recvfrom(serverA_sockfd, mapID, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
        
//    recv source vertex index
    if ((recvLen1 = recvfrom(serverA_sockfd, vertexIndex, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
        
    cout << "The Server A has received input for finding shortest paths: starting vertex " << vertexIndex << " of map " << mapID << "." << endl;
}




int main (){

    init_UDP();
    
    // ServerA boot up message
    cout << "The Server A is up and running using UDP on port " << SERVERAPORT << "." << endl;
    
//    recv Map ID & start Node
//    int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);
    
    while(1){
        recvFromAWS();
        
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
