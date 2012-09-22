#include <stdio.h>
#include <WINSOCK2.H>

void main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		return;
	}
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 )
	{
		WSACleanup( );
		return; 
	}

	SOCKET sockSrv=socket(AF_INET,SOCK_DGRAM,0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6101);

	bind(sockSrv,(sockaddr*)&addrSrv,sizeof(sockaddr));

	char sendBuf[100];
	char recvBuf[100];
	char tempBuf[100];
	int len=sizeof(sockaddr);
	SOCKADDR_IN addrClient;
	while(1)
	{
		recvfrom(sockSrv,tempBuf,strlen(tempBuf),0,(sockaddr*)&addrClient,&len);
		if('q'!=tempBuf[0])
		{
			sprintf(recvBuf,"%s say: %s",inet_ntoa(addrClient.sin_addr),tempBuf);
			printf("%s\n",recvBuf);
			printf("please input your data: ");
			gets(sendBuf);
			sendto(sockSrv,sendBuf,strlen(sendBuf)+1,0,(sockaddr*)&addrClient,len);
		}
		else
		{
			printf("%s request to quit the chat platform.\n",
				inet_ntoa(addrClient.sin_addr));
			sendto(sockSrv,"q",strlen("q")+1,0,(sockaddr*)&addrClient,len);
			break;
		}
	}
	closesocket(sockSrv);
	WSACleanup();
}
