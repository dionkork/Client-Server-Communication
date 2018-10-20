/*
############################## ERGASIA 3 ##################################
#									  #
#   A server for Statistics only program in which the server can 	  #
#   receive or send a number of messages with specific length. The	  #
#   sending and receiving time are calculated and then saved in 2 	  #
#   files: upload.txt and download.txt. 				  #
#                                                                         #
#   The TCP port is passed as argument.					  #
#							                  #
#   Korkouti Dionys 8748 				26/6/2018	  #
###########################################################################

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>


void error(char *msg)
{
        perror(msg);
        exit(1);
}

void my_write(int sock, char *buff, int length);
void my_read(int sock, char *buff, int length);


int sockfd, newsockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;
int n;

int main(int argc, char* argv[]) {
	//Make the connection possible.
	if (argc < 2) {
        	fprintf(stderr,"ERROR, no port provided\n");
        	exit(1);
  	}

  	sockfd = socket(AF_INET, SOCK_STREAM, 0);

  	if (sockfd < 0)
        	error("ERROR opening socket");
  	bzero((char *) &serv_addr, sizeof(serv_addr));
  	portno = atoi(argv[1]);
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = INADDR_ANY;
  	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");

	//Wait for a client to be connected.
  	listen(sockfd, 5);

	char buffer[256];
	char *bufferM;

	long long start;
	long long end;

	long long write_time;
        long long read_time;

	long long total;

	struct timeval time_start;
	struct timeval time_end;

	FILE *download;
	FILE *upload;

	clilen = sizeof(cli_addr);
    	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

	if (newsockfd < 0)
                error("ERROR on accept");


	//Here it starts!

	// Get selected choice
	my_read(newsockfd, buffer, 256);

	int choice = atoi(buffer);

	bzero(buffer,256);
	strcpy(buffer,"I got your choice!");
	my_write(newsockfd, buffer, sizeof(buffer));


	//Get the length of the message
	my_read(newsockfd, buffer, 256);

	int length=atoi(buffer);

	bzero(buffer, 256);
	strcpy(buffer,"I got the length!");
        my_write(newsockfd, buffer, sizeof(buffer));


	//Get the number of messages
	my_read(newsockfd, buffer, 256);

        int num=atoi(buffer);

	bzero(buffer, 256);
	strcpy(buffer,"I got the number!");
        my_write(newsockfd, buffer, sizeof(buffer));


	//Start
	printf("Length is : %d and num is: %d \n",length, num);

	int i, j;

	switch (choice){
		// The case 1 is for sending messages from client to server.
		case 1:

			upload = fopen("upload.txt", "a+");

			printf("Your choice is 1! \n");

			for(i=0; i<num; i++){


				bufferM=(char*)malloc(length*sizeof(char));


				//gettimeofday(&time_start, NULL);
				//start= (long long)time_start.tv_sec*1000000 + (long long)time_start.tv_usec;

				//Read the message.
				my_read(newsockfd, bufferM, length);

				gettimeofday(&time_end, NULL);

				end= (long long)time_end.tv_sec*1000000 + (long long)time_end.tv_usec;

				//read_time=end-start;


				bzero(buffer, 256);
				strcpy(buffer, "I got the message!");

				my_write(newsockfd, buffer, 256);

				free(bufferM);

				my_read(newsockfd, buffer, 256);

				//write_time=atoll(buffer);
				start=atoll(buffer);

				bzero(buffer,256);
				strcpy(buffer, "I got the time!");
				my_write(newsockfd, buffer, 256);

				printf(" Time is %lli\n",(end-start));
				//total=write_time+read_time;
				//Save the total time.
				total=end-start;

				fprintf(upload, "%lli \n" ,total);
			}

			fclose(upload);

			break;
		case 2:
			//The case 2 is for Downloading messages from server to client.
			printf("Your choice is 2! \n");

			download = fopen("download.txt", "a+");

			my_read(newsockfd, buffer, 256);

			for(i=0; i<num; i++){

				//Create the random string message.
       				bufferM=(char*)malloc(length*sizeof(char));
                		for (j=0; j<length; j++){
                    			bufferM[j]= 'A' + (random() % 26);
                		}

				//Get the start and end time for reading the message.
                		gettimeofday(&time_start, NULL);
                		start= (long long)time_start.tv_sec*1000000 + (long long)time_start.tv_usec;

                		my_write(newsockfd, bufferM, length);


				gettimeofday(&time_end, NULL);
				end= (long long)time_end.tv_sec*1000000 + (long long)time_end.tv_usec;

				//Save the result
				write_time=end-start;

                		my_read(newsockfd, buffer, 256);

                		free(bufferM);

                		bzero(buffer, 256);
                		sprintf(buffer, "%lli", start);

				//Send the time to client.
		               	my_write(newsockfd, buffer, 256);

                		my_read(newsockfd, buffer, 256);

				read_time=atoll(buffer);

				total=read_time+write_time;

				printf("Total time is :%lli \n", total);

				fprintf(download,"%lli \n", total);

	            	}

			fclose(download);
			break;
		default:

			printf("WRONG choice exiting... \n");
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

	printf("CLIENT: %s \n", buff);
}
