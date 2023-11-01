#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <sys/socket.h>
#include <cstring>

using namespace std;

int PORT;
const int BUFFER_SIZE = 1024;
bool stop_server = false;
int main_server_socket;
int addrlen, new_socket, client_socket[30], max_clients = 30, activity, socket_descriptor;
int max_socket_descriptor;

// Function to handle messages from the server administrator
void handle_client()
{
    while (true)
    {
        string option;
        getline(cin, option);

        // Check if the administrator wants to stop the server
        if (option == "exit")
        {
            stop_server = true;
            break;
        }
        else
        { // Convert the input to a socket descriptor
            int socket_descriptor = stoi(option);

            string response;
            response = "";

            // Prompt the administrator for a response to send to the client
            cout << "Enter your response to client " << socket_descriptor << ": ";
            getline(cin, response);

            // Send the response to the specified client
            send(socket_descriptor, response.c_str(), BUFFER_SIZE, 0);
        }
    }
}

int main(int argc, char *argv[])
{
    // Check the command line arguments
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }

    // Get the port number from the command line arguments
    PORT = stoi(argv[1]);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    char buffer[BUFFER_SIZE];

    fd_set readfds;

    // Initialize the array of client socket descriptors to 0
    for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // Create the main server socket
    if ((main_server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
        cerr << "Failed to create master socket";
        return 1;
    }

    int opt = 1;
    // Set socket options to allow reuse of the address
    if (setsockopt(main_server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        cerr << "setsockopt";
        return 1;
    }

    // Bind the socket to the specified server address and port
    if (bind(main_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "Failed to bind socket" << endl;
        return 1;
    }

    cout << "Server listening on port: " << PORT << endl;

    // Listen for incoming connections
    if (listen(main_server_socket, 3) < 0)
    {
        cerr << "Failed to listen on socket" << endl;
        return 1;
    }

    addrlen = sizeof(server_addr);
    puts("Waiting for connections...");

    // Create a thread to handle administrator input
    thread t2(handle_client);

    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(main_server_socket, &readfds);
        max_socket_descriptor = main_server_socket;

        for (int i = 0; i < max_clients; i++)
        {
            socket_descriptor = client_socket[i];

            if (socket_descriptor > 0)
                FD_SET(socket_descriptor, &readfds);

            if (socket_descriptor > max_socket_descriptor)
                max_socket_descriptor = socket_descriptor;
        }

        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        // Use the select function to monitor socket activity
        activity = select(max_socket_descriptor + 1, &readfds, NULL, NULL, &tv);

        if ((activity < 0) && (errno != EINTR))
        {
            cerr << "Select error";
        }

        // Check if there is incoming activity on the main server socket
        if (FD_ISSET(main_server_socket, &readfds))
        {
            if ((new_socket = accept(main_server_socket, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen)) < 0)
            {
                cerr << "Failed to accept";
                return 1;
            }

            // Inform about the new connection
            cout << "\nNew connection: " << endl
                 << "Socket fd: " << new_socket << endl
                 << "IP: " << inet_ntoa(server_addr.sin_addr) << endl
                 << "Port: " << ntohs(server_addr.sin_port) << endl
                 << endl;

            // Send a welcome message to the new client
            char message[50] = "Welcome, new client! How can I help you?";
            if (send(new_socket, message, strlen(message), 0) != strlen(message))
            {
                cerr << "Failed to send message to client";
            }

            // Add the new socket to the array of client sockets
            for (int i = 0; i < max_clients; i++)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    cout << "Adding to list of sockets as " << i << endl;
                    break;
                }
            }
        }

        // Handle IO operations on existing client sockets
        for (int i = 0; i < max_clients; i++)
        {
            socket_descriptor = client_socket[i];

            if (FD_ISSET(socket_descriptor, &readfds))
            {
                memset(buffer, '\0', BUFFER_SIZE);
                int valread = read(socket_descriptor, buffer, BUFFER_SIZE);

                // Check if the client sent an "exit" message
                if (strcmp(buffer, "exit") == 0)
                {
                    // Get client details and inform about the disconnection
                    getpeername(socket_descriptor, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen);
                    cout << "Client " << socket_descriptor << " disconnected" << endl
                         << "IP: " << inet_ntoa(server_addr.sin_addr) << endl
                         << "Port: " << ntohs(server_addr.sin_port)
                         << endl;

                    // Close the socket and mark it as 0 in the list for reuse
                    close(socket_descriptor);
                    client_socket[i] = 0;
                }

                string broadcast = "broadcast: ";
                string priv_message = "private message to client ";
                if (!broadcast.compare(0, broadcast.length(), buffer, broadcast.length()))
                {
                    string response = "Client " + to_string(socket_descriptor) + " wants to " + buffer;
                    cout << response << endl;
                    for (int receiver_clients = 0; receiver_clients < max_clients; receiver_clients++)
                    {
                        send(client_socket[receiver_clients], response.c_str(), BUFFER_SIZE, 0);
                    }
                }

                else if (!priv_message.compare(0, priv_message.length(), buffer, priv_message.length()))
                {
                    int rec_client_fd = buffer[priv_message.length()] - '0';
                    if (buffer[priv_message.length() + 1] != ':')
                    {
                        rec_client_fd *= 10;
                        rec_client_fd += buffer[priv_message.length() + 1] - '0';
                    }

                    if (rec_client_fd >= 4 && client_socket[rec_client_fd - 4])
                    {
                        string response = "From client " + to_string(socket_descriptor) + ", " + buffer;
                        cout << response << endl;
                        send(rec_client_fd, response.c_str(), BUFFER_SIZE, 0);
                    }
                    else
                    {
                        cout << "Client " << rec_client_fd << " is not connected to server." << endl;
                    }
                }

                else if (strcmp(buffer, "exit") != 0)
                {
                    // Echo back the message from the client
                    buffer[valread] = '\0';
                    cout << "CLIENT (SOCKET FD " << socket_descriptor << "): " << buffer << endl;
                }
            }
        }

        // Check if the server administrator wants to stop the server
        if (stop_server)
        {
            break;
        }
    }

    // Detach the thread and shut down the main server socket
    t2.detach();
    shutdown(main_server_socket, SHUT_RDWR);
    return 0;
}
