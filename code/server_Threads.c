/*
############################## ZSUN COMM ##############################
# 								      #
#   A server program in which the program can receive a number of     #
#   clients which is defined by MAX_CLIENTS. Each client can send     #
#   and receive messages to and from the server. The server uses      #
#   the TCP protocol for communications. Each client can be served    #
#   at the same time which depends on value MAX_CLIENTS. The server   #
#   can save the messages and return them uppon request. In order     #
#   to return the message the server requirs an ip from the client.   #
#								      #
#   The TCP port is need as an argument.			      #
#   								      #
#   Korkouti Dionys 8748 			26/6/2018	      #
#######################################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
//#include <sys/time.h>

#define MAX_CLIENTS 64
#define MAX_MESSAGES_NUM 100000

//The message struct. Contain info from sender, receiver, the message and the message_id
struct messages {
	int message_id;
	char sender[64];
	char receiver[64];
	char message[256]
};

//Error handling function
void error(char *msg)
{
	perror(msg);
  	exit(1);
}

//Interupt handler.
void exithandler(int);


int counter = 0;
int rep;

typedef struct {
  	int id;
} parm;


int sockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;
int n;

//Mutex lock, and condition
pthread_mutex_t lock;
pthread_cond_t cv;


struct messages M[MAX_MESSAGES_NUM];
int j=0;
int number_of_connected_clients=0;

//The function which is use by threads. Does all the work.
void *work(void *arg) {
	parm *p = (parm *)arg;

	int myj;
	char buffer[256];
	char bufferip[64];
	char bufferm[512];
	int newsockfd;

	long long start;
	long end;

	struct timeval time_start;
	struct timeval time_end;

	//Accept the connection.
	clilen = sizeof(cli_addr);
  	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	//Get the Senders IP

  	if (newsockfd < 0)
  		error("ERROR on accept");



  	bzero(bufferip,64);
  	n = read(newsockfd,bufferip,64);

  	if (n < 0) error("ERROR reading from socket");

	//Connection is established.

  	printf("\n######## CLIENT ACCEPTED ########\n");
	printf("IP CLIENT: %s \n", bufferip);

	char myIP[64];
	strcpy(myIP, bufferip);

  	n = write(newsockfd,"I got your ip",18);

	if (n < 0) error("ERROR writing to socket");

    	//Get the OPTION SEND/RECEIVE MESSAGE
	//Wait for the Choice

	if (newsockfd < 0)
        	error("ERROR on accept");
	bzero(buffer,256);
    	n = read(newsockfd,buffer,255);

    	if (n < 0) error("ERROR reading from socket");
    	printf("CLIENT: Choice is %s\n",buffer);
    	int choice = atoi(buffer);

	//Send Feedback.
	n = write(newsockfd,"I got your choice",18);

    	if (n < 0) error("ERROR writing to socket");


	switch (choice){
		//Case 1 is for sending messages.
		case 1 :
			printf("\n######## SEND Message Proccess ########\n \n");
			//Lock the value of j which is the number of messages.
			pthread_mutex_lock(&lock);
		        j++;
            		myj=j;

			if( j>MAX_MESSAGES_NUM){
				printf("\n ######## MESSAGE CAPACITY FULL EXITING .... ########\n");
				exit(1);
			}
		        pthread_mutex_unlock(&lock);

			//Mutex Unlocked

			strcpy(M[myj].sender, myIP);

			M[myj].message_id=myj;
			// Get the Receivers IP

			bzero(bufferip,64);
			n = read(newsockfd,bufferip,63);

			if (n < 0) error("ERROR reading from socket");
			printf("CLIENT: The receivers ip: %s\n",bufferip);
			strcpy(M[myj].receiver, bufferip);
			//Send Feedback
			n = write(newsockfd,"I got the receivers ip",18);

			if (n < 0) error("ERROR writing to socket");

			// Get the Message

			bzero(buffer,256);
			n = read(newsockfd,buffer,255);

			if (n < 0) error("ERROR reading from socket");
			printf("CLIENT: The message : %s\n",buffer);
			strcpy(M[myj].message, buffer);
			//Send Feedback
  			n = write(newsockfd,"I got your message",18);

  			if (n < 0) error("ERROR writing to socket");

			printf("Message was SUCESSFULLY send from thread: %d with message id : %d \n",p , myj);

			break;

		case 2 :
			//Case 2 for receiving messages.
			printf("\n######## RECEIVE Message Proccess ########\n \n");
			int numm=0;
			int t;
			int ret;
			char sendnumm[256];

			//Count the number of new messages for the client.
			for (t=0; t<MAX_MESSAGES_NUM; t++){
				ret=strcmp(M[t].receiver,myIP);
				if (ret==0){
					numm++;
				}
			}
			//Send the number to the client and wait for feedback.
			bzero(sendnumm, 256);
  		        sprintf(sendnumm,"%d",numm);

            		bzero(buffer,256);
            		strcpy(buffer, sendnumm);

            		printf("Nummber of Messages is %s \n", sendnumm);

            		n=write(newsockfd, buffer, 256);
            		if (n < 0) error("ERROR writing to socket");

			//No new messages.
			if (numm==0){

				printf("No new messages for the client! \n");

			}else{
			//Send the messages to the client.
			bzero(buffer,256);
                	n = read(newsockfd,buffer,255);

                	if (n < 0) error("ERROR reading from socket");
                		printf("CLIENT: %s \n",buffer);

				int count =0;

				for (t=1; t<MAX_MESSAGES_NUM; t++){
					int ret=strcmp(M[t].receiver, myIP);
					//If the IP's match then send the messages.
                    			if (ret==0){

						//gettimeofday(&time_start, NULL);
						//start= (long long)time_start.tv_sec*1000000 + (long long)time_start.tv_usec;

						//printf("START time is : %lli \n", start);
						bzero(bufferm, 512);
						strcpy(bufferm, "Sender: ");
						strcat(bufferm, M[t].sender);
						strcat(bufferm, M[t].message);

						//gettimeofday(&time_start, NULL);
                        			//start= (long long)time_start.tv_sec*1000000 + (long long)time_start.tv_usec;

						n = write(newsockfd, bufferm, strlen(bufferm));
                        			if (n < 0) error("ERROR writing to socket");

						bzero(buffer,256);
               					n = read(newsockfd,buffer,255);

               					if (n < 0) error("ERROR reading from socket");
               					printf("CLIENT: %s \n",buffer);

				/*		//Send Times for speed/System Response

						bzero(buffer, 256);
						sprintf(buffer,"%lli", start);
						n=write(newsockfd, buffer, 256);
                        			if (n < 0) error("ERROR writing to socket");

						//Wait for reply
						bzero(buffer,256);
                        			n = read(newsockfd,buffer,255);

                        			if (n < 0) error("ERROR reading from socket");
                        			printf("CLIENT: %s \n",buffer);
               			 */	}
                    		}

			}

			break;

		default:
			printf("ERROR on your choice!");

	}
	//Send feedback for the connection complete.
	bzero(buffer, 256);
	n=write(newsockfd, "Connection Complete!", 20);

	if (n < 0) error("ERROR writing to socket");
	printf("\nCLIENT LOGGED OUT: %s", myIP);
	printf("######## Connection Complete ########\n");

	//Reduce the number of connected clients.
	pthread_mutex_lock(&lock);
  	number_of_connected_clients--;
  	pthread_cond_signal(&cv);
  	pthread_mutex_unlock(&lock);

	//Exit the thread
  	pthread_exit(NULL);

}


//Main Function.

int main(int argc, char* argv[]) {

	printf("######## SERVER ONLINE ######## \n \n");

	//Create signal for " CTRL C " when the server terminates.
	signal(SIGINT, exithandler);

	int i=0;
	pthread_t *threads;
	pthread_attr_t pthread_custom_attr;
	int n=MAX_CLIENTS;

	//Create Number of threads
	threads = (pthread_t *)malloc(n*sizeof(*threads));
	pthread_attr_init(&pthread_custom_attr);

	parm *p = (parm *)malloc(sizeof(parm)*n);

	//Establish the connection with the port and the socket.
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

	//Listen for clients to connect.
	listen(sockfd,MAX_CLIENTS);
	//Loop forever.
	while (1){

		//If clinets are more than the server can handle, wait for the some client to complete connection.
		if( number_of_connected_clients >= MAX_CLIENTS ) {
			pthread_mutex_lock(&lock);
			pthread_cond_wait(&cv, &lock);
			pthread_mutex_unlock(&lock);
		}

		//Give the thread a pid, and create it.
		p[i].id = i;
		pthread_create(&threads[i], &pthread_custom_attr, work, (void *)(&p[i]));
		i++;

		//Increase the number of connected clients.
		pthread_mutex_lock(&lock);
		number_of_connected_clients ++;
		pthread_mutex_unlock(&lock);

  }

	free(p);

	exit(0);
}

//Function to manage the exit.
void exithandler(int signum){

	printf("\n##WARNING## :Exiting server. All your messages will be deleted! \n");
	exit(1);
}
