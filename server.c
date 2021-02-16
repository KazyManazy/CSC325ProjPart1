#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_PENDING 5
#define MAX_LENGTH 1028
#define LENGTH_FIELD 4

/* 
	The client file was heavily influenced from these sources:
	Computer Networks: A Systems Approach (Peterson & Davie)
	Unix Networking Programming Volume 1 (Wesley)
 	Linux Man Pages
*/
int main(int argc, char * argv[]) {

	char *hostPortString;
	int hostPortNumber;
	//Struct used to represent an internet address.
	struct sockaddr_in clientAddress;
	int socketID;
	int bindCheck;
	int newSocketID;
	char buffer[MAX_LENGTH];
	int bufferLength;
	int addressLength;
	char messageSizeString[LENGTH_FIELD + 1];
  	messageSizeString[4] = '\0';
  	int totalMessageSize;
  	int i;

	//Making sure that one argument was entered when running this file.
	if(argc==2) {
		hostPortString = argv[1];
		hostPortNumber = atoi(hostPortString);
	}
	else {
		fprintf(stderr, "Need 1 argument when running this file: Port Number\n");
		exit(1);
	}

	/* This sections builds the sockaddr_in struct for the client's address */
	//This sets the entire clientAddress to 0.
	bzero((char *)&clientAddress, sizeof(clientAddress));
	//Specifies that the address is an internet IPv4 address.
	clientAddress.sin_family = AF_INET;
	//Represents a wildcard address so that any outside address can connect.
	clientAddress.sin_addr.s_addr = INADDR_ANY;
	//Sets the port number for the client to connect with.
	clientAddress.sin_port = htons(hostPortNumber);

	//Creates a socket and socketID is the identifier to reference that socket.
	//PF_INET and AF_INET are the same, but PF_INET is commonly used when calling socket.
	//SOCK_STREAM is for two-way byte streams.
	//0 is given so the default socket protocol will be used.
	socketID = socket(PF_INET, SOCK_STREAM, 0);
	if (socketID < 0) {
		perror("Error in creating the socket: \n");
		exit(1);
	}
	
	//binds a name (client address) to the socket.
	bindCheck = bind(socketID, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
	if (bindCheck < 0) {
		perror("Error in binding the socket: \n");
		exit(1);
	}
	
	//Marks the socket as a passive socket to accept incoming connections.
	//MAX_PENDING is the backlog or the maximum number of pending connections.
	listen(socketID, MAX_PENDING);

 	// Continually waits for incoming connections
  	while(1) {

  		//Accepts the first connection request from the queue of pending connections for the listening socket.
  		newSocketID = accept(socketID, (struct sockaddr *)&clientAddress, &addressLength);
  		if (newSocketID < 0) {
  			perror("Error in accepting new socket connection: \n");
  			exit(1);
  		}

  		//Receives messages from the socket.  recv returns the number of bytes.
    	while (bufferLength = recv(newSocketID, buffer, sizeof(buffer), 0)) {

    		if(bufferLength < 0) {
    			perror("Error in keeping connected with the client: \n");
    			close(newSocketID);
    			exit(1);
    		}

    		//TESTING
    		//printf("Recv Value: %d \n", bufferLength);
    		//TESTING

    		//Finds the length of the payload in the first 4 bytes of the message.
      		fputs("Payload Length: ", stdout);
      		for(i = 0; i < LENGTH_FIELD; i++) {
        		messageSizeString[i] = buffer[i];
        		fputc(buffer[i], stdout);
      		}
      		fputc('\n', stdout);

      		//Prints out the payload based on the first 4 bytes/
      		fputs("Payload   Text: ", stdout);
      		totalMessageSize = atoi(messageSizeString);
      		for(i = 4; i < (totalMessageSize + 4); i++) {
        		fputc(buffer[i], stdout);
      		}
      		fputc('\n', stdout);

      		//Zeroes out the buffer so that new messages will start at the beginning of the buffer.
      		bzero(buffer, sizeof(buffer));
    	}
    	
    	//When recv receives 
    	close(newSocketID);

    	//TESTING
    	//printf("Exit Value: %d \n", bufferLength);
    	//TESTING

    	exit(bufferLength);
  	}
}