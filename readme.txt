

***************************************************************************************************
					Samuel Bruner
					USC ID: 3198542984
***************************************************************************************************

					Project Description:

	This project successfully implements four coherent files - a front end client, AWS server, and two backend servers. The ultimate objective is to calculate the shortest path delays given an initial map ID, starting node, and file size to be transmitted. The client establishes a TCP connection with the AWS server to send this information. The AWS establishes a UDP connection server A to pass on map ID and starting node to compute shortest path distances. This result is returned to AWS while AWS forwards it to server B to compute propagation delay, transmission delay, and total delay for each path. This result is returned to AWS which forwards it to the client. All three servers continue to loop until terminated in the event that a client has multiple queries after receiving the first result.  

***************************************************************************************************
					File Descriptions:

*client.c:
	The client receives command line input - <Map ID> <Source Vertex Index> <File Size>. The client connects to the AWS TCP socket. All of the client's inputs are sent to AWS. The client then receives each destinations respective distance (km), propagation delay (s), transmission delay (s), and total delay (s).
--------------------------------------------------------------------------
*aws.c:
	The AWS boots up and configures TCP connection with the client The AWS receives the map ID, vertex index, and file size via TCP from the client. The map ID / vertex index is sent to server A via UDP and receives shortest path from server A. AWS forwards shortest path data along with propagation speed, transmission speed, and file size to server B via UDP. AWS receives delay from server B. Finally, AWS sends the each destinations respective distance (km), propagation delay (s), transmission delay (s), and total delay (s).
--------------------------------------------------------------------------
*serverA.c:
	Backend server A is used to preprocess the Map text file that contains all possible maps denoted by a valid map ID(a-z,A-Z). Each map contains a propagation speed, transmission speed, and a list of two nodes and their respective edge distance. A vector of structs called "graph" is used to store all relevant information pertaining to each unique map. The map.txt file is read, and data corresponding to each map is stores in its respective graph struct. A list of unique nodes is then created and a map is used to map the index (key) to the original node (value) since nodes may not be continuous starting at 0. The mapped node value is used for all computation and indexing the adjacency matrix that is later created. A summary of the map file is outputted to indicate the number of vertices and edges in each respective Map ID. An adjacency matrix is constructed, following receiving the desired Map ID and starting node from the AWS chosen by the client, to facilitate the implementation of Dijkstra's algorithm. The shortest path from the desired starting node to all possible destinations is outputted to the terminal and passed on to the AWS. 
--------------------------------------------------------------------------
*serverB.c:
	Backend server B is used to calculate propagation, transmission, and total delays of each shortest path to all destinations. Server B uses propagation speed, transmission speed, file size, and shortest path distances received from AWS via UDP to calculate this result. A vector pair is used to store the <node, distance> pair. Each node  The delays are returned to AWS via UDP.
***************************************************************************************************					The format of all the messages exchanged:
Client:
	sendToAWS: sends mapID, vertex index, and file size respectively via TCP to AWS.
	
	recvFromAWS: if first byte is -1 or -2, error has occurred after initially sending to AWS. Otherwise, receive transmission delay (same for all destinations). Continue to receive destination node and respective distance, propagation delay, and total delay until the NULL char signifies end of map. 
--------------------------------------------------------------------------
AWS: 
	recvFromClient: receive mapID, vertex index, and file size respectively via TCP from client.

	sendToA: Sends server A mapID and vertex index via UDP respectively. 
	
	recvFromA: If first byte is -2, server A received an incorrect map ID or source node. Otherwise, AWS receives propagation speed and transmission speed, respectively. Until NULL Char is received, AWS receives destination followed by distance.
	
	sendToB: Sends to server B the file size, propagation speed, and transmission speed, respectively. The shortest path pairs consisting of destination followed by distance are all sent in order. When complete, the NULL char is sent to signify send of transmission. 
	
	recvFromB: if first byte is -1, error has occurred where the file size is too large for server B. Otherwise, AWS receives transmission delay followed by propagation delay and total delay until NULL char is received. Propagation delay and total delay is received in order corresponding to ascending nodes.
	
	sendToClient: sends transmission delay followed by a loop of destination node, length, propagation delay, and total delay until all destinations are sent. Then the NULL char is sent.
--------------------------------------------------------------------------
ServerA:
	recvFromAWS: receives mapID and vertex index via UDP respectively. 

	sendToAWS: sends propagation speed and transmission speed followed by a loop of destination and distance, respectively, until all nodes are sent. Then the NULL char is sent.
--------------------------------------------------------------------------
ServerB:
	recvFromAWS: If first byte is -1, error has occurred where the file size is too large for server B. If first byte is -2, error has occurred where there is an invalid map id or source node. Otherwise, server B receives file size, propagation speed, and transmission speed. Destination and distance are received until the NULL Char is received. 

	sendtoAWS: sends transmission delay followed by a loop of propagation delay and total delay, respectively. When all delays are sent, the NULL char is sent.
	
***************************************************************************************************Any idiosyncrasy of your project. It should say under what conditions the project fails, if any:
	
	The program works under all tested conditions. Server A has exception handling if the requested node or map is not valid. Server A returns the error to the AWS / client and aborts. Server B has exception handling if file size is too large (>9,223,372,036,854,775,807 bits). Server B return the error to AWS and Client and aborts. All files have exception handling for errors relating to creating sockets, binding sockets, sending / receiving to / from sockets, etc. Program execution will terminate upon encountering any of these errors. Ubuntu does not support exception handling for file size overflow mentioned above, so the client will check if file size fits in long long upon reading from command line. 

Port Numbers:
Client: Dynamic
AWS TCP: 24984
AWS UDP: 23984
ServerA: 21984
ServerB: 22984

***************************************************************************************************					Makefile
1. Run "make" to compile all files
2. Run "make aws" to start AWS
3. Run "make serverA" to start server A
4. Run "make serverB" to start server B
5. Run ./client <map ID> <source node> <file size> to query AWS

***************************************************************************************************					Reused Code / Resources:

Sources for setting up socket:
* Eduonix Learning Solutions: https://www.youtube.com/watch?v=LtXEMwSG5-8
* CMU 441 Computer Networks - Course powerpoint (https://www.cs.cmu.edu/~srini/15-441/S10/lectures/r01-sockets.pdf)
* https://en.wikibooks.org/wiki/C_Programming/Networking_in_UNIX
* Beej
* http://www.linuxhowtos.org/C_C++/socket.htm
  

Dijkstra:
https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/

