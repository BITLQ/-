
.PHONY:all
all:httpd cgi login

httpd:main.cpp /home/test/http_protect/httpd.c
	g++ $^ -g -o $@ -lpthread

.PHONY:cgi
cgi:
	cd cgi/; make; make output; cd -

.PHONY:login
login:
	cd login/; make; make output; cd -

.PHONY:clean
clean:
	rm -rf httpd output cgi_math login_cgi; cd cgi; make clean; cd -;cd login; make clean; cd -
.PHONY:output
output:all
	mkdir -p output/
	cp httpd output/
	cp -rf log output/
	cp -rf conf output/
	cp -rf wwwroot output/
	mkdir -p output/wwwroot/cgi_bin
	cp -f cgi_math output/wwwroot/cgi_bin/
	cp -f login_cgi output/wwwroot/cgi_bin/
	cp -r httpd_ctl.sh output/
	cp -f dir_cgi.sh output/
