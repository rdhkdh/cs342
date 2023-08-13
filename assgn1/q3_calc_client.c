#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#define BUFF_SIZE 256

void error(const char *msg){
	perror(msg);
	exit(1);
}

int main(int argc,char *argv[]) // localhost, port no args
{
	int sockfd,portno,n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[BUFF_SIZE]={0};
	if(argc < 3){ // if less than 2 args passed
		fprintf(stderr,"usage %s hotname port\n",argv[0]);
		exit(1);
	}

	portno = atoi(argv[2]); //2nd arg is port no
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd < 0){
		error("Error opening socket.");
	}

	server = gethostbyname(argv[1]); // 1st arg is localhost- 127.0.0.1
	if(server == NULL){
		fprintf(stderr,"No such host.\n");
        exit(0);
	}
	bzero((char *) &serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr_list[0], (char *) &serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	if(connect(sockfd, (struct sockaddr *) &serv_addr , sizeof(serv_addr)) < 0){
		error("Connection Failed !!");
	}
  
	printf("Enter expression: ");
	fgets(buffer, BUFF_SIZE, stdin);
	send(sockfd,buffer,BUFF_SIZE,0); //send exp to server

	memset(buffer, 0, BUFF_SIZE);
    recv(sockfd, buffer, BUFF_SIZE, 0); //receive and print result
    printf("Result from server: %s\n", buffer);

	//int num1,num2,choice,ans;

// 	//Reading Server String 
// S:      bzero(buffer,256);
// 	num1 = read(sockfd,buffer,256);
// 	if(num1 < 0){
// 		error("Error reading From Socket");
// 	}

// 	printf("Server - %s\n",buffer);
// 	scanf("%d",&num1);
// 	write(sockfd,&num1,sizeof(int));

// 	bzero(buffer,256);
//         num2 = read(sockfd,buffer,256);
//         if(num2 < 0){
//                 error("Error reading From Socket");
//         }

//         printf("Server - %s\n",buffer);
//         scanf("%d",&num2);
//         write(sockfd,&num2,sizeof(int));

// 	bzero(buffer,256);
//         choice = read(sockfd,buffer,256);
//         if(choice < 0){
//                 error("Error reading From Socket");
//         }

//         printf("Server - %s\n",buffer);
//         scanf("%d",&choice);
//         write(sockfd,&choice,sizeof(int));

// 	if(choice == 5){
// 		goto Q;
// 	}

	// read(sockfd,&ans,sizeof(int));
	// printf("Server : The answer is : %d\n",ans);

	// if(choice != 5){
	// 	goto S;
	// }

// Q:  printf("You Have Selected to exit\nExit Successfully");
// 	close(sockfd);
// 	return 0;

}
