/*************************************************************************
	> File Name: server_CGI.cpp
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Tue 14 Mar 2017 11:04:53 PM PDT
 ************************************************************************/

#include<stdio.h>
#include<assert.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("usage is [ip] [port]\n");
        return 1;
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[2]));
    local.sin_addr.s_addr = inet_addr(argv[1]);

    int sock = socket(PF_INET,SOCK_STREAM,0);
    assert(sock != -1);
    int val = 1;

    setsockopt(sock,0,SO_REUSEADDR,&val,sizeof(val));

    int ret = bind(sock,(struct sockaddr*)&local,sizeof(local));
    assert(ret != -1);
    

    ret = listen(sock,5);
    assert(ret != -1);

    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    socklen_t peerlen = sizeof(peer);

    int connfd = accept(sock,(struct sockaddr*)&peer,&peerlen);
    printf("connfd = %d\n",connfd);
    if(connfd < 0)
    {
        perror("connect falied\n");
        return 2;
    }

    else
    {
        //使用dup和dup2
//        close(1);
//        ret = dup(connfd);
//        if(ret < 0)
//        {
//            perror("dup");
//            return 3;
//        }

        close(1);
        ret = dup2(connfd,1);
        assert(ret >= 0);

        printf("aaaaa\n");
        close(connfd);
    }

    close(sock);
    return 0;
}

