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
#include <iomanip>
#include <vector>

//#include "serverB.h"
using namespace std;

#define LOCALIP "127.0.0.1" // IP Address of Host
#define UDPPORT 23984 // UDP Port # backend servers connects to
#define SERVERBPORT 22984
#define BUFLEN 1000 // Length of socket stream buffer

char buf [BUFLEN];
char mapID [BUFLEN];
char vertexIndex[BUFLEN];
char fileSizeBuf[BUFLEN];
int recvLen1;
int sendLen;
char pSpeedBuf[BUFLEN]; // propagation speed
char tSpeedBuf[BUFLEN]; // transmission speed
double propSpeed; // in km/s
double transSpeed; // in Bytes/s
long fileSize;
vector<double> propDelay;
vector<double> transDelay;
vector<double> totDelay;

vector< pair <int, int> > shortestPathPairs;
vector< pair <int, double> > totalDelayPairs;

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
    
    cout << "The Server B is up and running using UDP on port " << SERVERBPORT << "." << endl;
}


void recvFromAWS(){
    char destBuf[BUFLEN];
    char lenBuf[BUFLEN];
    socklen_t awsLen = sizeof(awsAddrUDP);
    memset(buf, '0', sizeof(buf));
    int recvDone = 0; // 0 = not finished receiving, 1 = finished receiving
    
    
    if ((recvLen1 = recvfrom(serverB_sockfd, fileSizeBuf, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen )) < 0){
        perror("Error receiving message from aws");
        exit(EXIT_FAILURE);
        
    }
    
    fileSize = atol(fileSizeBuf);
    
    // Recv 1st propagation speed 2nd transmission speed
    
    if ((recvLen1 = recvfrom(serverB_sockfd, pSpeedBuf, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen )) < 0){
        perror("Error receiving message from aws");
        exit(EXIT_FAILURE);
    }
    pSpeedBuf[recvLen1] = '\0';
    
    propSpeed = atof(pSpeedBuf);
    
    if ((recvLen1 = recvfrom(serverB_sockfd, tSpeedBuf, BUFLEN, 0, (struct sockaddr *)&awsAddrUDP, &awsLen )) < 0){
        perror("Error receiving message from aws");
        exit(EXIT_FAILURE);
    }
    tSpeedBuf[recvLen1] = '\0';
    
    transSpeed = atof(tSpeedBuf);
    
    // receive edge data
    while (!recvDone){
        
        if ((recvLen1 = recvfrom(serverB_sockfd, destBuf, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen )) < 0){
            perror("Error receiving message from aws");
            exit(EXIT_FAILURE);
        }
        destBuf[recvLen1] = '\0';
        cout << "dest: " << destBuf << endl;
        
        // receive min length if destination received is valid
        if (destBuf[0] != '\0'){
            if ((recvLen1 = recvfrom(serverB_sockfd, lenBuf, BUFLEN, 0, (struct sockaddr *)&awsAddrUDP, &awsLen )) < 0){
                perror("Error receiving message from aws");
                exit(EXIT_FAILURE);
            }
            lenBuf[recvLen1] = '\0';
            cout << "len: " << lenBuf << endl;
            
            shortestPathPairs.push_back(make_pair(atoi(destBuf), atoi(lenBuf)) );
        }
        else{
            //toggle recvDone
            recvDone = 1;
        }
        
        cout << recvDone << endl;
        
    } // end while
    
    
    cout << "The Server B has received data for calculation: "<< endl;
    cout << fixed;
    cout << "* Propagation speed: " << setprecision(2) << propSpeed << " km/s" << endl;;
    cout << "* Transmission speed "<< setprecision(2) << transSpeed << " Bytes/s" << endl;
    
    for (auto it = shortestPathPairs.begin(); it != shortestPathPairs.end(); it++){
        cout << "* Path length for destination " << it->first << ": " << it->second << endl;
    }
    
    
}


void calcDelay(){
    int i = 0;
    for (auto it = shortestPathPairs.begin(); it != shortestPathPairs.end(); it++){
        // prop delay
        propDelay.push_back(it->second * propSpeed);
        // trans delay
        transDelay.push_back(fileSize * transSpeed);
        // total delay
        totDelay.push_back(propDelay[i] + transDelay[i]);
        
        totalDelayPairs.push_back(make_pair(it->first, totDelay[i]));
        i++;
    }
    
    cout << "The Server B has finished the calculation of the delays: " << endl;
    cout << "------------------------" << endl << "Destination" << setw(10) << "Delay" << endl << "------------------------" << endl;
    
    for(auto it = totalDelayPairs.begin(); it != totalDelayPairs.end(); it++){
        cout << it->first << setw(20) << it->second << endl;
    }
    
}

void sendToAWS(){
    
    // send delays
    char propBuf[BUFLEN];
    char transBuf[BUFLEN];
    char totBuf[BUFLEN];
    for(int i = 0; i < propDelay.size(); i++){
        // send propagation delay
        sprintf(propBuf, "%f", propDelay[i]);
        if ((sendLen = sendto(serverB_sockfd, propBuf, strlen(propBuf), 0, (struct sockaddr *) &awsAddrUDP, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message to AWS from Server B");
            exit(EXIT_FAILURE);
        }
        memset(propBuf, '\0', sizeof(propBuf));
        
        // send transmission delay
        sprintf(transBuf, "%f", transDelay[i]);
        if ((sendLen = sendto(serverB_sockfd, transBuf, strlen(transBuf), 0, (struct sockaddr *) &awsAddrUDP, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message to AWS from Server B");
            exit(EXIT_FAILURE);
        }
        
        memset(transBuf, '\0', sizeof(transBuf));
        
        // send total delay
        sprintf(totBuf, "%f", totDelay[i]);
        if ((sendLen = sendto(serverB_sockfd, totBuf, strlen(totBuf), 0, (struct sockaddr *) &awsAddrUDP, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message to AWS from Server B");
            exit(EXIT_FAILURE);
        }
        memset(totBuf, '\0', sizeof(totBuf));
    }
    memset(buf, '\0', sizeof(buf));
    // Send NULL char to signify end of communication
    if ( ( sendLen = sendto(serverB_sockfd, buf, strlen(buf), 0, (struct sockaddr *) &awsAddrUDP, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to AWS from Server B");
        exit(EXIT_FAILURE);
    }
    
    
    
    
    cout << "The Server B has finished sending the output to AWS" << endl;
}


int main(){
    
    
    init_UDP();
    
    while(1){
        
        recvFromAWS();
        calcDelay();
        sendToAWS();
        
        
        // erase path data
        shortestPathPairs.clear();
        propDelay.clear();
        transDelay.clear();
        totDelay.clear();
        totalDelayPairs.clear();
    }
    return 0;
}
