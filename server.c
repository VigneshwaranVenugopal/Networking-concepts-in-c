#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
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

int main(){
	int udpSocket, nBytes;
	char buffer[1024];
	struct dataPacket dataPack;
	struct sockaddr_in serverAddr, clientAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	int i;
	struct ackPacket ackPack;
	struct rejectPacket rejPack;

	/*Initialize ACK*/
	ackPack.packetStart = 0XFFFF;
	ackPack.packetEnd = 0XFFFF;
	ackPack.type = 0XFFF2;


	/*Create UDP socket*/
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	/*Configure settings in address struct*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7891);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	/*Bind socket with address struct*/
	bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	/*Initialize size variable to be used later on*/
	addr_size = sizeof serverStorage;
		ackPack.segmentID = NULL;
	while(1){
		/* Try to receive any incoming UDP datagram. Address and port of
		requesting client will be stored on serverStorage variable */
		nBytes = recvfrom(udpSocket,(struct dataPacket*)&dataPack,sizeof(struct dataPacket),0,(struct sockaddr *)&serverStorage, &addr_size);
		printf("\n You typed: %s \n",dataPack.payload);
		printf("Segment ID: %d \n",dataPack.segmentID);
		//Same segment ID
		if(dataPack.segmentID == ackPack.segmentID){ // Send reject packet for same segment ID
			rejPack.packetStart = 0xffff;
			rejPack.packetEnd = 0xffff;
			rejPack.rejectSubCode = 0XFFF7;
		    rejPack.type = 0XFFF3;
		    printf(" Sending Reject packet");
		    sendto(udpSocket,(struct rejectPacket*)&rejPack,sizeof(struct rejectPacket),0,(struct sockaddr *)&serverStorage,addr_size);
			ackPack.segmentID = NULL;
		}
        else if(dataPack.segmentID != ackPack.segmentID+1){ // Send reject packet if not in sequence
			rejPack.packetStart = 0xffff;
			rejPack.packetEnd = 0xffff;
			rejPack.rejectSubCode = 0XFFF4;
		    rejPack.type = 0XFFF3;
		    printf(" Sending Reject packet");
		    sendto(udpSocket,(struct rejectPacket*)&rejPack,sizeof(struct rejectPacket),0,(struct sockaddr *)&serverStorage,addr_size);
			ackPack.segmentID = NULL;
		}
		else if(dataPack.length!=strlen(dataPack.payload)){
			rejPack.packetStart = 0xffff;
			rejPack.packetEnd = 0xffff;
			rejPack.rejectSubCode = 0XFFF5;
		    rejPack.type = 0XFFF3;
		    printf(" Sending Reject packet");
		    sendto(udpSocket,(struct rejectPacket*)&rejPack,sizeof(struct rejectPacket),0,(struct sockaddr *)&serverStorage,addr_size);
			ackPack.segmentID = NULL;
		}
		else if(dataPack.packetEnd!=0xffff){
			rejPack.packetStart = 0xffff;
			rejPack.packetEnd = 0xffff;
			rejPack.rejectSubCode = 0XFFF6;
		    rejPack.type = 0XFFF3;
		    printf(" Sending Reject packet");
		    sendto(udpSocket,(struct rejectPacket*)&rejPack,sizeof(struct rejectPacket),0,(struct sockaddr *)&serverStorage,addr_size);
			ackPack.segmentID = NULL;
		}
		else{
			ackPack.type = 0xfff2;
            ackPack.clientId = dataPack.clientId;
		    ackPack.segmentID = dataPack.segmentID;
		    printf(" Sending Ack packet");
		    sendto(udpSocket,(struct ackPacket*)&ackPack,sizeof(struct ackPacket),0,(struct sockaddr *)&serverStorage,addr_size);
        }
	}

	return 0;
}
