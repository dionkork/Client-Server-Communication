/*
############################# Statistics  #############################
# 					                              #
#   A client for Statistics only program in which the user can type   #
#   the number of messages he wish to receive or send and the length  #
#   of each message. The message is a random string which is created  #
#   at the begining. The times of download and upload are stored in   #
#   the server_Statistics program.				      #
#                                                                     #
#	The IP and TCP port are passed as arguments.		      #
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

void error(char *msg){
        perror(msg);
        exit(0);
}

//My own write and read function for a "cleaner" code.
void my_write(int sock, char *buff, int length);
void my_read(int sock, char *buff, int length);
int n;

//Main function
int main(int argc, char *argv[]){

	// Variables for times, sockets, and buffers.
    	int sockfd, portno;

    	struct sockaddr_in serv_addr;
    	struct hostent *server;

	char buffer[256];
	char *bufferM;

	long long start;
	long long end;

	long long read_time;
    	long long write_time;

	long long total;

	struct timeval time_start;
    	struct timeval time_end;

	int length, num;

	//Check the number of arguments.
	if (argc < 3) {

        	fprintf(stderr,"usage %s hostname port\n", argv[0]);
         	exit(0);
    	}

	//Make the Connection to the server.
    	portno = atoi(argv[2]);
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    	if (sockfd < 0)
		error("ERROR opening socket");

	server = gethostbyname(argv[1]);

	if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	exit(0);
    	}

    	bzero((char *) &serv_addr, sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;

    	bcopy((char *)server->h_addr,
     		(char *)&serv_addr.sin_addr.s_addr,
      		server->h_length);

    	serv_addr.sin_port = htons(portno);

    	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        	error("ERROR connecting");

	//Here it starts!

	printf("1: Send message speed \n");
	printf("2: Receive message speed \n");

    	bzero(buffer,256);
    	fgets(buffer,255,stdin);

	int choice=atoi(buffer);

	//Send Choice

	my_write(sockfd, buffer, strlen(buffer));

	my_read(sockfd, buffer, 256);

	//Get and send the length of the messages
	do {

		printf("Please enter the length of messages! \n");
		bzero(buffer,256);
        	fgets(buffer,255,stdin);

		length=atoi(buffer);

		if ((length<=0)||(length>1024)){
			printf("WRONG input! Length must be: 0<L<=1024 \n");
		}

	}while ((length<=0)||(length>1024));

	//Send the length to the server.
	my_write(sockfd, buffer, strlen(buffer));

	my_read(sockfd, buffer, 256);

	//Get and send Number of messages

	do {

        	printf("Please enter the number of messages! \n");
        	bzero(buffer,256);
        	fgets(buffer,255,stdin);

        	num=atoi(buffer);

        	if ((num<=0)||(num>100000)){
         	   	printf("WRONG input! Number must be: 0<N<=10.000 \n");
        	}

    	}while ((num<=0)||(num>100000));

	//Send the number of messages to the Server
	my_write(sockfd, buffer, strlen(buffer));


    	my_read(sockfd, buffer, 256);


	printf("Length is : %d and num is: %d \n",length, num);


	//Start
	int i, j;

	switch (choice){
			//Send Messages Option.
		case 1:
			printf("\nYour choice is 1! \n");

			for(i=0; i<num; i++){
				//Create the random message.
				bufferM=(char*)malloc(length*sizeof(char));
				for (j=0; j<length; j++){
					bufferM[j]= 'A' + (random() % 26);
				}

				gettimeofday(&time_start, NULL);

        		        start= (long long)time_start.tv_sec*1000000 + (long long)time_start.tv_usec;
				//Send the start time to the server.
				my_write(sockfd, bufferM, length);

				//gettimeofday(&time_end, NULL);

				//end= (long long)time_end.tv_sec*1000000 + (long long)time_end.tv_usec;
				//Wait for feedback.
				my_read(sockfd, buffer, 256);

				free(bufferM);

				//write_time=end-start;

				bzero(buffer, 256);
				//sprintf(buffer, "%lli", write_time);
				//Print the result.
				sprintf(buffer, "%lli", start);
				my_write(sockfd, buffer, 256);

				my_read(sockfd, buffer, 256);
			}

			break;
		case 2:
			//Receive Messages Option.
			printf("Your choice is 2! \n");

			bzero(buffer, 256);
            		strcpy(buffer, "Checking download speed...");
            		my_write(sockfd, buffer, strlen(buffer));


			for(i=0; i<num; i++){

				//Count the time that the my_read will execute and send the time to the server.
				bufferM=(char*)malloc(length*sizeof(char));

				gettimeofday(&time_start, NULL);

				start= (long long)time_start.tv_sec*1000000 + (long long)time_start.tv_usec;

		                my_read(sockfd, bufferM, length);

                		gettimeofday(&time_end, NULL);

                		end= (long long)time_end.tv_sec*1000000 + (long long)time_end.tv_usec;

				//The execution time of my_read
				read_time=end-start;

                		bzero(buffer, 256);
                		strcpy(buffer, "I got the message!");
				//Send Feedback
                		my_write(sockfd, buffer, 256);

                		free(bufferM);

                		my_read(sockfd, buffer, 256);

                		//start=atoll(buffer);

                		bzero(buffer,256);
				sprintf(buffer, "%lli", read_time);
				//Resault
				my_write(sockfd, buffer, 256);

                		//printf("\nTime is : %lli\n",(end-start));
            		}

			break;

		default:

			printf("WRONG choice exiting ... \n");
			exit(1);
	}

	return 0;

}
//My_Write function with the error handling
void my_write(int sock, char *buff, int length){

        n = write(sock, buff, length);

        if (n < 0) error("ERROR writing to socket");

}
//My_Read function with the error handling.
void my_read(int sock, char *buff, int length){

        bzero(buff, length);

        n = read(sock, buff, length);

        if (n < 0) error("ERROR reading from socket");

        printf("SERVER: %s \n", buff);
}



