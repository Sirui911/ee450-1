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
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
//#include "aws.h"
using namespace std;


#define LOCALIP "127.0.0.1" // IP Address of Host
#define TCPPORT 24984 // TCP Port # client connects to
#define UDPPORT 23984 // UDP Port # backend servers connects to
#define SERVERAPORT 21984
#define SERVERBPORT 22984
#define BACKLOG 3 // backlog of pending connections for listen
#define BUFLEN 10

char buf [BUFLEN];
char mapID [BUFLEN];
char vertexIndex[BUFLEN];
char fileSize[BUFLEN];
int recvLen1, sendLen;
vector< pair <int, int> > shortestPathPairs;

struct sockaddr_in awsAddrTCP, awsAddrUDP;
int aws_TCP_sockfd, aws_UDP_sockfd;

struct sockaddr_in clientAddr;
int client_sockfd;

struct sockaddr_in serverAAddr, serverBAddr;
int serverA_sockfd, severB_sockfd;

// Initializes TCP Port on AWS
void init_TCP(){
    
    // *** 1. CREATE SOCKET ***
       if ( (aws_TCP_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
           perror("Error creating TCP socket");
           exit(EXIT_FAILURE);
       }
       // specify AWS address
       awsAddrTCP.sin_family = AF_INET;
       //AWS Port #
       awsAddrTCP.sin_port = htons(TCPPORT);
       //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
       awsAddrTCP.sin_addr.s_addr = inet_addr(LOCALIP);
       
       // *** 2. BIND SOCKET ***
       
       if (::bind(aws_TCP_sockfd, (struct sockaddr *) &awsAddrTCP, sizeof(awsAddrTCP)) == -1 ){
           perror("Error binding TCP socket"); //error handling found @ geeksforgeeks.org
           exit(EXIT_FAILURE);
       }
    // *** 3. LISTEN FOR CONNECTIONS ***
    
    listen(aws_TCP_sockfd, BACKLOG);
    
}

void init_UDP(){
    // *** 1. CREATE SOCKET ***
    if ( (aws_UDP_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }

    // specify AWS address
    
    awsAddrUDP.sin_family = AF_INET;
    //AWS Port #
    awsAddrUDP.sin_port = htons(UDPPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    awsAddrUDP.sin_addr.s_addr = inet_addr(LOCALIP);
    
    // *** 2. BIND SOCKET ***
    
    if (::bind(aws_UDP_sockfd, (struct sockaddr *) &awsAddrUDP, sizeof(awsAddrUDP)) == -1 ){
        perror("Error binding UDP socket");
        exit(EXIT_FAILURE);
    }
}

void acceptFromClient(){
    //clientlen from CMU class notes
    socklen_t clientLen = sizeof(clientAddr);
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    if ( (client_sockfd = accept(aws_TCP_sockfd,(struct sockaddr *) &clientAddr, &clientLen)) == -1){
        perror("Error accepting socket");
        exit(EXIT_FAILURE);
    }
}

void recvFromClient(){
    if ((recvLen1 = recv(client_sockfd, buf, BUFLEN, 0)) < 0){
        perror("Error receiving message from client");
        exit(EXIT_FAILURE);
    }
           
    buf[recvLen1] = '\0';
}

void separateClientBuffer(){
    //      Separate buffer from client into mapID, vertexIndex, and fileSize
    memset(mapID, '\0' , sizeof(mapID));
    memset(vertexIndex, '\0' , sizeof(vertexIndex));
    memset(fileSize, '\0' , sizeof(fileSize));
    
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
}

// Sets port and IP of serverA and serverB
void setServerAB(){
//    Server A
    serverAAddr.sin_family = AF_INET;
    //AWS Port #
    serverAAddr.sin_port = htons(SERVERAPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverAAddr.sin_addr.s_addr = inet_addr(LOCALIP);
    
//    Server B
    serverBAddr.sin_family = AF_INET;
    //AWS Port #
    serverBAddr.sin_port = htons(SERVERBPORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverBAddr.sin_addr.s_addr = inet_addr(LOCALIP);
}

// Sends Map ID and Vertex index to Server A via UDP
void sendToA(){
    
    if ((sendLen = sendto(aws_UDP_sockfd, &mapID, sizeof(mapID), 0, (struct sockaddr *) &serverAAddr, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server A from AWS");
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(aws_UDP_sockfd, &vertexIndex, sizeof(vertexIndex), 0, (struct sockaddr *) &serverAAddr, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server A from AWS");
        exit(EXIT_FAILURE);
    }
    
    cout << "The AWS has sent map ID and starting vertex to server A using UDP over port " << SERVERAPORT << endl;
}




void recvFromA(){
    char destBuf[BUFLEN];
    char lenBuf[BUFLEN];
    socklen_t serverALen = sizeof(serverAAddr);
    memset(buf, '0', sizeof(buf));
    while (buf[0] != '\0'){
        
        if ((recvLen1 = recvfrom(serverA_sockfd, buf, BUFLEN, 0, (struct sockaddr *) &serverAAddr, &serverALen )) < 0){
            perror("Error receiving message from client");
            exit(EXIT_FAILURE);
        }
        if (buf[0] != '\0'){
            strcpy(destBuf,buf);
            destBuf[recvLen1] = '\0';
        }
        if ((recvLen1 = recvfrom(serverA_sockfd, buf, BUFLEN, 0, (struct sockaddr *)&serverAAddr, &serverALen )) < 0){
            perror("Error receiving message from client");
            exit(EXIT_FAILURE);
        }
        if (buf[0] != '\0'){
            strcpy(lenBuf,buf);
            lenBuf[recvLen1] = '\0';
            shortestPathPairs.push_back(make_pair(atoi(destBuf), atoi(lenBuf)) );
        }
        
    } // end while
    for (auto it = shortestPathPairs.begin(); it != shortestPathPairs.end(); it++){
        cout << it->first << " " << it->second << endl;
    }
}

int main (){
    
    
    
    init_TCP();
    // Boot up message
    cout << "The AWS is up and running." << endl;
    
    
    init_UDP();
    
    setServerAB();
    // ********* FOR CLIENT SOCKET CONNECTION *********
    
    
    
    
//      while(1) loop for main process for AWS to send/recv to/from client, serverA, and serverB
    while (1) {
        
        // *** 4. ACCEPT CONNECTIONS ***
        acceptFromClient();
        // *** 5. RECEIVE DATA FROM CLIENT ***
        recvFromClient();
        separateClientBuffer();
        
        // output message for receiving data from client
        cout << "The AWS has received map ID " << mapID << ", start vertex " << vertexIndex << " and file size " << fileSize << " from the client using TCP over port " << TCPPORT << endl;
        
        sendToA();
        recvFromA();
        
        
    } // end of while(1)
    
    
    
    
    
    
    return EXIT_SUCCESS;
}

