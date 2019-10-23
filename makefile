all: 
	g++ -o aws aws.cpp
	g++ -o client client.cpp
	g++ -o serverA serverA.cpp
	g++ -o serverB serverB.cpp

.PHONY: aws
aws:
	./aws

.PHONY:serverA
serverA:
	./serverA

.PHONY:serverB
serverB:
	./serverB