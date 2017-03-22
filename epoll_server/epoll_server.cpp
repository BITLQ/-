/*************************************************************************
	> File Name: epoll_server.cpp
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Sat 18 Mar 2017 11:12:01 PM PDT
 ************************************************************************/

//#define _GNU_SOURCE

#include<iostream>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#define MAX_USER  5
#define MAX_EVENTS 256
#define MAX_FD 65535
using namespace std;

//标识当前client数量
int user_count = 0;


// 保存客户数据的结构
struct client_data
{
    //int fd;
    struct sockaddr_in address;
    char *pbuf;
    char write_buff[1024];
};


//保存当前已链接的 client信息；
client_data *users = new client_data[MAX_FD];

//添加事件
void fdadd(int epfd,int fd)
{
    epoll_event event;
    event.events = EPOLLIN | EPOLLRDHUP;
    event.data.fd = fd;

///* dbug */  printf("fd = %d\n",fd);
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);
}

//注销事件
void del_fd(int epfd, int fd)
{
///* dbug */ printf("%d fd is del event\n",fd);

    epoll_event event;
    event.events = EPOLLIN | EPOLLRDHUP;
    event.data.fd = fd;
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&event);
}

//修改
void change_fd(int epfd, int fd)
{
///* dbug */ printf("%d fd is change \n",fd);
    epoll_event event;
    event.events = EPOLLOUT | EPOLLRDHUP;
    event.data.fd = fd;
    epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&event);
}
int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("the right putting is [ip] [port]\n");
        exit(1);
    }
    int sock = socket(AF_INET,SOCK_STREAM,0);
    assert(sock != -1);
    
    int val = 1;
    int ret = setsockopt(sock,0,SO_REUSEADDR,&val,sizeof(val));
    assert(ret != -1);
    
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[2]));
    local.sin_addr.s_addr = inet_addr(argv[1]);

    ret = bind(sock,(struct sockaddr*)&local, sizeof(local));
    assert(ret != -1);

    ret = listen(sock,5);
    assert(ret != -1);

    int epfd = epoll_create(256);
    //监听套接字注册输入事件；
    //fdadd(epfd,sock);
    printf("sock = %d\n",sock);
    epoll_event tmpevent;
    tmpevent.events = EPOLLIN;
    tmpevent.data.fd = sock;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sock,&tmpevent);
    
    epoll_event evs[MAX_EVENTS];

    while(1)
    {
        int nums = epoll_wait(epfd,evs,MAX_EVENTS,-1);
///* dbug */  printf("nums = %d\n",nums);

        for(int i = 0; i < nums; ++i)
        {
///* dbug */   printf("evs[i],data.fd = %d\n",evs[i].data.fd);

            if(evs[i].data.fd == sock && (evs[i].events & EPOLLIN))
            {
                 struct sockaddr_in peer;
                 peer.sin_family = AF_INET;
                 socklen_t len = sizeof(peer);

                 int connfd = accept(sock,(struct sockaddr*)&peer,&len);
/* dbug */       printf("connfd = %d\n",connfd);
                 if(connfd < 0)
                 {
                     printf("accept is failed\n");
                     continue;
                 }

                if(user_count >= MAX_USER)
                {
                    //用户数量达到上线，关闭链接；
                    const char* info = "too many link!\n";
                    send(connfd,info,strlen(info),0);
                    close(connfd);
                    continue;
                }

                fdadd(epfd,connfd);
                user_count++;
                users[connfd].address = peer;
            }
            else if(evs[i].events & EPOLLRDHUP)
            {
//* dbugs */    printf("client close %d",evs[i].data.fd);                
                //client断开链接；
                printf("the %s client is quit!\n",inet_ntoa(users[evs[i].data.fd].address.sin_addr));

                del_fd(epfd,evs[i].data.fd);
                close(evs[i].data.fd);
                user_count--;
            }
            else if(evs[i].events & EPOLLIN)
            {
                //client 发来消息
///* dbug */      printf("fd is %d\n",evs[i].data.fd);
               int ret = recv(evs[i].data.fd,users[evs[i].data.fd].write_buff,sizeof(users[evs[i].data.fd].write_buff),0);

                if(ret < 0)
                {
                    perror("recv");
                    continue;
                }
                else if(ret == 0)
                {
                    //EPOLLRDHUP;
                }
                else
                {
                    //const char* info = "hello\n";
                    const char* info = "http/1.0 200 0k\r\n\r\n<html><h1>hello world</h1></html>";
                    strncpy(users[evs[i].data.fd].write_buff,info,strlen(info));
                    change_fd(epfd,evs[i].data.fd);          
                }
            }
            else if(evs[i].events & EPOLLOUT)
            {
                char *info = users[evs[i].data.fd].write_buff;
//* dbug */      printf("send fd = %d\n",evs[i].data.fd);                
                send(evs[i].data.fd,info,strlen(info),0);
                memset(users[evs[i].data.fd].write_buff,'\0',sizeof(users[evs[i].data.fd].write_buff));
                fdadd(epfd,evs[i].data.fd);
            }
        }
    }

    delete[] users;
    return 0;
}
