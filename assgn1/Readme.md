# Assignment 1
Ridhiman Kaur Dhindsa, 210101088  
14 August 2023

## Ques 1
Client can send messages to server in Base64 encoding. Server will decode the client's message and print it. Then it will send an acknowledgement message to client. This way client can send multiple messages, until it decides to exit. On receiving exit message from client, server exits gracefully. Connections are made using TCP sockets.
3 types of messages used:  
1) Regular message (Client->Server)  
2) Acknowledgemnt message (Server->client)  
3) Exit message (client->server)

### Steps for execution
1) Open linux terminal in folder containing the files.  
2) Compile server file using command ```g++ -o q1_server q1_server.cpp```  
3) Compile client file using command ```g++ -o q1_client q1_client.cpp```
4) Start server using command ```./q1_server  <port_number>```  
5) Start client using command ```./q1_client  <local_host>   <port_number>```

> eg: ```./q1_server  9090```  
```./q1_client 127.0.0.1 9090```

## Ques 2
A server-client chat through which both server and client can send messages to each other. 
The messages are displayed on respective terminals. Connections are made using TCP sockets. 
The communication process gracefully exits when 'exit' is written at either side. It also 
displays the time duration of the process and bytes used.

### Steps for execution
1) Open linux terminal in folder containing the files.  
2) Compile server file using command ```g++ -o q2_server q2_server.cpp```  
3) Compile client file using command ```g++ -o q2_client q2_client.cpp```
4) Start server using command ```./q2_server  <port_number>```  
5) Start client using command ```./q2_client  <local_host>   <port_number>```

> eg: ```./q2_server  9090```  
```./q2_client 127.0.0.1 9090```

## Ques 3
A network based calculator wherein a client sends expressions to a server. The server calculates 
the value and sends it back to the client, which displays it. Following functions are supported: +,-,*,/,^.
Separate programs for TCP and UDP protocols have been written. The connection is closed when -1 is entered 
by client.

### Steps for execution (TCP program):
1) Open linux terminal in folder containing the files.  
2) Compile server file using command ```g++ -o q3_serverTCP q3_serverTCP.cpp```  
3) Compile client file using command ```g++ -o q3_clientTCP q3_clientTCP.cpp```
4) Start server using command ```./q3_serverTCP  <port_number>```  
5) Start client using command ```./q3_clientTCP  <local_host>   <port_number>```

> eg: ```./q3_serverTCP  9090```  
```./q3_clientTCP 127.0.0.1 9090```

### Steps for execution (UDP program):
1) Open linux terminal in folder containing the files.  
2) Compile server file using command ```g++ -o q3_serverUDP q3_serverUDP.cpp```  
3) Compile client file using command ```g++ -o q3_clientUDP q3_clientUDP.cpp```
4) Start server using command ```./q3_serverUDP  <port_number>```  
5) Start client using command ```./q3_clientUDP  <local_host>   <port_number>```

> eg: ```./q3_serverUDP  9090```  
```./q3_clientUDP 127.0.0.1 9090```