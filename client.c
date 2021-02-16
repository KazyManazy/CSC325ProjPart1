#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_LENGTH 1024

/* 
    The client file was heavily influenced from these sources:
    Computer Networks: A Systems Approach (Peterson & Davie)
    Unix Networking Programming Volume 1 (Wesley)
    Linux Man Pages
*/
int main(int argc, char * argv[]) {

	char *hostIPAddress;
	char *hostPortString;
	int hostPortNumber;
	//Structure used to represent a host.
	struct hostent *hostServer;
	//Structure used to represent an internet address.
	struct sockaddr_in serverAddress;
	int socketID;
	int connectionCheck;
	char buffer[MAX_LENGTH + 2];
	int payloadLength;
	int totalMessageSize;
  int overMaxLength = 0;
	char* payload;


	//Checks to make sure there is 2 arguments entered that will be the host IP address and the port number.
	if (argc==3) {
    	hostIPAddress = argv[1];
    	hostPortString = argv[2];
    	hostPortNumber = atoi(hostPortString);
  	}
  	else {
	    fprintf(stderr, "Need 2 arguments when running this file: IPV4 Address, Port Number.\n");
	    exit(1);
  	}

  	//Creates the hostent struct, but no lookup is performed since the hostIPAddress is an IPV4 address.
  	hostServer = gethostbyname(hostIPAddress);
  	//Checks to see of the hostServer hostent struct was initialized.
  	if(!hostServer) {
  		fprintf(stderr, "A valid IPV4 Address was not entered as the first argument when running this file.\n");
  		exit(1);
  	}

  	/* The following section builds the sockaddr_in struct for the server's address. */
  	//This sets the entire serverAddress structure to 0.  
  	bzero((char *)&serverAddress, sizeof(serverAddress));
  	//Specifies that the address is in the internet domain with an IPv4 Address.
  	serverAddress.sin_family = AF_INET;
  	//Copies the hostServer's IPv4 Address to the serverAddress's sin_addr struct.
  	bcopy(hostServer->h_addr, (char *)&serverAddress.sin_addr, hostServer->h_length);
  	//Sets the port number for the server address and makes sure that it is stored in memory correctly (htons).
  	serverAddress.sin_port = htons(hostPortNumber);

  	//Creates a socket and socketID is the identifier to reference that socket.
  	//PF_INET and AF_INET are the same, but PF_INET is commonly used when calling socket.  Reference in Beej's Guide to Network Programming.
  	//SOCK_STREAM is for two-way byte streams.
  	//0 is given so the default socket protocol will be used.
  	socketID = socket(PF_INET, SOCK_STREAM, 0);
  	if(socketID < 0) {
  		perror("Error in creating the socket: \n");
  		exit(1);
  	}

  	//Creates the connection with the server using the socket and server address.
  	connectionCheck = connect(socketID, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
  	if (connectionCheck < 0) {
  		perror("Error in connecting to the socket: \n");
  		close(socketID);
  		exit(1);
  	}

  	//Actually sending the messages.
  	while (fgets(buffer, sizeof(buffer), stdin)) {
    	payloadLength = 0;
      overMaxLength = 0;
    	while(buffer[payloadLength] != '\n') {
    		payloadLength = payloadLength + 1;
    		if (payloadLength > MAX_LENGTH) {
          payloadLength = MAX_LENGTH;

          //EDGE CASE TESTING
          //printf("2nd to last char: %c \n", buffer[MAX_LENGTH - 1]);
          //printf("last char: %c \n", buffer[MAX_LENGTH]);
          //EDGE CASE TESTING


          if(buffer[MAX_LENGTH] == '\n') {
            break;
          }
          overMaxLength = 1;
    			printf("WARNING: Message too long (keep below 1024 characters) Concatenating to 1024. \n");
    			break;
    		}
    	}

    	//TESTING
    	//printf("Payload length: %d \n", payloadLength);
    	//TESTING

      //Creates the proper payload size in bytes in memory.
    	totalMessageSize = payloadLength + 4;
    	payload = malloc(totalMessageSize*sizeof(char));

    	//TESTING
    	//printf("Size allocated for total payload: %d \n", totalMessageSize);
    	//TESTING

    	for(int i = 0; i < payloadLength; i++) {
    		payload[i + 4] = buffer[i];
    	}
    	payload[0] = (payloadLength / 1000) + '0';
    	payloadLength = payloadLength % 1000;
    	payload[1] = (payloadLength / 100) + '0';
    	payloadLength = payloadLength % 100;
    	payload[2] = (payloadLength / 10) + '0';
    	payload[3] = (payloadLength % 10) + '0';

    	//TESTING
    	//printf("Total Message: ");
    	//for(int i = 0; i < totalMessageSize; i++) {
    	//	printf("%c", payload[i]);
    	//}
    	//printf("\n \n");
    	//TESTING

    	send(socketID, payload, totalMessageSize, 0);
      
      //Will get rid of the characters after 1024.
      while (overMaxLength == 1 && fgetc(stdin) != '\n') {
        continue;
      }

      //Frees the bytes used for the current payload.
    	free(payload);

  	}

  	//TESTING
  	//printf("Closing socket. \n");
  	//TESTING

  	close(socketID);
  	exit(0);
}
