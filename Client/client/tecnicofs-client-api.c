#include <tecnicofs-client-api.h>
#include <socketClient.h>
#include <server/socketServer.h>
#include "unix.h"
#include<sys/types.h>
#include<sys/socket.h>

int sockClient;

int tfsMount(char * address) {

	int sockClient, servlen;

	struct sockaddr_un serv_addr;
	
	/* O nome serve para que os clientes possam identificar o servidor*/
	bzero((char *)&serv_addr, sizeof(serv_addr));

	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, address);
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

	// inicialize socketClient
	if ((sockClient = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		err_dump("client: can't open stream socket");

	// 
	if (connect(sockClient, (struct sockaddr*) &serv_addr, servlen) < 0)
		err_dump("client : can't connect to server");

}


int tfsUnmount() {

	close(sockClient);
}


