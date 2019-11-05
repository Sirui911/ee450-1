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
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>
#include <limits>
//#include "serverA.h"
using namespace std;

#define LOCALIP "127.0.0.1" // IP Address of Host
#define UDPPORT 23984 // UDP Port # backend servers connects to
#define SERVERAPORT 21984
#define BUFLEN 10 // Length of socket stream buffer

char buf [BUFLEN];
char recvMapID [BUFLEN];
char recvVertexIndex[BUFLEN];
char recvFileSize[BUFLEN];
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
    int numVert; // number of vertices in Map
    int numEdge; // number of edges in Map
    vector<vector<int> > adjmat;
    map<int,int> nodeMap;
    vector<int> node1;
    vector<int> node2;
    vector<int> edge; // distance in Km
    vector< pair <int, int> > orderedDistPairs;
    
    void dijkstra(int source);
    int minDistance(int dist[], bool sptSet[]);
    void printDijkstra(vector< pair <int, int> > orderedDistPairs);
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
    
    if (::bind(serverA_sockfd, (struct sockaddr *) &serverAAddr, sizeof(serverAAddr)) == -1 ){
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
                nodeIndex = 0;
                graphs.push_back(graph());
                graphs[graphsIndex].mapID = word.at(0);
                cout << "Debug MapID: " << graphs[graphsIndex].mapID << endl;
                
                // store propagation speed
                fileInput >> graphs[graphsIndex].propSpeed;
                cout << "Debug PropSpeed: " << graphs[graphsIndex].mapID << ": " << graphs[graphsIndex].propSpeed << endl;
                
                // store Transmission speed
                fileInput >> graphs[graphsIndex].transSpeed;
                cout << "Debug TransSpeed: " << graphs[graphsIndex].mapID << ": " << graphs[graphsIndex].transSpeed << endl;
                
                // while fileInput != isalpha and != eof
                fileInput >> word;
                
                while(isalpha(word.at(0)) == false && fileInput.eof() == false ){
                    
                    graphs[graphsIndex].node1.push_back(stoi(word));
                    fileInput >> word;
                    graphs[graphsIndex].node2.push_back(stoi(word));
                    fileInput >> word;
                    graphs[graphsIndex].edge.push_back(stoi(word));
                    fileInput >> word;
                    
                   // For Debug only:
                    cout << "Debug node1: " << graphs[graphsIndex].mapID << ": " << graphs[graphsIndex].node1[nodeIndex] << endl;
                    nodeIndex++;
                }
                
                
                
                graphsIndex++;
            } // end of MapID
            
            
            
        }
        
        // determine number of maps
        
        cout << "The Server A has constructed a list of " << graphs.size() << " maps: " << endl << "-------------------------------------------" << endl;
        
        
        vector<int> combinedNodes;
        vector<int>::iterator it;
        for (int i = 0; i < graphs.size(); i++){
            combinedNodes.resize(graphs[i].node1.size() + graphs[i].node2.size());
            copy(graphs[i].node1.begin(), graphs[i].node1.end(), combinedNodes.begin());
            copy(graphs[i].node2.begin(), graphs[i].node2.end(), combinedNodes.begin() + graphs[i].node1.size() );
            
            sort(combinedNodes.begin(),combinedNodes.end());
            it = unique(combinedNodes.begin(), combinedNodes.end());
           
            
            
            
            
            // determine number of edges in each map
            graphs[i].numEdge = graphs[i].edge.size();
            
            // vector of unique nodes
            combinedNodes.resize(distance(combinedNodes.begin(),it));
            
            // create map to index node numbers
            int mapKey = 0;
            for(it = combinedNodes.begin(); it < combinedNodes.end(); it++){
                graphs[i].nodeMap[mapKey] = *it;
                mapKey++;
            }
            
            // determine number of verticies in each map
            graphs[i].numVert = combinedNodes.size();
            
            
            // create size of adjacency matrix such that:
            // row = node1, col = node2
            graphs[i].adjmat.resize(graphs[i].numVert, vector<int>(graphs[i].numVert));
            
            
            int node1MapKey = -1;
            int node2MapKey = -1;
            //loop through nodes and construct adjacency matrix
            for (int j = 0; j < graphs[i].edge.size(); j++) {
                // find Key of mapped node value
                for (auto it = graphs[i].nodeMap.begin(); it != graphs[i].nodeMap.end(); it++){
                    if (it->second == graphs[i].node1[j]){
                        node1MapKey = it->first;
                    }
                    if (it->second == graphs[i].node2[j]){
                        node2MapKey = it->first;
                    }
                }
                graphs[i].adjmat[node1MapKey][node2MapKey] = graphs[i].edge[j];
                graphs[i].adjmat[node2MapKey][node1MapKey] = graphs[i].edge[j];
               // }
            }
            
            combinedNodes.clear();
        } // end of for iterating through each MapID
        
        cout << "Map ID   Num Vertices   Num Edges" << endl << "-------------------------------------------" << endl;
        for (int i = 0; i < graphs.size(); i++){
            cout << graphs[i].mapID << setw(15) << graphs[i].numVert << setw(15) << graphs[i].numEdge << endl;
        }

        cout << "-------------------------------------------" << endl;
        // a = 97, z = 122
        // A = 65, Z = 90
        
        
        //DEBUG: Prints Adjacency Matrix *******
        
        int flag = 1;
        for( auto it2 = graphs[2].nodeMap.begin(); it2 !=  graphs[2].nodeMap.end(); it2++){
            if (flag){
                cout <<  setw(8) << it2->second;
                flag = 0;
            }
            else
                cout <<  setw(4) << it2->second;
        }
        cout << endl << endl;
        
        map<int, int>::iterator it1 = graphs[2].nodeMap.begin();
        for( int i = 0; i < graphs[2].numVert; i++){
            cout << setw(4) << it1->second;
             for( int j = 0; j < graphs[2].numVert; j++){
                 cout <<  setw(4) << graphs[2].adjmat[i][j];
             }
            cout << endl;
            
            it1++;
        }
        
       

    } // end of reading file
    
    else {
        std::cout << "Error: File not found :(" << std::endl;
    }
    
    
} // end of construct map function


void recvFromAWS(){
    socklen_t awsLen = sizeof(awsAddrUDP);
//    recv map ID
    if ((recvLen1 = recvfrom(serverA_sockfd, recvMapID, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
        
//    recv source vertex index
    if ((recvLen1 = recvfrom(serverA_sockfd, recvVertexIndex, BUFLEN, 0, (struct sockaddr *) &awsAddrUDP, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
        
    cout << "The Server A has received input for finding shortest paths: starting vertex " << recvVertexIndex << " of map " << recvMapID << "." << endl;
}

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int graph::minDistance(int dist[], bool sptSet[]){
    // Initialize min value
    int min = INT_MAX, min_index;
  
    for (int v = 0; v < numVert; v++){
        if (sptSet[v] == false && dist[v] <= min){
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// A utility function to print the constructed distance array
void graph::printDijkstra(vector< pair <int, int> > orderedDistPairs)
{
    
    cout << "The Server A has identified the following shortest paths:" << endl;
    cout << "-----------------------------\nDestination\t\t" << "Min Length\n-----------------------------" << endl;

    for ( auto it = orderedDistPairs.begin(); it != orderedDistPairs.end(); it++){
        cout << nodeMap[it->first] << setw(20) << it->second << endl;
    }
    cout << "-----------------------------" << endl;
}


// Dijkstra algorithm resource: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
void graph::dijkstra(int source){
    int dist[numVert]; // The output array.  dist[i] will hold the shortest
       // distance from src to i
     
       bool sptSet[numVert]; // sptSet[i] will be true if vertex i is included in shortest
       // path tree or shortest distance from src to i is finalized
     
       // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < numVert; i++){
           dist[i] = INT_MAX;
           sptSet[i] = false;
    }
       
    int sourceKey = -1;
    // convert source node input to key so it corresponds to adjacency matrix
    for (auto it = nodeMap.begin(); it != nodeMap.end(); it++){
        if (it->second == source){
            sourceKey = it->first;
        }
    }
    if (sourceKey == -1){
        cout << "Source node not found in Map" << endl;
        exit(EXIT_FAILURE);
    }
        
        // Distance of source vertex from itself is always 0
       dist[sourceKey] = 0;
     
       // Find shortest path for all vertices
       for (int count = 0; count < numVert - 1; count++) {
           // Pick the minimum distance vertex from the set of vertices not
           // yet processed. u is always equal to src in the first iteration.
           int u = minDistance(dist, sptSet);
     
           // Mark the picked vertex as processed
           sptSet[u] = true;
     
           // Update dist value of the adjacent vertices of the picked vertex.
           for (int v = 0; v < numVert; v++)
     
               // Update dist[v] only if is not in sptSet, there is an edge from
               // u to v, and total weight of path from src to  v through u is
               // smaller than current value of dist[v]
               if (!sptSet[v] && adjmat[u][v] && dist[u] != INT_MAX && dist[u] + adjmat[u][v] < dist[v])
                   dist[v] = dist[u] + adjmat[u][v];
       }
     

    // add shortest path elements in pair to reorder by distance
    for (int i = 0; i < numVert; i++){
        orderedDistPairs.push_back( make_pair(i ,dist[i]) );
    }
    
    //lambda funct for sorting pairs by distance instead of vertex
    std::sort(orderedDistPairs.begin(), orderedDistPairs.end(), [](const std::pair<int,int> &left, const std::pair<int,int> &right) {
        return left.second < right.second;
    });
    orderedDistPairs.erase(orderedDistPairs.begin());
    
       // print shortest path
       printDijkstra(orderedDistPairs);
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
        //int graphIndex = (int)(size_t)toupper(recvMapID[0]) - 65;
        int graphIndex = 0;
        for (int i = 0; i < graphs.size(); i++){
            if (graphs[i].mapID == recvMapID[0])
                graphIndex = i;
        }
        //cout << testIndex << endl;
        //cout << graphs[(int)(size_t)recvMapID - 65].mapID << endl;
        graphs[graphIndex].dijkstra(atoi(recvVertexIndex));
        
        
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
