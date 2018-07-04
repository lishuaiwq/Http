ROOT=$(shell pwd)
.PHONY:all
all:http cgi

http:httpd.c
	gcc -o $@ $^ -l pthread
cgi:
	cd $(ROOT)/wwwroot/cgi;make clean;make;cd -


.PHONY:output
output:
	mkdir -p output/wwwroot/cgi
	mkdir -p putput/wwwroor/lib
	cp http output
	cp -rf lib/lib output/lib
	cp -f wwwroot/*.html output/wwwroot
	cp -rf wwwroot/imags output/wwwroot
	cp -f  wwwroot/cgi/netCal output/wwwroot/cgi
	cp -f  wwwroot/cgi/selectData  output/wwwroot/cgi
	cp -f  wwwroot/cgi/insertData output/wwwroot/cgi
.PHONY:clean
clean:
	rm -rf http
	cd $(ROOT)/wwwroot/cgi;make clean;cd -
	rm -rf output

