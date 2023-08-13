// TCP protocol server socket, multi-client
//Ridhiman Dhindsa, 210101088

#include<iostream>
#include<sys/socket.h> //for socket functions
#include<sys/types.h> //for socket functions
#include<sys/select.h> //fd_set class
#include<netinet/in.h> //for IP related structs
#include<cstring> //memset fn
#include<unistd.h> //sleep fn
#define PORT 9909
using namespace std;

struct sockaddr_in srv; //reserved struct for networking purposes
fd_set fr,fw,fe; //socket descriptors for read, write, exception
int nMaxFd, nSocket; 
int nArrClient[5]; //max 5 clients served. Can increase the no.

void ProcessNewMessage(int nClientSocket)
{
    cout<<"Processing new message for client socket: "<<nSocket<<endl;
    char buff[256+1]= {0,};
    int nRet = recv(nClientSocket,buff,256,0);
    if(nRet<0) 
    {
        cout<<"Something wrong happened...Closing client connection.\n";
        close(nClientSocket);
        for(int nIndex=0; nIndex<5; nIndex++)
        {
            if( nArrClient[nIndex]==nClientSocket )
            {
                nArrClient[nIndex]=0;
                break;
            }
        }
    }
    else
    {
        cout<<"The message received from client is: "<<buff<<endl;
        // send response to client
        send(nClientSocket,"Processed your request", 23,0);
        cout<<"--------------------------------------------------\n";
    }
}

void ProcessNewRequest()
{
    //new connection request
    if( FD_ISSET(nSocket,&fr) )
    {
        cout<<"Ready to read something. Something new came up at the port.\n";
        socklen_t nLen = sizeof(struct sockaddr);
        int nClientSocket = accept(nSocket,NULL,&nLen);
        if(nClientSocket>0) 
        {
            //put it into the client fd_set
            int nIndex;
            for(int nIndex=0; nIndex<5; nIndex++)
            {
                if(nArrClient[nIndex]==0) 
                { 
                    nArrClient[nIndex]= nClientSocket; 
                    send(nClientSocket, "Got the connection successfully.", 33, 0);
                    break; 
                }
            }
            if(nIndex==5) { cout<<"No space for new connections.\n"; }
        }
    }
    else // request from an already connected client
    {
        for(int nIndex=0; nIndex<5; nIndex++)
        {
            if( FD_ISSET(nArrClient[nIndex], &fr) )
            {
                //got a new message from client.
                //recv it, and queue it in server
                ProcessNewMessage(nArrClient[nIndex]);

            }
        }
    }
}

int main()
{
    int nRet = 0;

    // STEP 1- initialize socket
    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(nSocket==-1)  { cout<<"Socket failed to open.\n"; exit(EXIT_FAILURE); }
    else{ cout<<"Socket opened succesfully with ID= "<<nSocket<<endl; }

    // STEP 2- initialize environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT); //host to network short
    srv.sin_addr.s_addr = INADDR_ANY; // assigning local machine's IP
    memset( &(srv.sin_zero), 0, 8 );

    // Intermediate STEP- non blocking sockets
    // u_long optval = 0;
    // nRet = ioctlsocket(nSocket, FIONBIO, &optval);

    // setsockopt - winsoc

    // STEP 3- bind the socket to the local port
    nRet = bind(nSocket, (sockaddr*) &srv, sizeof(sockaddr));
    if(nRet==-1)  { cout<<"Failed to bind socket to local port.\n"; exit(EXIT_FAILURE); }
    else{ cout<<"Socket successfully bound to local port."<<endl; }

    // STEP 4- listen for the requests from client (queue the requests)
    nRet = listen(nSocket, 5); // max requests in queue= 5
    if(nRet==-1) { cout<<"Failed to start listening to local port.\n"; exit(EXIT_FAILURE); }
    else{ cout<<"Started listening on port "<<PORT<<"..."<<endl; }

    nMaxFd = nSocket;
    struct timeval tv;
    tv.tv_sec = 1; //time limit to check sockets are ready
    tv.tv_usec = 0;

    while(1)
    {
        FD_ZERO(&fr); //clearing all sockets
        FD_ZERO(&fw);
        FD_ZERO(&fe);

        FD_SET(nSocket,&fr);
        FD_SET(nSocket,&fe);

        //processing further requests - multiple clients
        for(int nIndex=0; nIndex<5; nIndex++)
        {
            if(nArrClient[nIndex]!=0)
            {
                FD_SET(nArrClient[nIndex], &fr);
                FD_SET(nArrClient[nIndex], &fe);
            }
        }

        //cout<<"\nBefore select call, fd value is= "<<*fr.fds_bits<<endl;

        // STEP 5- keep waiting for new requests and process as per request
        nRet = select(nMaxFd+1, &fr, &fw, &fe, &tv);
        if(nRet>0) 
        { //when someone connects or communicates with a msg over a dedicated connection
            cout<<"Data on port...Processing now...\n";
            //process the request
            if( FD_ISSET(nSocket, &fe) ) { cout<<"There is an exception in socket descriptor.\n"; }
            if( FD_ISSET(nSocket, &fw) ) { cout<<"Ready to write something.\n"; }
            // removed if condition on &fr
                 
                // accept the new connection or process request from already connected client
                ProcessNewRequest();
            
            //break; //problem
        }
        else if(nRet==0)
        {   //no connection or communication request made 
            //OR none of the socket descriptors are ready
            cout<<"Nothing on port: "<<PORT<<endl;
        }
        else
        { // It failed, and application should display error msg
            cout<<"Error: Process failed during STEP-5.\n";
            exit(EXIT_FAILURE);
        }
        //cout<<"After the select call, fd value is= "<<*fr.fds_bits<<"\n\n";
        sleep(2);
    }
    
}