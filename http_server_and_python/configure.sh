#!/bin/bash/

PATH_ROOT=$(pwd)
INCLUDE=$PATH_ROOT

#http src
SRC=$(echo ${INCLUDE}/*.c)
SERVER_NAME=httpd
LIB=-lpthread
cc=gcc

#cgi
CGI_PATH=$PATH_ROOT/cgi/
MATH_SRC=$(ls $CGI_PATH | grep 'math' | grep -E '.c$')
MATH_CGI_BIN=cgi_math

#cgi Makefile
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

#top Makefile
cat << EOF >Makefile

.PHONY:all
all:${SERVER_NAME} cgi

${SERVER_NAME}:${SRC}
	${cc} -o \$@ \$^ ${LIB}

.PHONY:cgi
cgi:
	cd cgi/; make; make output; cd -

.PHONY:clean
clean:
	rm -rf ${SERVER_NAME} output cgi_math; cd cgi; make clean; cd -
.PHONY:output
output:all
	mkdir -p output/
	cp ${SERVER_NAME} output/
	cp -rf log output/
	cp -rf conf output/
	cp -rf wwwroot output/
	mkdir -p output/wwwroot/cgi_bin
	cp -f cgi_math output/wwwroot/cgi_bin/
	cp -r httpd_ctl.sh output/
EOF

