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

struct numbers{
	unsigned long phoneno;
	uint8_t technology;
	uint16_t paid;
}subscriber[5];

int  inputnumbers();
struct accpermission reqPack;
struct accpermission replyPack;
	
int main(){
	int i,j = 0;
	j = inputnumbers();
	
	int udpSocket, nBytes;
	char buffer[1024];
	struct sockaddr_in serverAddr, clientAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	
	replyPack.packetStart = 0xffff;
	replyPack.packetEnd = 0xffff;
	
	
	/*Create UDP socket*/
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	/*Configure settings in address struct*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7891);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	addr_size = sizeof serverStorage;
	
	
	while(1){
		nBytes = recvfrom(udpSocket,(struct accpermission*)&reqPack,sizeof(struct accpermission),0,(struct sockaddr *)&serverStorage, &addr_size);
		printf("\nChecking for the technology and payment information of %u",reqPack.subscriberno);
		checksubscriber();
		sendto(udpSocket,(struct accpermission*)&replyPack,sizeof(struct accpermission),0,(struct sockaddr *)&serverStorage,addr_size);
		printf("\nWaiting to recieve packet");
	}
	
	return 0;
}

// Hardcoding all the subscriber number and other info
int inputnumbers(){
	int i=0;
	subscriber[0].phoneno= 4088860314;
	subscriber[0].technology = 4;
	subscriber[0].paid = 0;
	subscriber[1].phoneno= 4088860315;
	subscriber[1].technology = 3;
	subscriber[1].paid = 1;
	printf("The numbers in the directory are:\n");
	for(i=2;i<6;i++){
	subscriber[i].phoneno=4088860314+i;
	subscriber[i].technology = 3;
	subscriber[i].paid = 1;
	}
		for(int j=0;j<6;j++)
				printf("\n%u",subscriber[j].phoneno);
	return 0;
}

int checksubscriber(){
	int i;
	for(i=0;i<6;i++){
	if(reqPack.subscriberno==subscriber[i].phoneno)
	{
		if(reqPack.technology==subscriber[i].technology)
		{
			if(subscriber[i].paid==1)
			{
				printf("\nAccept message sent\n");
				replyPack.acc_per = 0XFFFB;
				return 0;
			}
			else{
				printf("\nSubscriber did not pay bill");
				replyPack.acc_per = 0XFFF9;
				return 0;
			}
		}
		else{
			printf("\nUnsupported technology\n");
			replyPack.acc_per = 0XFFFA;
			return 0;
		}
	}
	}
	replyPack.acc_per = 0XFFFA;
	printf("\nSubscriber not in dictionary");
	return 0;
}
	
			
		
			

