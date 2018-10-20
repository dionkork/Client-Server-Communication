/*
############################## ERGASIA 3 ##############################
# 								      #
#   A client program in which the user can send and receive messages  #
#   from a server. The user types his ip and can choose an option to  #
#   send or receive. Then he will be informed about his messages or   #
#   he will be asked to type the receiver's ip address.		      #
#                                                                     #
#   The IP and TCP port are passed as arguments.		      #
#								      #
#   Korkouti Dionys 8748 			26/6/2018	      #
#######################################################################
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>

// Error Control Function
void error(char *msg){
	perror(msg);
  	exit(0);
}

// Main Function. It is required IP and PORT as arguments.
int main(int argc, char *argv[]){

	int sockfd, portno, n;

  	struct sockaddr_in serv_addr;
  	struct hostent *server;

	char bufferm[512];
  	char buffer[256];
  	char bufferip[64];

	long long start;
	long long end;
	struct timeval time_end;
	struct timeval time_start;

	//Check the arguments.
  	if (argc < 3) {

		fprintf(stderr,"usage %s hostname port\n", argv[0]);
   		 exit(0);
  	}
	//Save port number and create a socket.
	portno = atoi(argv[2]);
  	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
		error("ERROR opening socket");
  	server = gethostbyname(argv[1]);

	if (server == NULL) {
    		fprintf(stderr,"ERROR, no such host\n");
    		exit(0);
  	}

	//Important commands to make the connection possible.
  	bzero((char *) &serv_addr, sizeof(serv_addr));
  	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr,
        	(char *)&serv_addr.sin_addr.s_addr,
        	server->h_length);

	serv_addr.sin_port = htons(portno);

	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    		error("ERROR connecting");

	//Conncetion Complete
	printf("######## Successfull connection to the server! ########\n \n");

	// Get the Ip Address from user and send it to the server.
  	printf("Please enter your ip : ");

  	bzero(bufferip,64);
  	fgets(bufferip,63,stdin);
  	n = write(sockfd,bufferip,strlen(bufferip));

  	if (n < 0)
    		error("ERROR writing to socket");
	//Wait for a server response.
  	bzero(bufferip, 64);
  	bzero(buffer, 256);
  	n = read(sockfd,buffer,255);

  	if (n < 0)
    		error("ERROR reading from socket");
  	printf("SERVER: %s \n",buffer);

	//Send to the server the user's choice.
  	printf("\n1: Send a message \n2: Receive a message \nPlease enter your choice: ");

  	bzero(bufferip,64);
  	fgets(bufferip,63,stdin);
  	n = write(sockfd,bufferip,strlen(bufferip));

  	if (n < 0)
    		error("ERROR writing to socket");

  	bzero(buffer, 256);
  	n = read(sockfd,buffer,255);

  	if (n < 0)
    		error("ERROR reading from socket");
  	printf("\nSERVER: %s \n",buffer);

  	int choice=atoi(bufferip);

	//printf("Your Choice is %d \n", choice);

	//Case 1: Send a Message
	//Case 2: Receive Messages.
	switch (choice) {

		case 1 :

			printf("\n######## SEND MESSAGE ########\n \n");
			// Get the Receiver's ip address and send it to server.

  			printf("Please enter the receivers ip : ");

  			bzero(bufferip,64);
  			fgets(bufferip,63,stdin);
  			n = write(sockfd,bufferip,strlen(bufferip));

  			if (n < 0)
    				error("ERROR writing to socket");
			//Wait for server response.
  			bzero(buffer,256);
  			n = read(sockfd,buffer,255);

  			if (n < 0)
    				error("ERROR reading from socket");

			printf("SERVER: %s \n",buffer);

			//Get and Send the user's message!

  			printf("\nPlease enter your message : ");

  			bzero(buffer,256);
  			fgets(buffer,255,stdin);
  			n = write(sockfd,buffer,strlen(buffer));

  			if (n < 0)
    				error("ERROR writing to socket");
			//Wait for response.
  			bzero(buffer,256);
  			n = read(sockfd,buffer,255);

  			if (n < 0)
    				error("ERROR reading from socket");
  			printf("SERVER: %s \n",buffer);

			break;

		case 2 :

			printf("\n######## RECEIVE MESSAGES ########\n \n");
			//Get the time for system response statistics.

			gettimeofday(&time_start, NULL);
		        start=(long long)time_start.tv_sec*1000000+(long long)time_start.tv_usec;

			//Receive the Message from Server
			bzero(buffer,256);
            		n = read(sockfd,buffer,256);

            		if (n < 0)
                		error("ERROR reading from socket");

			printf("SERVER: The New Messages are: %s \n",buffer);

			int numm=atoi(buffer);

			printf("\n");

			if (numm==0){

				printf("No new messages! Sorry :( \n");

			}else{
				bzero(buffer,256);
                		n=write(sockfd,"I got the Number of messages!",29);

                		if (n < 0)
                    		error("ERROR writing to socket");

				//Receive all the messages.
				for(int t=0; t<numm; t++){

					bzero(bufferm,512);
	               			n = read(sockfd,bufferm,511);

        	       			if (n < 0){
                       				error("ERROR reading from socket");
					}

					//gettimeofday(&time_end, NULL);
					//end=(long long)time_end.tv_sec*1000000+(long long)time_end.tv_usec;


					printf("SERVER| %s\n",bufferm);

					bzero(buffer,256);
					n=write(sockfd,"I got the message!",18);

					if (n < 0){
                                		error("ERROR writing to socket");
					}

					//Get time from server

				/*	bzero(buffer, 256);
        				n=read(sockfd, buffer, 255);

        				if (n < 0)
                				error("ERROR reading from socket");
        				printf("SERVER: Time is : %s \n", buffer);

					long long start =atoll(buffer);

					//Reply to server
					bzero(buffer,256);
                    			n=write(sockfd,"I got the time!",15);

                    			if (n < 0){
                        		error("ERROR writing to socket");
                    			}

					printf("End time is : %lli \n", end);
					printf("Total time is : %lli\n", (end-start) );
				*/
				}

				printf("No more messages! \n");
			}
			// Get the end time for system response statistics.
			gettimeofday(&time_end, NULL);
            		end=(long long)time_end.tv_sec*1000000+(long long)time_end.tv_usec;

			printf("\nTOTAL Server Response Time is : %lli \n", (end-start) );

			break;
		default:

			printf("Error with your choice! \n");

	}

	//Wait for <<Connection Complete>> from Server
	bzero(buffer, 256);
	n=read(sockfd, buffer, 255);

	if (n < 0)
		error("ERROR reading from socket");
	printf("SERVER: %s \n", buffer);

   return 0;
}
