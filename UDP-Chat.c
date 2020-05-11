#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "LIST.h"

// maximum size of the strings that can be sent
#define stringSize 1000

// struct to pass in arguments to the threads
typedef struct commandArguments {
	char* first;
	char* second;
	char* third;
} commandArguments;

// global variables for keyboard/send list and receive/screen list
LIST *sendList, *recvList;

// global variables for the mutexes being used for keyboard/send and receive/screen methods
pthread_mutex_t sendMutex, recvMutex;

// global variables for the condition variable used to signal when a message is input or received
pthread_cond_t sendCond, recvCond;

// global variables for sockets and getaddrinfo linked lists
struct addrinfo *recvAddr, *sendAddr;
int recvSocket, sendSocket;

// global variables for the pthreads
pthread_t keyboard, screen, sendThread, receive;

// method used for the keyboard thread
void* keyboardMethod() {

	size_t string_Size = stringSize;
	
	// infinte loop
	while(1) {
		// create a new string in heap memory so it can be stored in the list
		char* stringSend = (char*)malloc(stringSize*sizeof(char));

		// get the string that the user wants to send
		write(1, "User: ", sizeof("User: "));		
		read(0, stringSend, stringSize);
		write(1, "\n", sizeof("\n"));

		// check whether there is an empty node in sendList to store the string, if not wait
		while(isListFull());

		// use to mutex to lock out send from the critical section while this method adds the string to sendList
		pthread_mutex_lock(&sendMutex);

		// add the string to the sendList
		ListPrepend(sendList, stringSend);

		// unlock mutex because we are leaving the critical section
		pthread_mutex_unlock(&sendMutex);

		// break out of the while loop when the sentinal value is input
		if(strcmp(stringSend, "!\n") == 0) {
			pthread_cond_signal(&sendCond);
			break;	
		}

		// signal that a message is ready to be sent
		pthread_cond_signal(&sendCond);
	
	}


}

// method used for the screen thread
void* screenMethod() {

	// a variable that will hold the string in the recvList
	char* stringReceive;

	//infinite loop
	while(1) {

		// use mutex to lock out the receive from the critical section while this method reads a string from recvList
		pthread_mutex_lock(&recvMutex);

		// wait until there is something to print in the recvList
		if(ListCount(recvList) == 0)
			pthread_cond_wait(&recvCond, &recvMutex);

		// retreive the message at the end of the recvList
		stringReceive = ListTrim(recvList);

		// unlock the mutex because we are leaving the critical section
		pthread_mutex_unlock(&recvMutex);

		// print out the message retreived from the recvList
//		write(1, "From other user: ", sizeof("From other user: "));
//		write(1, stringReceive, sizeof(stringReceive));
//		write(1, "\n", sizeof("\n"));

		printf("From other user: ");
		printf("%s", stringReceive);
		printf("\n");

		if(strcmp(stringReceive, "!\n") == 0) {
			free(stringReceive);
			break;
		}

		free(stringReceive);
	}

	//cancel all other threads and perfrom clean up
	pthread_cancel(keyboard);
	pthread_cancel(sendThread);
	pthread_cancel(receive);

	close(sendSocket);
	close(recvSocket);

    	freeaddrinfo(sendAddr);
	freeaddrinfo(recvAddr); 

}

// method used for the send method
void* sendMethod(void* cA) {

	// get the address and setup the sockets to send messages
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // TCP stream sockets
    	
	// get ready to connect
	if ((status = getaddrinfo(((commandArguments*)cA)->second, ((commandArguments*)cA)->third, &hints, &sendAddr)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	if((sendSocket = socket(sendAddr->ai_family, sendAddr->ai_socktype, sendAddr->ai_protocol) ) < 0 ) { 
        	perror("socket creation failed"); 
        	exit(EXIT_FAILURE); 
    	}

	// variable to hold the message retreived from sendList
	char* stringSend;
	
	// infinite loop
	while(1) {
		// use mutex to lock out keyboard from the critical section while this method retreives the message from sendList
		pthread_mutex_lock(&sendMutex);

		// wait until there is a message in the sendList
		if(ListCount(sendList) ==0)
			pthread_cond_wait(&sendCond, &sendMutex);
	
		// retreive the message at the end of sendList
		stringSend = ListTrim(sendList);

		// unlock the mutex because this method is leaving the critical section
		pthread_mutex_unlock(&sendMutex);
		
		// send the message
		if((sendto(sendSocket, stringSend, strlen(stringSend), 0, sendAddr->ai_addr, sendAddr->ai_addrlen))<0) {
		    printf("Didn't send message\n");
		}

		if(strcmp(stringSend, "!\n") == 0) {
			free(stringSend);
			break;
		}

		free(stringSend);
	}

	// cancel other threads and perform clean up

	pthread_cancel(receive);
	pthread_cancel(screen);	

	close(sendSocket);
	close(recvSocket);

    	freeaddrinfo(sendAddr);
	freeaddrinfo(recvAddr); 	
	
}

// method used for the receive method
void* receiveMethod(void* cA) {

	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	// get ready to connect
	if ((status = getaddrinfo(NULL, ((commandArguments*)cA)->first, &hints, &recvAddr)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	if((recvSocket = socket(recvAddr->ai_family, recvAddr->ai_socktype, recvAddr->ai_protocol) ) < 0 ) { 
        	perror("socket creation failed"); 
        	exit(EXIT_FAILURE); 
    	}

	if((bind(recvSocket, recvAddr->ai_addr, recvAddr->ai_addrlen)) < 0 ) { 
        	perror("bind failed"); 
        	exit(EXIT_FAILURE); 
    	}

	struct sockaddr_storage src_addr;
	socklen_t src_addr_len = sizeof(src_addr);
        int n;
	
	// infinite loop 
	while(1) {

		// create a string in heap memory so it can be added into the list
		char* receiveString = (char*)malloc(stringSize*sizeof(char));

		// receive the message 
		if((n = recvfrom(recvSocket, receiveString, stringSize, 0, (struct sockaddr*)&src_addr, &src_addr_len))<0) {
		    printf("Didn't receive message\n");
		}
		receiveString[n] = '\0';
		
		// check whether is any space in the sendList
		while(isListFull());

		// use mutex to lock out the screen from the critical section while this method inputs another message into recvList
		pthread_mutex_lock(&recvMutex);

		// input the message at the start of the recvList
		ListPrepend(recvList, receiveString);

		// unlock mutex because this method is leaving the critical section
		pthread_mutex_unlock(&recvMutex);

		// set the condition statement for recvCond in order to print it to the screen
		pthread_cond_signal(&recvCond);
	}
	
}

void itemFree(void* item) {
  free(item);
}

int main(int argc, char*  argv[]) {

	// create the 2 mutexes, one for keyboard/send and one for receive/screen
	pthread_mutex_init(&sendMutex, 0);
	pthread_mutex_init(&recvMutex, 0);

	// creat the 2 condition variables
	pthread_cond_init(&recvCond, 0);
	pthread_cond_init(&sendCond, 0);

	// create the two lists to manage data
	sendList = ListCreate();
	recvList = ListCreate();

	// add the command line arguments into the struct
	commandArguments cA = {.first = argv[1], .second = argv[2], .third = argv[3]};

	// create 4 threads, one for each operation
	pthread_create(&keyboard, 0, keyboardMethod, NULL);
	pthread_create(&screen, 0, screenMethod, NULL);
	pthread_create(&sendThread, 0, sendMethod, &cA);
	pthread_create(&receive, 0, receiveMethod, &cA);

	// join the threads when they are finished
	pthread_join(keyboard, 0);
	pthread_join(screen, 0);
	pthread_join(sendThread, 0);
	pthread_join(receive, 0);

	// destroy both mutexes
	pthread_mutex_destroy(&sendMutex);
	pthread_mutex_destroy(&recvMutex);

	// destroy both condition variables
	pthread_cond_destroy(&recvCond);
	pthread_cond_destroy(&sendCond);

	// delete the two lists
	ListFree(sendList, itemFree);
	ListFree(recvList, itemFree);
	

	return 0;

}

