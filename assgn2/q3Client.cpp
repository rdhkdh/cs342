#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <sys/socket.h>
#include <cstring>

using namespace std;

const int BUFFER_SIZE = 1024;
int PORT;
bool stop_client = false;

// Function to handle sending messages to the server
void handle_server(int client_fd)
{
    while (true)
    {
        string send_msg;
        getline(cin, send_msg);

        // Send the message to the server
        send(client_fd, send_msg.c_str(), send_msg.length(), 0);

        // Check if the user wants to exit
        if (send_msg == "exit")
        {
            stop_client = true;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    // Check the command line arguments
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <hostname>" << endl;
        return 1;
    }

    // Get the port number and IP address from the command line arguments
    char ip_addr_server[40];
    strcpy(ip_addr_server, argv[1]);
    PORT = stoi(argv[2]);

    int status, valread, client_fd;
    struct sockaddr_in serv_addr;

    // Create a socket
    char buffer[BUFFER_SIZE] = {0};
    if ((client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        cout << "Failed in socket creation.";
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip_addr_server, &serv_addr.sin_addr) <= 0)
    {
        cout << "Invalid address/ Address not supported";
        return 1;
    }

    if ((status = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        cout << "Connection failed";
        return 1;
    }

    // Create a thread to handle sending messages to the server
    thread t1(handle_server, client_fd);

    while (true)
    {
        memset(buffer, '\0', sizeof(buffer));
        valread = read(client_fd, buffer, BUFFER_SIZE);

        // Check if the server closed the connection
        if (valread == 0)
        {
            cout << "Connection closed\n";
            break;
        }

        cout << "SERVER: " << buffer << endl;

        // Check if the user wants to exit
        if (stop_client)
        {
            break;
        }
    }

    t1.detach();

    // Close the connected socket
    close(client_fd);
    return 0;
}
