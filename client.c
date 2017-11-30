#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

struct dataPacket
{
	uint16_t packetStart;
	uint8_t clientId;
	uint16_t type;
	uint8_t segmentID;
	uint8_t length;
	char payload[255];
	uint16_t packetEnd;
};
struct ackPacket
{
	uint16_t packetStart;
	uint8_t clientId;
	uint16_t type;
	uint8_t segmentID;
	uint16_t packetEnd;
};
struct rejectPacket
{
	uint16_t packetStart;
	uint8_t clientId;
	uint16_t type;
	uint16_t rejectSubCode;
	uint8_t segmentID;
	uint16_t packetEnd;
};
int clientSocket;
	struct ackPacket ackPack;
	void waitFor (unsigned int secs);
	
int main(){
	int messageSize=0,counter=0;
	        int retrans=1;
        int recsize;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	struct dataPacket dataPack;

	struct timeval tv;
	dataPack.packetStart = 0XFFFF;
	dataPack.packetEnd = 0XFFFF;				// Changing the error sequence to demonstrate case 3
	dataPack.clientId = 10;
	dataPack.segmentID = 1;
	dataPack.type = 0XFFF1;
	strcpy(dataPack.payload,"Hellohowareyoubaby");
	dataPack.length=strlen(dataPack.payload);
	//dataPack.length++;                          // Demo for length mismatch
	
	
	clock_t begin;
	double time_spent;
	unsigned int i;
	/*Create UDP socket*/
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

	/*Configure settings in address struct*/

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7891);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	/*Initialize size variable to be used later on*/

	addr_size = sizeof serverAddr;
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	int temp = setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
	while(counter <= 5 ){
	    printf("\nSending packet %x \n",dataPack.segmentID);
		sendto(clientSocket,(struct dataPacket*)&dataPack,sizeof(struct dataPacket),0,(struct sockaddr *)&serverAddr,addr_size);
		int messageSize = recievefrom();
		// If ack is not recived
		if(messageSize<=0)
		{
			while(retrans<=5)
			{
			waitFor(1);
			printf("\n Noreply from server\n");
            printf("\n Retransmitting after three seconds\n");
            sendto(clientSocket,(struct dataPacket*)&dataPack,sizeof(struct dataPacket),0,(struct sockaddr *)&serverAddr,addr_size);
            recsize = recvfrom(clientSocket,(struct ackPacket*)&ackPack,sizeof(struct ackPacket),0,NULL, NULL);
				if(retrans==3){
				 printf("\n Server does not respond\n");
				 exit(0);
				}
				retrans++;
			}
		}
		if(counter >= 5 ){
		printf("\n Five messages sent exiting loop \n");
		exit(0);

		}
		if(messageSize > 0){
	    counter++;
		printf("\n ACK recieved from server for segment %x \n",ackPack.segmentID);
		if(counter == 5)
		exit(0);
		}
		dataPack.segmentID++;  // Romve this for showing duplicate packet error
		//dataPack.segmentID++;  //out of requence
	}
	//if( )
	/*Receive message from server*/


	printf("%d \n",messageSize);
	printf("%x \n",ackPack.type);
	printf("%x \n",ackPack.segmentID);
	if (ackPack.type == 0XFFF2){
		printf("Ack Recieved");
	}
	printf("Received from server: %s\n",buffer);
	return 0;
}

int recievefrom(){
	int chumma;
	chumma = recvfrom(clientSocket,(struct ackPacket*)&ackPack,sizeof(struct rejectPacket),0,NULL, NULL);
	if(ackPack.type==0xfff3)
	errorhandling(ackPack.type);
	return chumma;
}

void errorhandling(uint16_t type){
	struct rejectPacket *rejPack = ((struct rejectPacket*) &ackPack);
		printf("Reject code recieved with code:%x",rejPack->rejectSubCode);
	exit(0);
}

void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}
