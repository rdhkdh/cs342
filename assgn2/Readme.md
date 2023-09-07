# Networks Assignment 2

## Question 1
Implemented a DNS lookup program in C, which takes user input of hostname and gives 
output of IPv4 address. The program runs by establishing a UDP connection to Google's open source DNS server 
'8.8.8.8', port 53. It is implemented using socket programming in Linux. It sends a DNS query packet to the server following the DNS message format.
Then parses the DNS response packet and extracts the IP address(es) associated with the provided
domain name.

#### To-do:
* Explain the code here as well as commenting
* Do caching part

### Steps for execution:
1. Compile in Linux terminal: ```gcc q1_website.c -o q1_website```
2. Then run the file: ```./q1_website```
3. The program will prompt you to enter hostname. Enter a host name such as 'www.google.com', 'www.github.com', 'www.linuxhints.com' etc. It will output the IP address of the corresponding host name.

> NOTE: This will work only if the connection is through mobile networks, and not IITG 
network. For IITG network, change DNS_SERVER value to '172.17.1.1' in the code.

## Question 2
Implemented a web cache (proxy server) in C.

## Question 3


## Question 4