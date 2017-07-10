#ifndef __HTTPD_H__
#define __HTTPD_H__

#include <sys/sendfile.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<assert.h>
#include<pthread.h>
#include<ctype.h>
#include<string.h>
#include<strings.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<syslog.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <cassert>

#define SIZE 4096
#define NORMAL 0
#define WARING 1
#define FALAT  2

extern "C"
{
    int startup(char* ip,int port);
    int deal_accept_request(int );
    void print_log(const char* error, int level);
    void clear_header(int sock); 
}


void addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD,fd, &event);
}

#endif //__HTTPD_H__
