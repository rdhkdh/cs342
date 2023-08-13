// TCP Protocol client socket
// Ridhiman Dhindsa, 210101088

#include<iostream>
#include<sys/socket.h> //for socket functions
#include<sys/types.h> //for socket functions
#include<sys/select.h> //fd_set class
#include<netinet/in.h> //for IP related structs
#include <arpa/inet.h> // inet_addr fn
#include<cstring> //memset fn
#include<unistd.h> //sleep fn
#define PORT 9909
using namespace std;

int nClientSocket;
struct sockaddr_in srv; //reserved struct for networking purposes

int main()
{
    int nRet = 0;

    // STEP 1- initialize socket
    nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(nClientSocket==-1)  { cout<<"Socket failed to open.\n"; exit(EXIT_FAILURE); }
    else{ cout<<"Socket opened succesfully with ID= "<<nClientSocket<<endl; }  

    // STEP 2- initialize environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT); //host to network short
    srv.sin_addr.s_addr = inet_addr("127.0.0.1"); // assigning local machine's IP
    memset( &(srv.sin_zero), 0, 8 );  

    //STEP 3- connect the client
    nRet = connect(nClientSocket, (struct sockaddr*) &srv, sizeof(srv));
    if(nRet<0) { cout<<"Client connection failed.\n"; exit(EXIT_FAILURE); }
    else
    { 
        cout<<"Connected to server."<<endl; 
        // talk to the client...
        char buff[255] = {0,};
        recv(nClientSocket, buff, 255, 0); //receive the msg
        cout<<"Press any key to view the message received from server.\n";
        getchar();
        cout<<buff<<endl;

        //send multiple messages
        cout<<"Now send your messages to the server:\n";
        while(1)
        {
            fgets(buff,256,stdin);
            send(nClientSocket,buff,256,0);
            cout<<"Press any key to get the response from server.\n";
            getchar();
            recv(nClientSocket,buff,256,0);
            cout<<buff<<endl<<"Now send next message: ";
        }
    }

}