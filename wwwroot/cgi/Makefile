#ROOT=$(shell pwd) 
#INCLUDE=-I$(ROOT)./../../lib/include
#LIB=-L$(ROOT)./../../lib/lib
#LIB_NAME=-lmysqlclient

.PHONY:all
all:Netcal insertData selectData echoServer

echoServer:echoServer.c
	gcc -o $@ $^ 
netCal:netCal.c
	gcc -o $@ $^ 
selectData:selectData.c
	gcc -I../../lib/include  -L../../lib/lib -l mysqlclient -o $@ $^ 
insertData:insertData.c
	gcc -I../../lib/include  -L../../lib/lib -l mysqlclient -o $@ $^ 
.PHONY:clean
clean:
	rm -f insertData Netcal selectData echoServer
