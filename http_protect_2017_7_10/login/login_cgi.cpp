/*************************************************************************
	> File Name: login_cgi.cpp
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Thu 29 Jun 2017 01:12:50 AM PDT
 ************************************************************************/

#include<iostream>
using namespace std;

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#define _SIZE_ 1024


static void math_data(char* data)
{
	assert(data);

	char *arr[3];
	int i = 0;
	char* start = data;

	while(*start)
	{
		if(*start == '=')
		{
			arr[i] = start+1;
			i++;
		}else if(*start == '&')
		{
			*start = '\0';
		}else
		{

		}
		start++;
	}
	arr[i] = NULL;

	char* user = arr[0];
	char* passwd = arr[1];

    if(strcasecmp(user, "lq") && strcasecmp(passwd, "123"))
    {
	    printf("<html>");
	    printf("<h1>");
        printf("dir all of file");
        system("ls /home/test/http_protect/wwwroot/Index/ | tee dir.txt");
	    printf("</h1>");
	    printf("</html>");
    }
    else{
        printf("error user or error passwd");
    }
}


int main()
{
	//getenv 获取环境变量
	char method[_SIZE_];
	char query_string[_SIZE_];
	char content_len[_SIZE_];
	char content_data[_SIZE_];
	
	//daemon(0,0);

	if(getenv("METHOD"))
	{
		strcpy(method,getenv("METHOD"));
	}else{
		//直接写到浏览器
		printf("METHOD is not exist\n");
		return 1;
	}

	if(strcasecmp(method,"GET") == 0)
	{
		if(getenv("QUERY_STRING") != NULL)
		{
			strcpy(content_data,getenv("QUERY_STRING"));
			//GET data -> data;
		}else
		{
			printf("QUERY_STRING is not exist!\n");
			return 2;
		}
	}else
	{		//post
		if(getenv("CONTENT_LENGTH"))
		{
			strcpy(content_len,getenv("CONTENT_LENGTH"));
			int len = atoi(content_len);
			
			int i = 0;
			char ch = '\0';
			for(; i < len; i++)
			{
				read(0,&ch,1);
				content_data[i] = ch;
			}

			content_data[i] = '\0';   //post ->data;
		}else
		{
			printf("CONTENT_LENGTH is not exist!\n");
			return 3;
		}
	}
	//cd cgi 通用网关协议

	//deal data
	//printf("CGI data is %s\n",content_data);
	
	math_data(content_data);
	
}
