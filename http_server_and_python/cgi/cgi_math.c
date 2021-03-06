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

	int data1 = atoi(arr[0]);
	int data2 = atoi(arr[1]);

	//printf("data1 = %d\n",data1);
	//printf("data2 = %d\n",data2);
	printf("<html>");
	printf("<h1>");
	printf("%d + %d = %d\n",data1,data2,data1+data2);
	printf("</h1>");
	printf("<html>");
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
