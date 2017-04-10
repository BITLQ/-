
.PHONY:all
all:httpd cgi

httpd:/home/test/http_protect/httpd.c /home/test/http_protect/main.c
	gcc -o $@ $^ -lpthread

.PHONY:cgi
cgi:
	cd cgi/; make; make output; cd -

.PHONY:clean
clean:
	rm -rf httpd output cgi_math; cd cgi; make clean; cd -
.PHONY:output
output:all
	mkdir -p output/
	cp httpd output/
	cp -rf log output/
	cp -rf conf output/
	cp -rf wwwroot output/
	mkdir -p output/wwwroot/cgi_bin
	cp -f cgi_math output/wwwroot/cgi_bin/
	cp -r httpd_ctl.sh output/
