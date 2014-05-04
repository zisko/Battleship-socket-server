// Battleship network game by Jacob Mizraji.
// This is the Client.

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void recievedData(char buffer[40024], int usersockfd){

	//print out recieved data to the log
	printf("dealing with recievedData\n");
	printf("%s\n", buffer);

	//send back a welcome string.
	printf("data recieved:\n\n");

	// int pid = getpid();
     char* clientresponse = "response FROM CLIENT!!!!!";
    // asprintf(&welcomestring, "Welcome to the BattleShip Server. You are now player %i", pid);

   	// printf("----------%s\n", welcomestring);

    if (send(usersockfd, clientresponse, strlen(clientresponse), 0) == 0)
    	perror("sent 0 bytes.... why?");

}

void printUSAGEandexit(char* error){
	fprintf(stderr, "ERROR: at %s ; please follow directions.\n\n", error );
	printf("USAGE: you must supply these arguments:\n\n\t-p [port of the server {3000..5000}(defaults to localhost)]");
	exit(1);
}

int connectToServer(char *port, char* ipaddr){
	struct addrinfo *servinfo;
	struct addrinfo hints;
	struct sockaddr_storage user_addr;
	socklen_t user_addr_size;
	int errstat;
	char iptoprint[1028];
	memset(&hints, 0, sizeof hints);
	int sockfd, usersockfd;
	int errno = 0;
	int recvlength;
	char buffer[40024];

	hints.ai_family   = AF_UNSPEC;
	// hints.ai_flags    = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	printf("port/ ip trying: %s:%s\n", ipaddr, port);

	if((errstat = getaddrinfo(ipaddr, port, &hints, &servinfo)) != 0){
		fprintf(stderr,"error at getaddrinfo: %s",gai_strerror(errstat));
		exit(1);
	}
	else
		printf("socket structure set up successfully\n");


	inet_ntop(servinfo->ai_family, servinfo->ai_addr, iptoprint , INET_ADDRSTRLEN);


	printf("going to use this IP: %s, port: %s\n", iptoprint, port );

	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	// if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) != 0){
	// 	perror(strerror(errno));
	// 	exit(1);
	// }
	// else{
	// 	printf("socket bound like yeezy\n");
	// }

	// if(listen(sockfd, 5) != 0){
	// 	perror(strerror(errno));
	// }
	// else{
	// 	printf("Listening...\n");
	// }



	//only client code
	if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) != 0){
		perror(strerror(errno));
		exit(1);
	}
		user_addr_size = sizeof user_addr;


		send(sockfd, "hello", 5, 0);


	//while(1){
		usersockfd = accept(sockfd, (struct sockaddr*)&user_addr, &user_addr_size);

		inet_ntop(user_addr.ss_family, (struct sockaddr *)&user_addr, iptoprint, user_addr_size);
		printf("forking.....\n");
		int ChildPID = fork();

		if (!ChildPID) { // this is the child process

            close(sockfd); // child doesn't need the listener

            printf("process ID: %i\n",getpid());
            if((recvlength = recv(usersockfd, buffer, sizeof(buffer), 0)) != 0){
				 recievedData(buffer, usersockfd);
			}

			if((recvlength = recv(usersockfd, buffer, sizeof(buffer), 0)) != 0){
				 recievedData(buffer, usersockfd);
			}

            close(usersockfd);

            exit(0);
        }

        close(usersockfd);

//}

	return 0;
}

int main(int argc, char *argv[]) {
	int c;
	char* port;
	char* ipaddr;

	//get command line args, do appropriate thing.
	while((c = getopt(argc, argv, "s:p:")) != -1){
		switch(c){
			case 's':
				ipaddr = optarg;
				if (ipaddr){
					continue;
				}
				else
					printUSAGEandexit("IP address must be inputed");
				break;
			case 'p':
				port = optarg;
				if (atoi(port) >= 3000 && atoi(port) <= 5000)
				{
					printf("starting server port: %s\n", port);
					connectToServer(port, ipaddr);
				}
				else
					printUSAGEandexit("port number invalid");
				break;
			default:
				printUSAGEandexit("arguments");

		}
	}

}

