#ifndef __HTTPD_H__
#define __HTTPD_H__

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

#define SIZE 4096
#define NORMAL 0
#define WARING 1
#define FALAT  2

int startup(char* ip,int port);
void* accept_request(void*);
void print_log(const char* error, int level);
void clear_header(int sock); 

#endif //__HTTPD_H__
