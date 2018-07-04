#include<stdio.h>
#include<signal.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<pthread.h>
#include<string.h>
#include<sys/stat.h>
//#define Debug
#define MAX  1024 
#define HOME_PAGE "index.html"
int sum=1;
int startup(int port)//用来获取一个监听套接字
{
   int sock=socket(AF_INET,SOCK_STREAM,0);
   if(sock<0)
   {
      perror("use socket");//其实这里不应该直接打印错误，而是将错误打印到日志里面去
	  exit(2); 
   } 
   int opt=1;
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));//端口复用 

   struct sockaddr_in local;
   local.sin_family=AF_INET;
   local.sin_addr.s_addr=htonl(INADDR_ANY);//访问本地的任意IP
   local.sin_port=htons(port);
    
   if(bind(sock,(struct sockaddr*) &local,sizeof(local))<0)
   {
      perror("use bind");
	  exit(3); 
   } 
   if(listen(sock,5)<0)
   {
      perror("use listen");
	  exit(4); 
   }
   return sock;
} 

int get_line(int sock,char line[],int n)//按行获取，将获取的内如放入指定的缓冲区，
		                                //给出缓冲区大小
{
            //回车换行有许多中表示，这里我们要处理多种类型的回车换行
		//一般来说回车换行有如下几种\n \r\n \r我们都要处理，因为我们要按行处理这里的这些
		//东西是行分隔符号所以我们都要去处理，处理方式一般将上述三种形式统一处理为\n
		//在这里可以将整个http请求看成一个行，放在接收缓冲区里面，只不过其中有回车换行来间隔
		//
         int c='A';//默认为‘A’
		 int i=0;
		 ssize_t s=0;
		 while(i<n-1&&c!='\n')
		 {
		    ssize_t s=recv(sock,&c,1,0);//读数据一次读一个字符,flag首次在这里设置为0 
			if(s>0)//读取正常
			{
			    if(c=='\r') //判断读取到的这个字符,
				{
					  //\r-->\n  \r\n-->\n
					  //在这里强调一点，当读取到\r的时候我们需要继续往后读取一个字符
					  //如果后面的这个字符是\n证明我们读取到这个行的末尾，如果不是\n则
					  //说明我们读取到了下一行的第一个字符，所以这里我们要处理避免
					  //读走下一行的第一个字符
                    if(recv(sock,&c,1,MSG_PEEK)>0)//使用recv具有的窥探功能避免出现上述的问题
					{
					  //补充在这里还会有一个问题就是\r后面如果没有字符的话，那么recv就阻塞
					  //所以我们应该.....后面再说怎么处理。。。
					    if(c!='\n')//如果发现后面的字符不是'\n'，说明已经读取到了此行的末尾 
						{
						  c='\n';//直接将c='\n'        
						} 
						else//下一个读取的就是'\n'
						{
						   recv(sock,&c,1,0);//如果是'\n'直接读取'\n' ，相当于将c='\n'了   
						} 
					} 
				} 
				//不等于'\r',读取到正常字符, 
				
				   line[i++]=c; //如果读取到的是'\n'，则将'\n'放进去以后推出循环  
			   
			} 
			else
			{
				
			      return -1;//读取失败
			} 
		 }
		 line[i]='\0';//最后将放入'\0'，此时读取了完整的一行请求   
		 return i;
} 

void clear_headr(int sock)//清理缓冲区剩余内容的函数
{
		char line[MAX]; 
   do
   {
       get_line(sock,line,sizeof(line));
//	printf("%s",line);//打印读取的行的内容 
   }while(strcmp(line,"\n")!=0);//最后读取到的line的内容是\n就证明我们读取完了整个报文  

} 
void echo_www(int sock,char *path,int size,int *err)//调用这个函数绝对是返回一个网页文件
{
      //在这里我们需要把缓冲区中的东西清理完，因为之前我们只是读取了头部一行而已，
     clear_headr(sock); 	  
    int fd=open(path,O_RDONLY);//打开请求的文件,将读取的二进制文件给其写回去
	if(fd<0)
	{
	  perror("open index.html"); 
	   *err=404;
	   return;
	} 
	char line[MAX];
	sprintf(line,"HTTP/1.0 200 OK\r\n");
	printf("line %s",line); 
    send(sock,line,strlen(line),0);//把状态行发送回去 
 //sprintf(line,"Content-Type: text/html\r\ncharset:UTF-8\r\n");
  // send(sock,line,strlen(line),0);//把状态行发送回去 
    sprintf(line,"\r\n"); 
	send(sock,line,strlen(line),0); 
	printf("line=%s",line); 
	sendfile(sock,fd,NULL,size);//两个文件之间直接拷贝不需要进过缓冲区,直接将文件拷贝过去
	printf("sock=%d fd=%d size=%d",sock,fd,size); 
	close(fd); 
} 
void echo_error(int code)
{
    switch(code){
	   case 404:
			   break;
	   case 501:
			   break;
	   default:
			   break;
	}  
} 
int exe_cgi(int sock,char *path,char method[],char* query_string)//要么是
		                                                          //post方法要么是带参数的get 方法
{

		char line[MAX]; 
		int content_length=-1;

		char method_env[MAX/2];//方法
		char query_string_env[MAX];//get参数
		char content_length_env[MAX/16];//参数大小 
     if(strcasecmp(method,"GET")==0)
	 {
           clear_headr(sock); //在这里只读取了头部行，所以要将剩下的读完	  
	 //如果是GET方法证明get带参数了
	 } else
	 {//否则是POST方法,则参数在正文部分，则需要将正文部分读出来
	   do
       {
         get_line(sock,line,sizeof(line));//读头部的内容
		 if(strncmp(line,"Content-Length: ",16)==0)
		 {
		     content_length=atoi(line+16);//获取数据字段的大小
		 } 
       }while(strcmp(line,"\n")!=0);//最后读取到的line内容是\n就证明我们读取完了整个报文状态行和头部  
	     if(content_length==-1)//有问题
		 {
		     return;
		 } 
	 }

	sprintf(line,"HTTP/1.0 200 OK\r\n");
	printf("line %s",line); 
    send(sock,line,strlen(line),0);//把状态行发送回去 
    sprintf(line,"\r\n"); 
	send(sock,line,strlen(line),0); 
	 //不管是get方法还是post方法我们都已经获取了他们的参数内容
     //只要有参数过来就要处理，既然处理就要执行可执行程序，则需要创建子进程
     int input[2];//
	 int output[2];//
	 pipe(input);
	 pipe(output); 
	 pid_t id=fork();
	 if(id<0)
	 {
	    return 404;
	 } else if(id==0)//子进程
	 {
	   //在这里子进程需要父进程给其的参数，父进程需要获得子进程执行的结果
	   //父进程获得结果就用管道，子进程需要的参数也通过管道获得
	   close(input[1]);//写端关闭,子进程用input读
       close(output[0]);//关闭读端 ,父进程用outpou写
	   //我们应该将写和读重定位到大家都知道文件描述符中，这样方便去读写,可以实现直接标准输入写到管道，读数据从管道读
	   dup2(input[0],0);//新的是旧的一份拷贝，最后和旧的一致 
	   dup2(output[1],1); 


		sprintf(method_env,"METHOD=%s",method);
		putenv(method_env);
		if(strcasecmp(method,"GET")==0)
		{
		sprintf(query_string_env,"QUERY_STRING=%s",query_string);
	    putenv(query_string_env);//只有get方法的时候才把这个导入环境变量 
		//printf("hehe\n"); 
		} 
		else//post方法
		{
		sprintf(content_length_env,"CONTENT_LENGTH=%d",content_length);
	   	putenv(content_length_env); 
		} 
	   //接下俩要替换可执行程序，由path指向
	  // printf("AAAAAAAAAAAAAA\n"); 
	   execl(path,path,NULL);//第一个参数执行的文件的路径，第二个执行的命令
	   //printf("execl is start");
	   //printf("%s\n",getenv("METHOD")); 
	   //printf("%s\n",getenv("QUERY_STRING")); 
	   exit(1); 
	 }else//父进程,线程在等，不会将服务器阻塞
	 {
	   close(input[0]);
	   close(output[1]);
      
	   char c;
	   if(strcasecmp(method,"POST")==0)//如果是post从读数据写给可执行程序
	   {
			  printf("读取post的数据\n");  
	         int i=0;
			 while(i++<content_length)
			 {
			     read(sock,&c,1); 
				 write(input[1],&c,1); 
			 } 
	   } 
	   printf("........准备读取cgi写回的数据...\n"); 
	   while(read(output[0],&c,1)>0)
	   {
			 //printf("写给浏览器\n");   
	         send(sock,&c,1,0); 
	   } 
	   printf("BBBBBBBBBBB\n"); 
       //父进程需要把什么东西告诉子进程//method get[query_string]  post[content-length] 
	   //在这里可以将这些东西写入管道中，但是不好区分，所以我们使用环境变量
	   waitpid(id,NULL,0); 

       close(input[1]);
	   close(output[0]); 
	 } 
    return 200;
} 
static void* handler_request(void* arg)//处理请求的函数，线程函数
{
    int sock=(int)arg;
	char line[MAX]; //读取的求情报文的一行
	int errCode=202;//错误码
	char method[MAX];//请求方法
	char url[MAX]; //请求的资源
	char path[MAX];//资源路径 
	char *query_string=NULL;
     int cgi = 0; 
#ifdef Debug	//下面这是测试读取请求报文的测试代码所以在这里我们暂时不需要
   do
   {
       get_line(sock,line,sizeof(line));
	   printf("%s",line);//打印读取的行的内容 
 }while(strcmp(line,"\n")!=0);//最后读取到的line的内容是\n就证明我们读取完了整个报文  
#else
   //如果没有#define debug那么就使用下面这段的程序
    if(get_line(sock,line,sizeof(line))<0)
	{
	//读取请求行失败
	    errCode=404;
		goto end; 
	}
   //走到这里证明读取成功了，那么我们就需要知道请求方法和请求的资源
   //首先来获取我们的方法get/post
   int i=0;
   int j=0;
   while(i<sizeof(method)-1&&j<sizeof(line)&&!isspace(line[j])/*line[i]!=' '*/)
   {
       method[i]=line[j];
	   i++;
	   j++;
   } 
   method[i]='\0';//得到了其中的方法
   //得到方法以后要判断我们的是什么方法,同时注意需要处理方法的大小写问题，因为大小写都一样
   //所以我们用strcasecmp()这个函数可以忽略比较的字符串的大小写
   if(strcasecmp(method,"GET")==0)
   {
        printf("这是get方法\n")  ;//是get就什么都不做 
   }else if(strcasecmp(method,"POST")==0)//post方法假定一定有数据，有数据就一定要用cgi
   {
       cgi=1; //只要有数据过来就调用cgi 
   } 
   else
   {
       errCode=404;
	   goto end;
   } 
   while(j<sizeof(line)&&isspace(line[j]))
   {
		   //因为上述获取方法字段后j指向的位置包括其后面可能会有连续很多的空格
		   //所以我们应该这些空格全部过滤掉，然后再去读取我们要的资源
      j++;
   } 
   //获取资源定位
   i=0;
   while(i<sizeof(url)-1&&j<sizeof(line)&&!isspace(line[j]))
   {
      url[i]=line[j];
	  i++;
	  j++;
   } 
   //一般来说去我们服务器获取的URL只有从/根目录开始的东西，如果带参数的话而且是
   //get方法那么?左侧是你要访问的资源，?的右侧是你要给资源的参数。请求的资源有可能还是
   //个可执行的程序,所以我们的url中很有可能包含了参数
   url[i]='\0';
  
   //我们要对url进行判断分割，将url和query_string分割开来
   if(strcasecmp(method,"GET")==0)//只有get方法才进行切割
   {
      query_string=url;
	  printf("%s\n",query_string); 
	  while(*query_string)
	  {
	      if(*query_string=='?')
		  {
		    
		  *query_string='\0';//url前半部分表示资源定位
		  query_string++;//让query_string指向参数
           cgi=1;
	printf("cgi=%d\n",cgi); 
		   break;
		  } 
		  query_string++;
	  } 
	  printf("en jinlaimei",cgi);
   } 
   //现在方法、资源、参数都准备好了，就需要判断我们请求的资源是否存在
  //url->一般是/a/b/c.html，我们需要将其改造成wwwroot/a/b/c.html
  sprintf(path,"wwwroot%s",url);//将wwwroot和url中的字符串拼接起来放在path中
  if(path[strlen(path)-1]=='/')//说明url中是/,即请求主页那么就给他主页
  {
     strcat(path,HOME_PAGE); 
  }
  printf("  method=%s  path:%s\n",method,path); 
  //判断请求的资源是否存在,请求的资源即文件在path中
  //  printf("cgi=%d\n",cgi); 
//	printf("aaaaa\n"); 
  struct stat st;
if(stat(path,&st)<0)
{
	  printf("有问题\n");  	
      //走进来文件不存在
	  errCode=404;
	  goto end;
}
else//找到对应的文件了，在这里有可能你访问的文件是一个可执行文件,这样的话就要用cgi的方式
{

	printf("cgi=%d   找到对应的文件\n",cgi); 
	if(S_ISDIR(st.st_mode))//如果是个目录，则返回目录下面的默认页面
	{
	      strcat(path,HOME_PAGE); 
	} 
	else//如果不是目录 
	{
			//判断是否为可执行文件
			if((st.st_mode&S_IXUSR)||(st.st_mode&S_IXGRP)||(st.st_mode&S_IXOTH))
			 {
			    cgi=1;
			 } 
	} 
//	printf("last cgi=%d\n"); 
	if(cgi)//证明有数据或者是可执行的文件
    {
		printf("cgi调用了\n"); 	
        errCode=exe_cgi(sock,path,method,query_string);//要么是post方法要么是带参数的get 方法
	}
	else//不是cgi,是get方法没有参数
	{
	 printf("正常页面显示被调用了\n"); 
	 echo_www(sock,path,st.st_size,&errCode);//返回我们的信息就行了 
	} 
} 
#endif


end:
   if(errCode!=200)
		echo_error(errCode); 
   close(sock); 
} 
int main(int argc,char* argv[])
{
    if(argc!=2){
		perror("use port errno");//这里可以封装成一个单独的函数去使用 	
	    return 1;
	} 
    int listen_fd=startup(atoi(argv[1]));//获取一个监听的套接字

    signal(SIGPIPE,SIG_IGN);//忽略SIGPIPE。因为往无效的套接字中写就会收到这个 


    printf("listenfd=%d\n",listen_fd); 
	for(;;)//循环建立连接
	{
	   struct sockaddr_in client;
       socklen_t len=sizeof(client); 
	   int newfd=accept(listen_fd,(struct sockaddr*)&client,&len);
	   if(newfd<0)
       {
	      perror("use aeecpt");
		   continue;//继续获取链接
	   }
	   printf("第%d次接收浏览器请求\n",sum++); 
	   pthread_t id;
	   pthread_create(&id,NULL,handler_request,(void*)newfd);//处理请求
	   pthread_detach(id);//线程分离 
	   pthread_join(id,NULL);//主线程用来回收线程 
	} 
} 


