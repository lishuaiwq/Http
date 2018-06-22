http:httpd.c
	gcc -o $@ $^ -lpthread
.PHONY:clean
clean:
	rm -rf http

