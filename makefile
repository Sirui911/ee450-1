all: 
	g++ -std=c++11 -o aws aws.cpp
	g++ -std=c++11 -o client client.cpp
	g++ -std=c++11 -o serverA serverA.cpp
	g++ -std=c++11 -o serverB serverB.cpp

.PHONY: aws
aws:
	./aws

.PHONY:serverA
serverA:
	./serverA

.PHONY:serverB
serverB:
	./serverB