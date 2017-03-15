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

#define _GUN_SOUTCE 1

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

#define USER_LIMIT 5  //最大用户数量
#define BUFFER_SIZE 64 //读缓冲区大小
#define FD_LIMIT 65535 //文件描述符数量限制

struct client_data
{
    sockaddr_in address;
    char* write_buf;
    char buf[BUFFER_SIZE];
};

int setnonblocking(int fd)
{
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("usage: %s is ip_address port_number\n",basename(argv[0]));
        return -1;
    }

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    address.sin_addr.s_addr = inet_addr(argv[1]);

    int listensock = socket(AF_INET,SOCK_STREAM,0);
    assert(listensock >= 0);

    int val = 1;
    setsockopt(listensock,0,SO_REUSEADDR,&val,sizeof(val));

    ret = bind(listensock,(struct sockaddr*)&address,sizeof(struct sockaddr_in));
    //assert(ret != -1);

    ret = listen(listensock,5);
    assert(ret != -1);

    //创建users数组
    client_data* users = (client_data*)malloc(sizeof(client_data)*FD_LIMIT);
    assert(users != NULL);

    pollfd fds[USER_LIMIT];
    int user_counter = 0;

    for(int i = 0; i < USER_LIMIT; ++i)
    {
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    fds[0].fd = listensock; //第一个fds设置为监听套接字
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;

    //接收和发送数据
    while(1)
    {
        ret = poll(fds,user_counter+1,-1);
        if(ret < 0)
        {
            printf("poll failed\n");
            break;
        }

        for(int i = 0; i < user_counter+1; ++i)
        {
            /* listensock只关心读事件；*/
            if((fds[i].fd == listensock ) && (fds[i].revents & POLLIN))
            {
                //监听套接字监听到了链接，并且接受数据
                struct sockaddr_in client_address;
                socklen_t client_addrlen = sizeof(struct sockaddr_in);

                int connfd = accept(listensock,(struct sockaddr*)&client_address,&client_addrlen);
               
                if(connfd < 0)
                {
                    printf("connect  is failed\n");
                    continue;
                }

                //用户太多时，关闭链接
                if(user_counter >= USER_LIMIT)
                {
                    const char* info = "too many users\n";
                    printf("%s",info);
                    send(connfd,info,strlen(info),0);
                    close(connfd);
                    continue;
                }

                user_counter++;
                users[connfd],address = client_address;
                setnonblocking(connfd);
                fds[user_counter].fd = connfd;
                fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
                fds[user_counter].revents = 0;
            }
            else if(fds[i].revents & POLLERR)
            {
                printf("get an error from %d\n", fds[i].fd);
                char errors[100];
                memset(errors,'\0',100);
                socklen_t length = sizeof(errors);
                if(getsockopt(fds[i].fd,SOL_SOCKET,SO_ERROR,&errors,&length) < 0)
                {
                    printf("getsockopt is failed\n");
                }
                continue;
            }
            else if(fds[i].revents & POLLRDHUP)
            {
                //客户端断开链接
                //将最后一个用户的信息拷贝到这个取消链接的信息中，user_counter--;
                printf("%s client left\n",inet_ntoa(users[fds[i].fd].address.sin_addr));
                users[fds[i].fd] = users[fds[user_counter].fd];
                close(fds[i].fd);
                fds[i] = fds[user_counter];
                i--;
                user_counter--;
                //改进的话，可以将客户ip地址打印
            }
            else if(fds[i].revents & POLLIN)
            {
                int connfd = fds[i].fd;
                memset(users[connfd].buf,'\0',BUFFER_SIZE);
                ret = recv(connfd,users[connfd].buf,BUFFER_SIZE - 1,0);
                printf("get %d bytes of client data %s from %d\n",ret,users[connfd].buf,connfd);
                if(ret < 0)
                {
                    /* 读取失败 */
                    users[fds[i].fd] = users[fds[user_counter].fd];
                    close(fds[i].fd);
                    fds[i] = fds[user_counter];
                    i--;
                    close(connfd);
                }
                else if(ret == 0)
                {
                   /* 读取完毕*/
                   /* 在POLLRDHU处已经处理；*/
                }
                else
                {
                    /* 如果接受到客户数据，则通知其它socket链接准备写数据*/
                    for(int j = 1 ; j <= user_counter; ++j)
                    {
                        //注意j从1开始
                        if(fds[j].fd == connfd)
                        {
                            continue;
                        }

                        fds[j].events |= ~POLLIN;
                        fds[j].events |= POLLOUT;
                        //users[fds[j].fd].write_buf = users[connfd].buf;
                        struct sockaddr_in peer;
                        socklen_t peerlen = sizeof(struct sockaddr_in);

                        if(getpeername(connfd,(struct sockaddr*)&peer,&peerlen))
                        {
                           perror("getpeersock failed\n");
                           continue;
                        }
                        char* ipadd = inet_ntoa(peer.sin_addr);
                        strcat(ipadd,": ");
                        strcat(ipadd,users[connfd].buf);
                        users[fds[j].fd].write_buf = ipadd;
                    }
                }
            }
            else if(fds[i].revents & POLLOUT)
            {
                int connfd = fds[i].fd;
                if(!users[connfd].write_buf)
                {
                    continue;
                }

                ret = send(connfd,users[connfd].write_buf,strlen(users[connfd].write_buf),0);
                users[connfd].write_buf = NULL;

                //将注册事件改回去；
                fds[i].events |= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }

    free(users);
    close(listensock);
    return 0;
}
