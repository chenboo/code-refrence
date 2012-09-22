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
	SOCKET sockClient=socket(AF_INET,SOCK_DGRAM,0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6101);

	char sendBuf[100];
	char tempBuf[100];
	char recvBuf[100];
	int len=sizeof(sockaddr);

	while(1)
	{
		printf("please input your data: ");
		gets_s(sendBuf);
		sendto(sockClient,sendBuf,strlen(sendBuf)+1,0,(sockaddr*)&addrSrv,len);
		recvfrom(sockClient,tempBuf,strlen(tempBuf),0,(sockaddr*)&addrSrv,&len);
		if('q'!=tempBuf[0])
		{
			sprintf(recvBuf,"%s say: %s",inet_ntoa(addrSrv.sin_addr),tempBuf);
			printf("%s\n",recvBuf);
		}
		else
		{
			printf("the server has been closed!\n");
			sendto(sockClient,"q",strlen("q")+1,0,(sockaddr*)&addrSrv,len);
			break;
		}
	}

	closesocket(sockClient);
	WSACleanup();
}