#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

//#define PORT 8080
#define BUFFER_SIZE 1024

float evaluate_expression(char *expression) {
    // Parse and evaluate the mathematical expression (You need to implement this part)
    // For simplicity, let's assume expressions are in the form "number operator number"
    float num1, num2, result;
    char operator;

    sscanf(expression, "%f %c %f", &num1, &operator, &num2);

    switch (operator) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        // case '^':
        //     result = pow(num1,num2);
        //     break;
        case '/':
            if (num2 != 0)
                result = num1 / num2;
            else {
                printf("Error: Division by zero\n");
                result = 0.0;
            }
            break;
        default:
            printf("Error: Invalid operator\n");
            result = 0.0;
    }

    return result;
}

int main(int argc,char *argv[]) // 1 arg reqd- port no
{
    if(argc<2) 
    {
        perror("Error: Port no not specified.");
        exit(1);
    }
    int PORT = atoi(argv[1]);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Read the expression from the client
        read(new_socket, buffer, BUFFER_SIZE);
        printf("Received expression: %s\n", buffer);
        
        if(atoi(buffer)==atoi("exit")) {goto Q;} //exit cmd

        // Evaluate the expression
        float result = evaluate_expression(buffer);

        // Convert the result to a string and send it to the client
        snprintf(buffer, BUFFER_SIZE, "%.2f", result);
        send(new_socket, buffer, strlen(buffer), 0);
        printf("Result sent: %.2f\n", result);

        
    }

    //close server socket
Q:    printf("Client wished to exit.\n");
    close(new_socket);

    return 0;
}
