#!/bin/bash/

PATH_ROOT=$(pwd)
INCLUDE=$PATH_ROOT

#http src
SRC=$(echo ${INCLUDE}/*.c)
SERVER_NAME=httpd
LIB=-lpthread
cc=gcc
cpp=g++

#cgi
CGI_PATH=$PATH_ROOT/cgi/
LOGIN_PATH=$PATH_ROOT/login/
MATH_SRC=$(ls $CGI_PATH | grep 'math' | grep -E '.c$')
MATH_CGI_BIN=cgi_math
LOGIN_SRC=$(ls $LOGIN_PATH | grep 'login_cgi.cpp')
LOGIN_CGI_BIN=login_cgi

#math_cgi Makefile
cat <<EOF >$CGI_PATH/Makefile
${MATH_CGI_BIN}:${MATH_SRC}
	$cc -o \$@ \$^
.PHONY:clean
clean:
	rm -f $MATH_CGI_BIN
.PHONY:output
output:
	cp $MATH_CGI_BIN ..
EOF

#login_cgi Makefile
cat <<EOF >$LOGIN_PATH/Makefile
${LOGIN_CGI_BIN}:${LOGIN_SRC}
	$cpp -o \$@ \$^
.PHONY:clean
clean:
	rm -f $LOGIN_CGI_BIN
.PHONY:output
output:
	cp $LOGIN_CGI_BIN ..
EOF
    
#top Makefile
cat << EOF >Makefile

.PHONY:all
all:${SERVER_NAME} cgi login

${SERVER_NAME}:main.cpp ${SRC}
	${cpp} \$^ -o \$@ ${LIB}

.PHONY:cgi
cgi:
	cd cgi/; make; make output; cd -

.PHONY:login
login:
	cd login/; make; make output; cd -

.PHONY:clean
clean:
	rm -rf ${SERVER_NAME} output cgi_math login_cgi; cd cgi; make clean; cd -;cd login; make clean; cd -
.PHONY:output
output:all
	mkdir -p output/
	cp ${SERVER_NAME} output/
	cp -rf log output/
	cp -rf conf output/
	cp -rf wwwroot output/
	mkdir -p output/wwwroot/cgi_bin
	cp -f cgi_math output/wwwroot/cgi_bin/
	cp -f login_cgi output/wwwroot/cgi_bin/
	cp -r httpd_ctl.sh output/
    cp -f dir_cgi.sh output/
EOF

