#include "mysql.h"

#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<string>

#define _SIZE_ 1024


static void math_data(char* data)
{
    
    while(*data++ != '?'){}
    string str;
    string arr[4];
    int i = 0;

    while(*data != '\0')
    {
        if(*data == '=')
        {
            arr[i] += "\'";
            ++data;
            while(*data != '\0' && *data != '&')
            {
                arr[i] += *data;
            }

            if(i != 3)
            {
                arr[i] += "\',";
            }else{
                arr[i] += "\'";
            }

            if(*data == '&')
            {
                ++i;
            }
        }

        ++data;
    }

    for(int i = 0; i < 4; ++i)
    {
        str += arr[i];
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
			return 3;
		}
	}
	//cd cgi 通用网关协议

	//deal data
	//printf("CGI data is %s\n",content_data);
	
	math_data(content_data);
	
}
