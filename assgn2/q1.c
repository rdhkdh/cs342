#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>

int main(int argc, char *argv[])
{
    char* hostname; //change this to cmd line arg later

    if(argc<2) {printf("Domain name not specified.\n"); return 0;}
    else{ hostname = argv[1]; }

    int error;
    
    struct addrinfo hints_1;
    struct addrinfo *res_1; //object ptr

    // setting all parameters of hints_1 to NULL
    memset(&hints_1, NULL, sizeof(hints_1));

    //port no 80 means requested service is http
    error = getaddrinfo(hostname, "80", &hints_1, &res_1); 
    if(error!=0) //error=0 means successful
    { 
        printf("Error: %i\n", error); 
        return 0;
    }

    unsigned char ip[50]= ""; //IP address string

    if(error==0)
    {
        // sa_data array contains the IP adrress info from elem 2-5.
        inet_ntop(AF_INET, &res_1->ai_addr->sa_data[2], ip, sizeof(ip));
        printf("Host name: %s\n", hostname);
        printf("IP address: %s\n", ip);
    }
}