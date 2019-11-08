all: 
	g++ -std=c++11 aws.cpp -o aws
	g++ -std=c++11 client.cpp -o client
	g++ -std=c++11 serverA.cpp -o serverA
	g++ -std=c++11 serverB.cpp -o serverB

.PHONY: aws
aws:
	./aws

.PHONY:serverA
serverA:
	./serverA

.PHONY:serverB
serverB:
	./serverB