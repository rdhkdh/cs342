#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<math.h>

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

int main(int argc, char *argv[]) // 1 arg reqd- port no
{
    if (argc < 2) {
        perror("Error: Port number not specified.");
        exit(1);
    }
    int PORT = atoi(argv[1]);

    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Receive data from client
        int bytes_received = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, &client_address_len);
        if (bytes_received < 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        printf("Received expression: %s", buffer);
        if(atoi(buffer)==atoi("exit")) {break;} //exit cmd

        // Evaluate the expression
        float result = evaluate_expression(buffer);

        // Convert the result to a string and send it to the client
        snprintf(buffer, BUFFER_SIZE, "%.2f", result);
        sendto(server_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&client_address, client_address_len);
        printf("Result sent: %.2f\n", result);
    }

    // Close the server socket
    printf("Client wished to exit.\n");
    close(server_socket);

    return 0;
}
