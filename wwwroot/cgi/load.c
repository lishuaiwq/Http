#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mysql.h>
#include<unistd.h>
char *change(char *s)
{
     char *new_name = malloc(25);
     char z[3] = {0};
     char a;
     char *p;
     char *m;
     int i = 0;
     int k;
     char c;
     p = s;
     while(*p)
     {
         if(*p == '%')
         {
             z[0] = *(p+1);
             z[1] = *(p+2);
             sscanf(z,"%x",&k);
             new_name[i++] = k;
             p = p+3;
         }
         else if(*p == '+')
         {
             new_name[i++] = ' ';
             p++;
         }
         else
         {
             new_name[i++] = *p;
            p++;
         }
     }
     return new_name;
 }
void judge(char *name,char* passwd)
{		
printf("<html>\n");
printf("<head>\n");
printf("<meta charset=\"utf-8\">\n");
//printf("<meta http-equiv=\"refresh\" content=\"3;url=http://192.168.11.128:9526/function.html\">");
printf("</head><body background=\"../images/index.jpg\">\n"); 
printf("<h1>登录界面<h2>"); 
   MYSQL* mysql_fd=mysql_init(NULL);//初始化mysql
   if(mysql_fd==NULL)
   {
      printf("init failed:%s\n", mysql_error(mysql_fd));
	   return;
    }
   else
   {
    //  printf("......\n"); 
   } 
   if(mysql_real_connect(mysql_fd,"127.0.0.1","root", "","http",0,0,0)==NULL)//连接mysql
   {
     printf("connect failed:%s\n", mysql_error(mysql_fd));
	  return;
   } 
   //printf("连接成功\n"); 
   char sql[1024];
   sprintf(sql,"select * from user_data where name=\"%s\"",name);
//printf("sql=%s\n",sql); 
 mysql_query(mysql_fd,sql); //使用mysql这个语句
 
   MYSQL_RES *res=mysql_store_result(mysql_fd);
   int row=mysql_num_rows(res);//获取行数,如果没有查到的话，获取的行数是0行
   int col=mysql_num_fields(res);//获取列数
 //  printf("%d   %d\n ",row,col); 
    
   if(row==0)
   {
      printf("登录失败,未注册!\n"); 
   }
   int i=0;
  char *arr[4];  
   for(i=0;i<row;i++)//一行一行读取数据库
   {
    MYSQL_ROW rowdata=mysql_fetch_row(res);//获取行中的内容,二维数组 
		int j=0;
		for(;j<col;j++)//读取一行，一次读取一个字段
		{
			  arr[j]=rowdata[j];   
		}
   }
   //printf("%s  %s \n",arr[0],arr[2]);
  // printf("%s  %s \n",name,passwd);  
   if(strcmp(arr[0],name)==0&&strcmp(arr[2],passwd)==0)
   {
     printf("<h1>恭喜你登录成功</h1>\n"); 
   } else
   { 
     printf("<h1>登录失败，账号或者密码错误</h1>\n"); 
   } 
printf( "<a href=\"#\" onClick=\"javascript :history.back(-1);\">\"返回上一页面\"</a>");
printf("</body></html>\n"); 
  mysql_close(mysql_fd); 
} 
int main() 
{
   char data[1024];
   if(getenv("METHOD"))
   {
      if(strcasecmp("GET",getenv("METHOD"))==0)
      {
	     strcpy(data,getenv("QUERY_STRING")); 
	  } else
	  {
	        int n=atoi(getenv("CONTENT_LENGTH"));
			int i=0; 
			for(i=0;i<n;i++)
			{
			   read(0,data+i,1); 
			} 
          data[i]=0;//得到了数据   
	  } 
   } 
//   printf("data=%s\n",data); 
   char *name;
   char *passwd;
 //在这里我只要姓名
 //char dat[]="name=lishuai&passwd=123456";
 strtok(data,"=&");//得到name
name=strtok(NULL,"=&");//的到lishuai
strtok(NULL,"=&");//等到passwd
passwd=strtok(NULL,"=&"); 
//printf("<html><head><meta charset=\"utf-8\"></head><body><h1>hehe</h1></body></html>");
//printf("name=%s   passwd=%s",name,passwd);
char arr[1024];
strcpy(arr,name);
name=change(arr); 
  judge(name,passwd);//将名字传过去 
   return 0;
} 
      
