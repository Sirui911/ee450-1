//
//  serverB.cpp
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
//#include "serverB.h"
using namespace std;

#define LOCALIP "127.0.0.1" // IP Address of Host
#define UDPPORT 23984 // UDP Port # backend servers connects to
#define SERVERBPORT 22984
#define BUFLEN 10 // Length of socket stream buffer

char buf [BUFLEN];
char mapID [BUFLEN];
char vertexIndex[BUFLEN];
char fileSize[BUFLEN];
int recvLen1;

struct sockaddr_in awsAddrUDP;
int aws_UDP_sockfd;

struct sockaddr_in serverBAddr;
int serverB_sockfd;


void init_UDP(){
    // *** 1. CREATE SOCKET ***
    if ( (serverB_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }
    
    // specify serverB address
    
    serverBAddr.sin_family = AF_INET;
    //AWS Port #
    serverBAddr.sin_port = htons(SERVERBPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverBAddr.sin_addr.s_addr = inet_addr(LOCALIP);
    
    // *** 2. BIND SOCKET ***
    
    if (::bind(serverB_sockfd, (struct sockaddr *) &serverBAddr, sizeof(serverBAddr)) == -1 ){
        perror("Error binding UDP socket");
        exit(EXIT_FAILURE);
    }
}

void recvFromAWS(){
    socklen_t awsLen = sizeof(awsAddrUDP);
//    recv map ID
    if ((recvLen1 = recvfrom(serverB_sockfd, mapID, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
        
//    recv source vertex index
    if ((recvLen1 = recvfrom(serverB_sockfd, vertexIndex, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
        
    cout << "The Server A has received input for finding shortest paths: starting vertex " << vertexIndex << " of map " << mapID << "." << endl;
}

int main(){
    
    init_UDP();
    
    recvFromAWS();
    
    return 0;
}
