#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

//#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc,char *argv[]) {

    if(argc < 3){ // if less than 2 args passed
		perror("Error: server or port number not passed.");
		exit(1);
	}
    int PORT = atoi(argv[2]); //2nd arg is port no
    char* localhost = argv[1]; //localhost = 127.0.0.1

    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, localhost, &server_address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        // Get user input for the expression
        printf("\nOperators: (+,-,*,/). Type 'exit' to exit.\n");
        printf("Enter the expression: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Send the expression to the server
        send(client_socket, buffer, strlen(buffer), 0);

        if(atoi(buffer)==atoi("exit")) {goto Q;} //check for exit cmd

        // Receive and print the result from the server
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        printf("Result: %s\n", buffer);
    }
    

    // Close the socket
Q:  printf("You wish to exit.\n");
    close(client_socket);

    return 0;
}
