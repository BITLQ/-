
/*************************************************************************
	> File Name: server.cpp
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Tue 14 Mar 2017 05:21:42 PM PDT
 ************************************************************************/

/*
 *
 *
 */
#define _GUN_SOUTCE 

#include<string.h>
#include<poll.h>
#include<sys/select.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<assert.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#define SIZE 64 

//客户端的功能：1. 给服务器端发送消息；2.将服务器发来的消息打印到终端

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("the right in: [ip] [port]");
        return 1;
    }

    int sock = socket(AF_INET,SOCK_STREAM,0);
    assert(sock >= 0);
    
    struct sockaddr_in peer; 
    peer.sin_family = AF_INET;
    peer.sin_port = htons(atoi(argv[2]));
    peer.sin_addr.s_addr = inet_addr(argv[1]);

    int ret = connect(sock,(struct sockaddr*)&peer,sizeof(struct sockaddr_in));
    if(ret < 0)
    {
        perror("connect failed");
        close(sock);
        return 2;
    }
    //链接成功;运用select和poll，splice;
    
    
    //注册文件描述符0和sock上的可读事件；
    struct  pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = sock;
    fds[1].events = (POLLIN | POLLRDHUP);
    fds[1].revents;

    char readbuf[SIZE];

    //定义splice使用的管道
    int pipefd[2];
    ret = pipe(pipefd);
    assert(ret != -1);

    while(1)
    {
    printf("send# ");
    fflush(stdout);
        ret = poll(fds,2,-1);
        if(ret < 0)
        {
            printf("poll failed\n");
            break;
        }

        if(fds[1].revents & POLLRDHUP)
        {
            printf("server close the connected\n");
            break;
        }

        else if(fds[1].revents & POLLIN)
        {
            memset(readbuf,'\0',SIZE);
            recv(fds[1].fd,readbuf,SIZE-1,0);
            printf("\n%s",readbuf);
        }

        if(fds[0].revents & POLLIN)
        {
            char buff[1024];
            int ret = read(fds[0].fd,buff,sizeof(buff)-1);
            if(ret < 0)
            {
                perror("read is failed\n");
                continue;
            }
            else if(ret > 0)
            {
                buff[ret] = 0;
                write(fds[1].fd,buff,ret);
            }
            //将标准输入直接写道sock（0拷贝）；
//            ret = splice(0,NULL,pipefd[1],NULL,32768,SPLICE_F_MORE | SPLICE_F_MOVE);
//            ret = splice(pipefd[0],NULL,sock,NULL,32768,SPLICE_F_MORE | SPLICE_F_MOVE);
        }
    }
    close(sock);
    return 0;
}
