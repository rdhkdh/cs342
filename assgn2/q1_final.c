//Group CS-18 RKD, HK, DP

#include<stdio.h> 
#include<string.h>    
#include<stdlib.h>    //malloc
#include<sys/socket.h>  
#include<arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include<netinet/in.h>
#include<unistd.h>    //getpid
#include<assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define DNS_PORT 53
#define DNS_SERVER "8.8.8.8"
#define CACHE_SIZE 5

// Creating a DNS header and question to send to Google DNS server
// all these elements use 16-bit unsigned integer as DNS Header requires it.
typedef struct {
  uint16_t xid;      // Randomly chosen identifier 
  uint16_t flags;    // Bit-mask to indicate request/response 
  uint16_t qdcount;  // Number of questions 
  uint16_t ancount;  // Number of answers 
  uint16_t nscount;  // Number of authority records 
  uint16_t arcount;  // Number of additional records 
} dns_header_t;

typedef struct {
  char *name;        // Pointer to the domain name in memory 
  uint16_t dnstype;  // The Question TYPE (1 = A) 
  uint16_t dnsclass; // The Question CLASS (1 = IN) 
} dns_question_t;

// Structure of the bytes for an IPv4 answer
typedef struct {
  uint16_t compression;
  uint16_t type;
  uint16_t class;
  uint32_t ttl;
  uint16_t length;
  struct in_addr addr;
} __attribute__((packed)) dns_record_a_t;

typedef struct {
    char hostname[100]; // The key (hostname)
    char ipv4_address[16]; // The value (IPv4 address)
} CacheEntry;

int main()
{
    CacheEntry cache[CACHE_SIZE]; 
    memset(cache, 0, sizeof(cache)); // Initialize all cache entries to zero

    printf("Type 'exit' to quit.\n");

    while(1) // Run program until user types 'exit'
    {
        unsigned char hostname[100];
        printf("Enter hostname for DNS lookup: ");
        scanf("%s",hostname);
        if(strcmp(hostname,"exit")==0) { printf("User wished to exit.\n"); exit(EXIT_SUCCESS); }

        //---------------caching-------------------------
        bool found_in_cache = false;
        char cached_ipv4_address[16];

        // Search the cache for the requested hostname
        for (int i = 0; i < CACHE_SIZE; i++) { 
            if (strcmp(cache[i].hostname, hostname) == 0) {
                found_in_cache = true;
                strcpy(cached_ipv4_address, cache[i].ipv4_address);
                break;
            }
        }

        if (found_in_cache) {
            // The IPv4 address is already in the cache, use it
            printf("IPv4 address (FROM CACHE): %s\n\n", cached_ipv4_address);
            continue; // Skip the DNS query
        }

        //-------------Set up the server address structure-------------------
        int socketfd = socket(AF_INET, SOCK_DGRAM, 0); //UDP protocol
        if(socketfd<0) {printf("Error in socket creation.\n"); exit(EXIT_FAILURE);}

        struct sockaddr_in address;
        address.sin_family = AF_INET; // address on internet IPv4
        address.sin_addr.s_addr = inet_addr(DNS_SERVER); // Google DNS server, 8.8.8.8
        address.sin_port = htons(DNS_PORT); // DNS runs on port 53

        //---------------Set up the DNS header-----------------
        dns_header_t header; 
        memset (&header, 0, sizeof (dns_header_t)); //set all elements to 0
        header.xid= htons (0x1234);    // Randomly chosen ID 
        header.flags = htons (0x0100); // Q=0, RD=1 
        header.qdcount = htons (1);    // Sending 1 question 

        //-------------Set up the DNS question------------------------
        dns_question_t question;
        question.dnstype = htons (1);  // QTYPE 1=A 
        question.dnsclass = htons (1); // QCLASS 1=IN 

        // DNS name format requires two bytes more than the length of the domain name as a string 
        question.name = calloc (strlen (hostname) + 2, sizeof (char));

        //-------Algorithm for converting a hostname string to DNS question fields-----------

        //Leave the first byte blank for the first field length
        memcpy (question.name + 1, hostname, strlen (hostname));
        uint8_t *prev = (uint8_t *) question.name;
        uint8_t count = 0; //Used to count the bytes in a field

        // Traverse through the name, looking for the (.) locations
        for(size_t i = 0; i < strlen(hostname); i++)
        {
            // A dot indicates the end of a field
            if (hostname[i] == '.')
            {
            /* Copy the length to the byte before this field, then
            update prev to the location of the . */
            *prev = count;
            prev = question.name + i + 1;
            count = 0;
            }
            else
            count++;
        }
        *prev = count;

        //---------Assembling the DNS header and question to send via a UDP packet------------

        // Copy all fields into a single, concatenated packet 
        size_t packetlen = sizeof(header) + strlen(hostname) + 2 + sizeof(question.dnstype) + sizeof(question.dnsclass);
        uint8_t *packet = calloc (packetlen, sizeof (uint8_t));
        uint8_t *p = (uint8_t *)packet;

        // Copy the header first 
        memcpy (p, &header, sizeof (header));
        p += sizeof (header);

        // Copy the question name, QTYPE, and QCLASS fields
        memcpy (p, question.name, strlen (hostname) + 1);
        p += strlen (hostname) + 2; // includes 0 octet for end
        memcpy (p, &question.dnstype, sizeof (question.dnstype));
        p += sizeof (question.dnstype);
        memcpy (p, &question.dnsclass, sizeof (question.dnsclass));

        // Send the packet to Google DNS server, then request the response 
        int x= sendto (socketfd, packet, packetlen, 0, (struct sockaddr *) &address, (socklen_t) sizeof (address));
        if(x<0) {printf("sendto failed\n"); return 0;}
        
        //-----Receiving a DNS header and confirming there were no errors------------------
        socklen_t length = 0;
        uint8_t response[512];
        memset (&response, 0, 512);

        // Receive the response from Google DNS server into a local buffer 
        ssize_t bytes = recvfrom (socketfd, response, 512, 0, (struct sockaddr *) &address, &length);

        //-------------Checking the header and question name of the DNS response--------------  

        dns_header_t *response_header = (dns_header_t *)response;
        assert ((ntohs (response_header->flags) & 0xf) == 0); //converting data from big-endian to little-endian

        /* Get a pointer to the start of the question name, and
        reconstruct it from the fields */
        uint8_t *start_of_name = (uint8_t *) (response + sizeof (dns_header_t));
        uint8_t total = 0;
        uint8_t *field_length = start_of_name;
        while (*field_length != 0)
        {
            /* Restore the dot in the name and advance to next length */
            total += *field_length + 1;
            *field_length = '.';
            field_length = start_of_name + total;
        }

        //-----------Printing the DNS resource records returned--------------------------
        char* ipv4_addr;

        // Skip null byte, qtype, and qclass to get to first answer
        dns_record_a_t *records = (dns_record_a_t *) (field_length + 5);
        //print IP address returned for every answer RR
        for (int i = 0; i < ntohs (response_header->ancount); i++)
        {
            printf ("IPv4 address after lookup: %s\n", inet_ntoa (records[i].addr));
            if(i==0) 
            {
                ipv4_addr = inet_ntoa (records[i].addr);
            }
        }
        printf("\n");

        // After receiving the DNS response, update cache with the new entry
        for (int i = 0; i < CACHE_SIZE; i++) {
            if (cache[i].hostname[0] == '\0') { // Find an empty slot in the cache
                strcpy(cache[i].hostname, hostname);
                strcpy(cache[i].ipv4_address, ipv4_addr);
                break;
            }
        }

    }//end of while loop

}// end of main
