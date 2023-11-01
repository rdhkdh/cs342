// Client side C/C++ program to demonstrate Socket 
// programming 
#include <bits/stdc++.h>
#include <arpa/inet.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#define PORT 8080 

using namespace std;

// Simulate a delay in seconds.
void wait(int seconds) {
    clock_t start_time = clock();
    while ((clock() - start_time) / CLOCKS_PER_SEC < seconds) {}
}

int main(int argc, char const* argv[]) 
{ 
	int status, valread, client_fd; 
	struct sockaddr_in serv_addr; 
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

	// Convert IPv4 and IPv6 addresses from text to binary 
	// form 
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) 
		<= 0) { 
		printf( 
			"\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if ((status 
		= connect(client_fd, (struct sockaddr*)&serv_addr, 
				sizeof(serv_addr))) 
		< 0) { 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	int receivedmsg;
	while(1){
		int bytesReceived = recv(client_fd, &receivedmsg, sizeof(receivedmsg), 0);
    		char* ack = "Acknowlegment received from client" ;
    		wait(receivedmsg%10);  //second digit is transmission delay of data packet so receiver needs to wait for that much time
    		send(client_fd, ack, strlen(ack), 0); 
		printf("Acknowledgement sent for packet number "); 
		printf("%d", receivedmsg/10);   
		printf("\n");	
	}
	

	// closing the connected socket 
	close(client_fd); 
	return 0; 
}

