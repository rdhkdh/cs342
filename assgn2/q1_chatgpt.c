//not working at all

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DNS_SERVER "8.8.8.8" //change to "8.8.8.8" for your net, "172.17.1.1" for IITG net
#define DNS_PORT 53

// Structure for the DNS header
struct DNSHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

int main() {
    int sockfd; //socket file descriptor
    struct sockaddr_in server_addr;
    char query[256];

    // Prompt the user for a hostname
    printf("Enter a hostname (e.g., www.example.com): ");
    fgets(query, sizeof(query), stdin);
    query[strcspn(query, "\n")] = '\0'; // Remove newline character

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    else{printf("UDP socket created succesfully.\n");}

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr)); //set all parameters to 0
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DNS_PORT);
    if (inet_pton(AF_INET, DNS_SERVER, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(1);
    }
    else{printf("Successfully converted DNS server IP address from presentation to numeric format.\n");}

    // Send the DNS query to the server
    if (sendto(sockfd, query, strlen(query), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("sendto");
        close(sockfd);
        exit(1);
    }
    else{printf("DNS query sent successfully.\n");}

    // Receive the DNS response
    char response[65536];
    socklen_t server_addr_len = sizeof(server_addr);
    ssize_t bytes_received = recvfrom(sockfd, response, sizeof(response), 0, (struct sockaddr *)&server_addr, &server_addr_len);
    if (bytes_received < 0) {
        perror("recvfrom");
        printf("Error receiving response.\n");
        close(sockfd);
        exit(1);
    }
    else{printf("Response received successfully.\nBytes recived: %ld\n",bytes_received);}

    // Parse the DNS response to extract the IP address (same code as in the previous answer)
    printf("Response: %s\n",response);

    // Assuming you have received the DNS response in the 'response' buffer

    // Parse the DNS response to extract the IP address
    struct DNSHeader *dns_header = (struct DNSHeader *)response;
    uint16_t answer_count = ntohs(dns_header->ancount);

    printf("Answer count: %d\n",answer_count);
    if (answer_count > 0) {
        // Move to the answer section of the response
        char *answer_section = response + sizeof(struct DNSHeader);

        // Skip the question section
        for (int i = 0; i < ntohs(dns_header->qdcount); i++) {
            while (*answer_section != '\0') {
                answer_section++;
            }
            answer_section += 5; // Skip the type and class fields
        }

        // Extract IP addresses from the answer section
        for (int i = 0; i < answer_count; i++) {
            if (answer_section[1] == 0x01) { // Check for IPv4 address type (A record)
                char ip_address[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &answer_section[12], ip_address, INET_ADDRSTRLEN);
                printf("IPv4 Address: %s\n", ip_address);
            }
            // Add additional checks for IPv6 (AAAA record) if needed

            // Move to the next answer in the response
            answer_section += 16; // Move to the next answer
        }
    } 
    else {
        printf("No answers found in the DNS response.\n");
    }


    
    // Close the socket
    close(sockfd);

    return 0;
}
