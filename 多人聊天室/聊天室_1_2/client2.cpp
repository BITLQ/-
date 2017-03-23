/*************************************************************************
	> File Name: client2.cpp
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Fri 17 Mar 2017 05:43:31 PM PDT
 ************************************************************************/

#include<iostream>

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
#include<sys/epoll.h>


/*  改进版客户端，预期实现功能：
 *  1.与服务器建立链接和发送采用tcp；
 *  2.客户端之间通信采用tcp；
 *  3.文件传输功能：udp的话反而降低性能 sendfile函数使用；
 *  传输时考虑使用线程或者子进程执行；
 *  4.I/0多路复用采用select，提高性能；
 *  5.缺陷：一对一聊天时其他客户端无法发送消息；
 */

using namespace std;

//全局变量定义区：
int tcpfd;  //tcp套接字；
int ucpfd;  //udp套接字；
int usedfalgs = 0; //占用标志；
int tcplistensock; //客户端监听套接字；

struct friend_data
{
    
    struct sockaddr_in address;
};

/* tcp用来建立链接，udp用来发送消息 */

//群聊和单对单聊天都是tcp建立链接



//会话同一采用udp
void Talk(int connfd)
{
    while(1)
    {
        if(connfd == tcplistensock)
        {
            //代表
        }
        else if(connfd == udpfd)
        {
            if(fds[2].revents & POLLIN)
            {
                recvfrom(udpfd,(struct));
            }
        }
    }
}

//创建套接子；
void tcpListen()
{
    //创建tcp套接子
    tcplistensock = socket(AF_INET,SOCK_STREAM,0);
    assert(tcplistensock != -1);
    
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(8082);
    local.sin_addr.s_addr = inet_addr("0");

    int ret = bind(tcplistensock,(struct sockaddr*)&local,sizeof(local));
    assert(ret != -1);

    ret = listen(tcplistensock,5);
    assert(ret != -1);

    //创建udp套接子
    udpfd = socket(AF_INET,SOCK_DGRAM,0);
    assert(udpfd != -1);

    local.sin_family = AF_INET;
    local.sin_port = htons(8082);
    local.sin_addr.s_addr = inet_addr("0");

    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    socklen_t len = sizeof(peer);
    
    ret =  bind(udpfd,(struct sockaddr*)&local,sizeof(local));
    assert(ret != -1);

    pollfd fds[3];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = tcplistensock;
    fds[1].events = POLLIN | POLLRDHUP;
    fds[1].revent = 0;
    fds[2].fd = udpfd;
    fds[2].events = POLLIN | POLLOUT;
    fds[2].revents = 0;

    if(usedfalgs == 1)
    {
        printf("\nthe %s want to talk about you!!!\n",inet_ntoa(peer.sin_addr));
        close(connfd);
    }
    else{
        int connfd = accept(tcplistensock,(struct sockaddr*)&peer,&len);
        assert(connfd != -1);


        //当前不忙，可以接受消息；
        usedfalgs = 1;
        Talk(connfd);
        //recvfrom();
    }
}

int tcpLink(char* ip,char* port)
{
    //int listsock = socket(AF_INET,SOCK_STREAM,0);
    //assert(listsock != -1);
    
    tcpfd = socket(AF_INET,SOCK_STREAM,0);
    assert(tcpfd != -1);

    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(atoi(port));
    peer.sin_addr.s_addr = inet_addr(ip);
    
}


void Session()
{
    //printf("输入群聊ip和端口号")
    //这里给默认端口号和地址，当前只有一个群；
    tcpLink("0","8080");
}

void onlyOne()
{
    
    char tmpip[20] = {0};
    char tmpport[10] = {0};

    printf("输入对话方[ip] [port] -> 0,0取消: ");

    scanf("%s%s",tmpip,tmpport);

    if(strlen(tmpip) == 1)
    {
        if(strlen(tmpport) == 1)
        {
            if(tmpip[0] == '0' && tmpport[0] == '0')
            {
                return;
            }
        }
    }

    tcpLink(tmpip,tmpport);
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
    
    return 0;
}
