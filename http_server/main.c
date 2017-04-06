#include "httpd.h"

static void Useage(const char* str)
{
	printf("Usage: %s [local_ip] [local_port]\n",str);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		Useage(argv[0]);
		return 0;
	}

	int listensock = startup(argv[1],atoi(argv[2]));

	while(1)
	{
		struct sockaddr_in peer;
		peer.sin_family = AF_INET;
		socklen_t len = sizeof(peer);
		int connfd = accept(listensock,(struct sockaddr*)&peer, &len);
		if(connfd < 0)
		{
			print_log("connfd failed!",WARING);
			close(connfd);
			continue;
		}

		printf("one client is connecting!\n");
		pthread_t tid;
		pthread_create(&tid,NULL,accept_request,(void*)connfd);
	}

	return 0;
}
