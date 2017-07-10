
#include "my_sql.h"

//link
Mysql* mysql_qpi::mysql_connect()
{
	//MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag) 
	//
	Mysql*con =  mysql_real_connect(mysql,localhost,root,NULL,student,3306,NULL,0); 
	if(con == NULL)
	{
		std::cout<<"mysql_connect is failed!"<<endl;
		return false;
	}else{
		cout<<"mysql_connect is success!"<<endl;
	}

	return true;
}

void mysql_api::mysql_select_api()
{}

void mysql_api::mysql_update_api()
{}

bool mysql_api::mysql_insert_api(string data)
{
    string str("insert into student(id,name,sex,age) values(");
    str += data;
    str += ")"
    if(mysql_real_query(mysql, str.c_str(), str.size()))
    {
        cout<<"mysql_real_query is failed"<<endl;
        return false;
    }else{  //query is success
          return true;
    }
}

void mysql_api::mysql_drop_api()
{}
