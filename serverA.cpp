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
#include <fstream>
#include <vector>
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

// structure to hold map.txt
struct graph{
    char mapID;
    double propSpeed; // in km/s
    double transSpeed; // in Bytes/s
    vector<int> node1;
    vector<int> node2;
    vector<int> edge; // distance in Km
};
// vector of struct graph to hold all possibile mapIDs
vector<graph> graphs;


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


void constructMap(){
    
    // Open the file "map.txt" for input
    std::ifstream fileInput("map.txt");
    if (fileInput.is_open()){
        
        // index of graphs struct
        int graphsIndex = 0;
        int nodeIndex = 0;
        string word;
        int num;
        int i;
        // Continue looping as long as not at EOF!
        while (fileInput.eof() != true) {
            // Use std::getline to grab a whole line
            /*
            std::string line;
            std::getline(fileInput, line);
            std::cout << line << std::endl;
             */
            if (graphsIndex == 0){
                
                fileInput >> word;
            }
            //cout << word << endl;
            
            // check if filestream is start of new MapID
            if ( isalpha(word.at(0)) ){
                i = 0;
                graphs.push_back(graph());
                graphs[graphsIndex].mapID = word.at(0);
                cout << "Debug MapID: " << graphs[graphsIndex].mapID << endl;
                
                // store propagation speed
                fileInput >> graphs[graphsIndex].propSpeed;
                cout << "Debug PropSpeed: " << graphs[graphsIndex].mapID << ": " << graphs[graphsIndex].propSpeed << endl;
                
                // store Transmission speed
                fileInput >> graphs[graphsIndex].transSpeed;
                cout << "Debug TransSpeed: " << graphs[graphsIndex].mapID << ": " << graphs[0].transSpeed << endl;
                
                // while fileInput ! isalpha
                fileInput >> word;
                
                while(isalpha(word.at(0)) == false && fileInput.eof() == false ){
                    graphs[graphsIndex].node1.push_back(stoi(word));
                    fileInput >> word;
                    graphs[graphsIndex].node2.push_back(stoi(word));
                    fileInput >> word;
                    graphs[graphsIndex].edge.push_back(stoi(word));
                    fileInput >> word;
                    
                    
                    cout << "Debug node1: " << graphs[graphsIndex].mapID << ": " << graphs[graphsIndex].node1[i] << endl;
                    i++;
                }
                i = 0;
                
                // end of MapID
                graphsIndex++;
            }
            
            
            
        }
        
    }
    
    else {
        std::cout << "Error: File not found :(" << std::endl;
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
    
    constructMap();
    
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
