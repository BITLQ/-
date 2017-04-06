#include"httpd.h"

//print syslog
void print_log(const char* errorstr,int status)
{
	char* err[] = {
		"NOMAL",
		"WARING",
		"FALAT"
		};

	openlog("httpd",LOG_PID|LOG_CONS,LOG_USER);

	// /var/log/messages
	syslog(LOG_INFO,errorstr,err[status]);

	closelog();
}

//listen sock work function
int startup(char* local_ip,int local_port)
{
	int listen_sock = socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock < 0)
	{
		print_log("socket failed!\n",WARING);
		exit(1);
	}

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(local_port);
	local.sin_addr.s_addr = inet_addr(local_ip);
	int opt = 1;
	if(setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)) < 0)
	{
		print_log("setsockopt is failed1\n",WARING);
		exit(1);
	}

	if(bind(listen_sock,(struct sockaddr*)&local,sizeof(local)) < 0)
	{
		print_log("bind falied!\n",WARING);
		exit(1);
	}

	if(listen(listen_sock,5) < 0)
	{
		print_log("liten falied!\n",WARING);
		exit(1);
	}

	return listen_sock;
}


//from sock read request line
static int readline(int sock,char buff[],int len)
{
	assert(buff);
	int i = 0;
	char c = 0;

	while(i < len && c != '\n')
	{
		ssize_t s = recv(sock,&c,1,0);
		if(s>0 && c == '\r')
		{
			printf("%c ",c);
			recv(sock,&c,1,MSG_PEEK);
			if(c == '\n')
			{
				recv(sock,&c,1,0);
			}
			c = '\n';
		}
		buff[i++] = c;
	}
	buff[i] = '\0';
	return i;
}


//return request error status
int echo_error()
{

}


//发送响应
int send_response(int sock,const char* line,int fd,struct stat st)
{
		printf("dbug: send_respose is running\n");
		const char *response = "HTTP/1.0 200 OK \r\n\r\n";
	// read all request and read '\n' end
		int s = 0;
		int ret = 0;
		char buff[SIZE];
		do
		{
			printf("dbug: send readline\n");
			s = readline(sock,buff,sizeof(buff));
			printf("dbug: buff is %s\n",buff);

		}while(s != 1 && buff[1] != '\n');

		printf("dbug：循环跳出\n");
		if(send(sock,response,strlen(response),0) < 0)
		{
			print_log("resposen send failed!\n",WARING);
			ret = 2;
		}else if(sendfile(sock,fd,NULL,st.st_size) < 0){
			print_log("respose file failed!\n",WARING);
			ret = 3;
		}

		printf("dubg: send ok!\n");
		return ret;
}



//判断请求路径的有效,并处里
int deal_path(int sock,char *path)
{
	int ret = 0;   //return status;
	char line[SIZE]; //keep path
	
	sprintf(line,"wwwroot%s",path);
	printf("path is %s\n",line);

	struct stat st;   //存储文件信息 
	int i = strlen(line);
	if(line[i - 1] == '/')
	{
		// wwwroot/ > return index.html
		strcat(line,"index.html");
	}
	// 先要判断路径是否有效，如有效，需读完所有请求

	if(stat(line,&st) < 0)
	{
		//404  file not fount
		print_log("request not found resource!\n",WARING);
		char *res = "http/1.0 404 NOT FOUND!\r\n\r\n";
		
		char buff[SIZE];
		int s = 0;
		do
		{
			s = readline(sock,buff,sizeof(buff));
		
		}while(s != 1 && buff[1] != '\n');

		send(sock,res,strlen(res),0);
		ret = 4;
	}

	else
	{
		if(S_ISREG(st.st_mode))
		{
			//是文件，下面判断是否是可执行程序
			if(st.st_mode&S_IXUSR && st.st_mode&S_IXGRP && st.st_mode&S_IXOTH)
			{
				//fork + exec
			}
			else
			{
				//普通文件  发送响应
				printf("dbug:will send respose!\n");
				//printf("path is %s\n",line);
				
				int fd = open(line,O_RDONLY);
				if(fd < 0)
				{
					print_log("open file failed\n",WARING);
					ret = 1;
				}else{
					printf("dbug: 进入发送阶段\n");
				 	ret = send_response(sock,line,fd,st);
				}
			}//else
		}//fi
	}//else

	return ret;
}//end

//sock deal funcation
int headler_sock(int sock)
{
	int ret = 0;
	//获取GET/POST 和 url
	//判断是否带有参数,利用指针
	
	char buff[102400];
	char path[SIZE];    //保存路径
	char method[64] = {0};
	char* uri_point = NULL;
	
	//CGI
	int cgi = 0;

	//GET /path?a=b&b=c HTTP/1.1

	if(readline(sock,buff,sizeof(buff)) < 0)
	{
		ret = 1;
		goto end;
	}

	uri_point = buff;
	int i = 0;
	int j = 0;
	char c = buff[i];

	while(i < strlen(buff) && c != ' ')
	{
		c = buff[i];
		if(c != ' ')
		{
			method[j] = buff[i];
			uri_point++;
		}
		if(c == ' ')
		{
			method[i] = '\0';
			uri_point++;
			break;
		}
		i++;
		j++;
	}

	//GET / HTTP/1.0
	while(*uri_point == ' ')
	{
		uri_point++;
	}

	if(!strcasecmp(method,"GET") && !strcasecmp(method,"POST"))
	{
		printf("dbug: do goto\n");
		echo_error();
		goto end;
	}

	
	//path = / ; method = GET/POST
	i = 0;
	if(strcasecmp(method,"GET") == 0)
	{
		printf("GET is OK\n");
		while( *uri_point != ' ')
		{
			if(*uri_point == '?')
			{
				cgi = 1;
				uri_point++;   //point argument
				break;
			}
			path[i++] = *uri_point++;
		}
		path[i] = '\0';
	}
	
	//method is post?
	if(strcasecmp(method,"POST") == 0)
	{
		cgi = 1;
	}
	

	if(cgi != 1)
	{
		//readlines only one
		//判断路径是否有效并处里；
		ret = deal_path(sock,path);
	}
	else
	{
	//	cgi_handle();
	}

end:
	close(sock);
	return ret; 
}



//pthread deal funcation
void* accept_request(void* connfd)
{
	int ret = pthread_detach(pthread_self());
	if(ret < 0)
	{
		print_log("pthread_detach failed\n",ret);
		pthread_exit((void*)1);
	}

	int fd = (int)connfd;

	if(headler_sock(fd) != 0)
	{
		close(fd);
		print_log("headler_sock is fault!\n",WARING);
		pthread_exit((void*)1);
		//return NULL;
	}

	close(fd);
	pthread_exit((void*)0);
}
