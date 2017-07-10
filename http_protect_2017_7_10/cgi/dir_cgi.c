/*************************************************************************
	> File Name: dir_cgi.c
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Fri 07 Jul 2017 12:06:40 AM PDT
 ************************************************************************/


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
	printf("</html>");
}


int main()
{
	//getenv 获取环境变量
	char dir_path[_SIZE_];
	
	//daemon(0,0);

	ddif(getenv("DIR_ENV"))
	{
		strcpy(dir_env,getenv("DIR_ENV"));
	}else{
		//直接写到浏览器
		printf("METHOD is not exist\n");
		return 1;
	}

	//cd cgi 通用网关协议

	//deal data
	//printf("CGI data is %s\n",content_data);
	
	math_data(content_data);
	
}
