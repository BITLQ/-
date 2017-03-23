
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
#include<signal.h>
#include<iostream>

using namespace std;

//客户端的功能：1. 给服务器端发送消息；2.将服务器发来的消息打印到终端

//待改善：1.将系统调用出错的信息不打印在终端，将perror重定向到一个error.log文件中；
//        2.将poll替换为更高效的epoll；

int sock;
int overfalg = 0;  //定义全局flags

//退出会话
void quitSession(int sig)
{
    close(sock);
    printf("\nsession is quitSession\n");
  //  exit(0);
}

void Session()
{
    signal(SIGINT,quitSession);
    //后期，服务器的端口号和ip应该是固定的；
    printf("输入群聊ip和端口号(0,0退出): ");
    char Sip[20] = {0};
    char Sport[10] = {0};
    
    scanf("%s%s",Sip,Sport);

    printf("%d, %d\n",strlen(Sip),strlen(Sport));

    int lenip = strlen(Sip);
    int lenport = strlen(Sport);

    //输入之后，检测是否为退出选项；
    //if((strlen(Sip)==1) && (strlen(Sport)==1));
    if(lenip == 1)
    {
        if(lenport == 1)
        {
            return ;
        }
    }

    sock = socket(AF_INET,SOCK_STREAM,0);
    assert(sock >= 0);
    
    struct sockaddr_in peer; 
    peer.sin_family = AF_INET;
    peer.sin_port = htons(atoi(Sport));
    peer.sin_addr.s_addr = inet_addr(Sip);

    int ret = connect(sock,(struct sockaddr*)&peer,sizeof(struct sockaddr_in));
    if(ret < 0)
    {
        perror("connect failed");
        close(sock);
        return;
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

    int  flag = 0;  //为达到控制输出的目的设置

    while(1)
    {
        if(flag == 1)
        {
            printf("（ctrl+c退出）send# ");
            fflush(stdout);
        }

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
            flag = 1;
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
}



void Display()
{
    printf("********欢迎使用在线聊天系统*******\n");
    printf("***1.进入多人聊天室\n");
    printf("***2.一对一聊天\n");
    printf("***0.退出聊天室\n");
}

int main(int argc, char *argv[])
{

    int op = 0;  //接收选项
    
    while(1)
    {
        Display();
        printf("select op: ");
        scanf("%d",&op);
        switch (op)
        {
            case 1:
                Session();
                break;
            case 2:
            Session();
                break;

            case 0:
                exit(0);

            default:
                printf("输入选项有误\n");
                break;
        }
    }

    close(sock);
    return 0;
}
