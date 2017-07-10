
ROOT_PATH=$(shell pwd)
INCLUDE_PATH=-I$(ROOT_PATH)/../lib/include
LIB_PATH=-L$(ROOT_PATH)/../lib/lib
lib=-lmysqlclient
cc=g++

#.PHONY:all
#all:cgi_insert cgi_select

cgi_insert:cgi_insert.cpp mysql_api.cpp
	$(cc) -o $@ $^ $(INCLUDE_PATH) $(LIB_PATH) $(lib)
#cgi_select:cgi_select.cpp mysql_api.cpp
#	$(cc) -o $@ $^ $(INCLUDE_PATH) $(LIB_PATH) $(lib)
#mysql_api: mysql_api.cpp
#	$(cc) -o $@ $^ $(INCLUDE_PATH) $(LIB_PATH) $(lib)

.PHONY:clean
clean:
	rm -f cgi_insert cgi_select
