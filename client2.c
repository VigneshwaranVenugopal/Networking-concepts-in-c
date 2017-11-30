#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

struct accpermission
{
	uint16_t packetStart;
	uint8_t clientId;
	uint16_t acc_per;
	uint8_t segmentno;
	uint8_t length;
	uint8_t technology;
	unsigned long subscriberno;
	uint16_t packetEnd;
};

struct accpermission reqPack;
struct accpermission recPack;
int clientSocket;
	
int main(){
	int messageSize=0,counter=0;
	int retrans=1;
    int recsize;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	struct timeval tv;
	
	//Requesting subscriber details
	reqPack.packetStart = 0XFFFF;
	reqPack.packetEnd = 0XFFFF;	
	reqPack.clientId = 10;
	reqPack.segmentno = 1;
	reqPack.subscriberno = 4088860316;	//Change phone number here
	reqPack.technology=4;				//Change technology here

	clock_t begin;
	double time_spent;
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7891);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
	addr_size = sizeof serverAddr;
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	int temp = setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
	
	printf("\nRquesting network for %u \n",reqPack.subscriberno);
	sendto(clientSocket,(struct accpermission*)&reqPack,sizeof(struct accpermission),0,(struct accpermission *)&serverAddr,addr_size);
		
	
	int temp1 = recvfrom(clientSocket,(struct accpermission*)&recPack,sizeof(struct accpermission),0,NULL, NULL);
	
	if(recPack.acc_per==0XFFFB)
		printf("Subscriber accepted\n");
	if(recPack.acc_per==0XFFFA)
		printf("Subscriber does not exist in database\n");	
	if(recPack.acc_per==0XFFF9)
		printf("Subscriber has not paid\n");
	return 0;
}
