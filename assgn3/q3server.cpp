// Server side C/C++ program to demonstrate Socket 
// programming 
#include <bits/stdc++.h>
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#define PORT 8080 

using namespace std;

// Simulate packet loss by randomly dropping packets.
bool shouldDropPacket(double probability) {
    return ((double)rand() / RAND_MAX) < probability;
}

// Simulate a delay in seconds.
void wait(int seconds) {
    clock_t start_time = clock();
    while ((clock() - start_time) / CLOCKS_PER_SEC < seconds) {}
}



int main(int argc, char const* argv[]) 
{ 
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0}; 
	const int timeOut = 5;
	int dataDelay = 0;
	int ackDelay = 0;

	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, 
				SO_REUSEADDR | SO_REUSEPORT, &opt, 
				sizeof(opt))) { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT); 

	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr*)&address, 
			sizeof(address)) 
		< 0) { 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) { 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	if ((new_socket 
		= accept(server_fd, (struct sockaddr*)&address, 
				(socklen_t*)&addrlen)) 
		< 0) { 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 
	
	
    	int packetNum = 0;
    	srand(time(0));
    	int msg = 0; //msg will be a 2 digit number with the first digit as the packet number and second digit as transmission delay of packet being sent

    	while (packetNum<=3) {
    		msg = 10*packetNum;
        	// Send a packet.
        	cout << "Sender: Sending packet number " << packetNum << endl;

        	if (!shouldDropPacket(0.1)) {  //data packet not lost
        		dataDelay = rand()%10;      //time taken by packet to be transmitted
        	
            		if (dataDelay >= timeOut){  //timeout due to data packet
            			msg = msg + dataDelay;
            			send(new_socket, &msg, sizeof(msg), 0); 
            			wait(timeOut);
                		cout << "Sender: Timeout (high delay) occured for packet number " << packetNum << endl; 
            		}
            		else{			 //data packet reaches before timeout
                		wait(dataDelay);
                		msg = msg + dataDelay;  

                		if (!shouldDropPacket(0.1)) {  //ACK not lost.
                    			ackDelay = rand()%10;

                    			if (ackDelay + dataDelay > timeOut) { // timeout due to ACK.
                        			send(new_socket, &msg, sizeof(msg), 0); 
                        			wait(timeOut - dataDelay);
                        			cout << "Sender: Timeout (high delay) occured for packet sequence number " << packetNum << endl;   
                    			} 
                    			else {           //ACK received by sender
                        			send(new_socket, &msg, sizeof(msg), 0); 
			 			valread = read(new_socket, buffer, 1024); 
			 			wait(ackDelay);
			 			printf("Acknowledgement received from client\n");
			 			packetNum = (packetNum + 1);		
 		     			}
                    
                 
               		} 
                		else { //ACK lost
                    			send(new_socket, &msg, sizeof(msg), 0); 
                    			wait(timeOut-dataDelay);
                    			cout << "Sender: Timeout (ACK loss) occured for packet sequence number " << packetNum << endl; 
                		}
                
       		} 

        	} 
        	else { //data packet lost
            		wait(timeOut);
            		cout << "Sender: Timeout (packet loss) occured for packet sequence number " << packetNum << endl; 
        	}
        
    }
        
    // closing the connected socket 
    close(new_socket); 
    // closing the listening socket 
    shutdown(server_fd, SHUT_RDWR); 
    return 0; 
}





















