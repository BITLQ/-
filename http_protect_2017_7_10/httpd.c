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
	
	int val = 1;	
	socklen_t len = sizeof(val);
	setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,(void*)&val,len);	

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
int readline(int sock,char buff[],int len)
{
	assert(buff);
	int i = 0;
	char c = 0;

	while(i < len && c != '\n')
	{
		ssize_t s = recv(sock,&c,1,0);
		if(s>0 && c == '\r')
		{
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
	printf("DEBUG7: read line buff is %s\n",buff);
	printf("DEBUG7: read line i is %d\n",i);
	return i;
}

//error response
static void send_error(int sock,const char* rep)
{	
	clear_header(sock);
	send(sock,rep,strlen(rep),0);
	const char* content_type = "Content-Type:text/html;Charset=ISO-8859-1\r\n\r\n";
	send(sock,content_type,strlen(content_type),0);
}

//return request error status
int echo_error(int sock,int retcode)
{
	char *response = NULL;
	switch(retcode)
	{
		case 400:
			response = "HTTP/1.0 400 ERROR REQUEST\r\n";
			send_error(sock,response);
			break;
		case 403:
			response = "HTTP/1.0 403 SERVER REFUSE REQUEST\r\n";
			send_error(sock,response);
			break;
		case 404:
			response = "HTTP/1.0 404 NOT FOUND\r\n";
			send_error(sock,response);
			break;
		case 500:
			response = "HTTP/1.0 500 SERVER HAVE A ERROR\r\n";
			send_error(sock,response);
			break;
		case 503:
			response = "HTTP/1.0 503 SERVER NOT TO BE USEING\r\n";
			send_error(sock,response);
			break;
		default:
			break;
	}

	send(sock,"<html><h2>",strlen("<html><h2>"),0);
	send(sock,response,strlen(response)-2,0);
	send(sock,"</h2></html>",strlen("</h2></html>"),0);
}

void clear_header(int sock)
{
		int s = -1;
		int ret = 0;
		char buff[SIZE];
		memset(buff,'\0',sizeof(buff));
		do
		{
			s = readline(sock,buff,sizeof(buff));
			//printf("DEBUG: si xunhuan \n");

		}while(s != 1 && strcmp(buff,"\n"));  //bug!!!
}

//发送响应
int send_response(int sock,const char* line,int fd,struct stat st,const char* responseline)
{
		int ret = 0;
	//	const char *response = "HTTP/1.0 200 OK \r\n\r\n";
	// read all request and read '\n' end

		clear_header(sock);

		if(send(sock,responseline,strlen(responseline),0) < 0)
		{
			print_log("resposen send failed!\n",WARING);
			ret = -1;
			return ret;
		}
		
		send(sock,"\r\n",strlen("\r\n"),0);

		if(sendfile(sock,fd,NULL,st.st_size) < 0){
			print_log("respose file failed!\n",WARING);
			ret = -2;
		}

		printf("dubg: send ok!\n");
		return ret;
}



//deal cgi  read line get / http/1.0
int cgi_deal(int sock, char* dir)
{
    char* path = "dir_cgi.sh";
    printf("dir is %s\n", dir);
	char buf[SIZE];
	char dir_env[SIZE/8];

	//Get 和 Post 的请求行和请求包头和空行都读取了；
	//tell Mozila this is a html;
	const char* status_line = "HTTP/1.0 200 OK\r\n";
	send(sock,status_line,strlen(status_line),0);
    //const char* content_type = "Content-Type:text/html;Charset=ISO-8859-1\r\n";
    const char* content_type = "Content-Type:text/html;Charset=utf-8\r\n";
    send(sock,content_type,strlen(content_type),0);
    const char* lang = "Accept-Language: zh-cn\r\n";
    send(sock,lang,strlen(lang),0);
    send(sock,"\r\n",strlen("\r\n"),0);

	//child proc
	int input[2];
	int output[2];
	
	pipe(input);
	pipe(output);

    printf("fork is running\n");
	pid_t id = fork();
	if(id < 0)
	{
		print_log("fork",FALAT);
        printf("207 fork is failed");
		echo_error(sock,500);    // server fault  1;
	}else if(id == 0){ 		//child ->exec bin<cgi
		close(input[1]);
		close(output[0]);
		
		print_log("id == 0",FALAT);
		//进行文件描述符的重定向；
		dup2(input[0],0);
		dup2(output[1],1);
		close(sock);		//child proc close the sock;

		//程序替换只替换目标代码和数据，不会改变环境变量
		
		//arguments through env val pass;
		sprintf(dir_env,"DIR_ENV=%s",dir);
		putenv(dir_env);
		print_log("execl is running\n",FALAT);
		int ret = execl(path,path,NULL);
        if(ret == -1)
        {
		    print_log("execl is fault\n",FALAT);
        }
	}//else if

	else//father
	{
		close(input[0]);
		close(output[1]);
		
		char ch = '\0';
		while(read(output[0],&ch,1) > 0)
		{
			send(sock,&ch,1,0);
		}
		print_log("father is done\n",FALAT);
		waitpid(id,NULL,0);
        print_log("child is wait success!",FALAT);
	}//else
    return 0;
}

//判断请求路径的有效,并处里
int deal_path(int sock,struct stat st,char* method,char *path)
{
    printf("DEBUG: running deal_path");
	int ret = 0;   //return status;
	
	int fd = open(path,O_RDONLY);
	if(fd < 0)
	{
		print_log("open file failed\n",WARING);
		ret = 6;
	}else{
		const char* response = "HTTP/1.0 200 OK\r\n";
	 	ret = send_response(sock,path,fd,st,response);
	}
	
	close(fd);
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

	int i = 0;
	int j = 0;
	char c = buff[i];
	//GET /path?a=b&b=c HTTP/1.1

	if(readline(sock,buff,sizeof(buff)) < 0)
	{
		ret = 1;
		goto end;
	}

	printf("DEBUG: request line is : %s\n",buff);
	uri_point = buff;

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
		echo_error(sock,403);
		goto end;
	}

	
	//path = / ; method = GET/POST
	i = 0;
	while(*uri_point != ' ')
	{
		path[i++] = *uri_point++;
	}
	path[i] = '\0';

	char pathline[SIZE]; //keep path
	memset(pathline,'\0',sizeof(pathline));
	
	sprintf(pathline,"wwwroot%s",path);

    printf("cat pathline is %s\n", pathline);
	struct stat st;   //存储文件信息 
	i = strlen(pathline);
	if(pathline[i - 1] == '/' && strcasecmp(method,"post") != 0)
	{
		// wwwroot/ > return index.html
		//strcat(pathline,"index.html");
        //换成返回显示根目录下文件的格式
        printf("pathline is %s\n",pathline);
        int renu = cgi_deal(sock, pathline);
        printf("cig_deal is runned\n");
        return renu;
	}

	if(stat(pathline,&st) < 0)
	{
		echo_error(sock,404);
		ret = 2;
		return ret;
	}else if(S_ISDIR(st.st_mode)){
		//request a dir
		sprintf(pathline,"/");
        printf("%s is pathline\n", pathline);
	}
	else{  
		echo_error(sock,400);
		ret = 3;
		return ret;
	}


	ret = deal_path(sock,st,method,pathline);
	if(ret != 0)
	{
		print_log("deal_path is failed!\n",FALAT);
        return -1;
	}
    end:
        return -1;
}



//pthread deal funcation
int deal_accept_request(int connfd)
{
	if(headler_sock(connfd) != 0)
	{
		close(connfd);
		print_log("headler_sock is fault!\n",WARING);
        return 1;
	}
    printf("deal_accept_request is runned\n");
    return 0;
}
