// TCP protocol server socket, multi-client
//Ridhiman Dhindsa, 210101088

#include<iostream>
#include<sys/socket.h> //for socket functions
#include<sys/types.h> //for socket functions
#include<netinet/in.h> //for IP related structs
#include<cstring>
#define PORT 9909
using namespace std;

struct sockaddr_in srv; //reserved struct for networking purposes

int main()
{
    int nRet = 0;

    //initialize socket
    int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(nSocket==-1)  { cout<<"Socket failed to open.\n"; exit(EXIT_FAILURE); }
    else{ cout<<"Socket opened succesfully with ID= "<<nSocket<<endl; }

    //initialize environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT); //host to network short
    srv.sin_addr.s_addr = INADDR_ANY; // assigning local machine's IP
    memset( &(srv.sin_zero), 0, 8 );

    //bind the socket to the local port
    nRet = bind(nSocket, (sockaddr*) &srv, sizeof(sockaddr));
    if(nRet==-1)  { cout<<"Failed to bind socket to local port.\n"; exit(EXIT_FAILURE); }
    else{ cout<<"Socket successfully bound to local port."<<endl; }

    //listen for the requests from client (queue the requests)
    nRet = listen(nSocket, 5);
    if(nRet==-1) { cout<<"Failed to start listening to local port.\n"; exit(EXIT_FAILURE); }
    else{ cout<<"Started listening on port "<<PORT<<"..."<<endl; }

    //keep waiting for new requests and process as per request

}