/*************************************************************************
	> File Name: my_mysql.h
	> Author:   刘强
	> Mail: 849886839@qq.com 
	> Created Time: Wed 12 Apr 2017 01:17:48 AM PDT
 ************************************************************************/

#ifndef _MY_MYSQL_H
#define _MY_MYSQL_H

#include<iostream>
#include<mysql.h>

class sql_api
{
 public:
    sql_api()
    {
        mysql = mysql_init(NULL);
		if(mysql == NULL)
		{
			std::cout<<"mysql_init return NULL\n";
		}
    }

	void mysql_connect_api();
	void mysql_select_api();
	void mysql_update_api();
	void mysql_insert_api();
	void mysql_drop_api();

	~sql_api()
	{
		if(sql != NULL)
		{
			mysql_close(mysql);
		}
	}

	
 private:
    MYSQL * mysql;
};
#endif



