#!/bin/bash/

PATH_ROOT=$(pwd)
INCLUDE=$PATH_ROOT
SRC=$(echo ${INCLUDE}/*.c)
SERVER_NAME=httpd
LIB=-lpthread
cc=gcc

cat << EOF >Makefile
${SERVER_NAME}:${SRC}
	${cc} -o \$@ \$^ ${LIB}
.PHONY:clean
clean:
	rm -f ${SERVER_NAME} 
EOF

