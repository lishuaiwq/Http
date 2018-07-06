#include<stdio.h>
#include<mysql.h>
#include<string.h>
#include<iconv.h>

char *change(char *s)//字符转换函数
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
void insert_data(char* name ,char* sex,char *passwd) 
{
   printf("<html><head><meta charset=\"utf-8\">\n");
   printf("</head><body background=\"../images/index.jpg\">\n"); 
   MYSQL* mysql_fd=mysql_init(NULL);//初始化mysql
   if(mysql_fd==NULL)
   {
      printf("init failed:%s\n", mysql_error(mysql_fd));
	   return;
    }
   else
   {
  //    printf("......\n"); 
   } 
   if(mysql_real_connect(mysql_fd,"127.0.0.1","root", "","http",0,0,0)==NULL)//连接mysql
   {
   //   printf("connect failed:%s\n", mysql_error(mysql_fd));
  // printf("连接成功\n"); 
	  return;
   } 
 //   printf("connect mysql success!\n"); 
//printf("name=%s sex=%s phone=%s\n",name,sex,phone); //往数据里面写可以\n

   char sql[1024];
   sprintf(sql,"insert into user_data(name,sex,password) values(\"%s\",\"%s\",\"%s\");",name,sex,passwd); 
//printf("数据库=%s",sql);
printf("name=%s sex=%s passwd=%s\n",name,sex,passwd); //往数据里面写可以\n
//const char* sql="insert into student_info(name,sex,phone) values(\"zhangsan\",\"man\",\"123\")";
  int ret=mysql_query(mysql_fd,sql);
  if(ret<0)
  {
     perror("use mysql_query");
	 exit(1); 
  } 
 if(name==NULL||sex==NULL||passwd==NULL)
 {
 printf("<h3>注册失败。</h3>\n"); 
 }else
  { 
  printf("<h3>恭喜你，注册成功,注册信息如下表！</h3>\n");
  } 
  printf("<table border=\"2\">\n");
  printf("<tr><td>name</td><td>sex</td><td>passwd</td><td>id</td></tr>\n"); 
  printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>",name,sex,passwd);
  printf("</table>"); 
  printf( "<a href=\"#\" onClick=\"javascript :history.back(-1);\">\"返回上一页面\"</a>");
  printf("</body>\n");
  printf("</html>\n");  
  mysql_close(mysql_fd); 
} 
int main()
{
//先从浏览器中拿数据 
     char data[1024]; 
     if(getenv("METHOD"))//方法存在在去判断是get还是Post
	 {
	    if(strcasecmp("GET",getenv("METHOD"))==0)//如果是get方法的话
		{
		    strcpy(data,getenv("QUERY_STRING")); 
		} else//post方法
		{
		    int content_length=atoi(getenv("CONTENT_LENGTH"));
			int i=0;
			for(;i<content_length;i++)
			{
			      read(0,data+i,1); 
			} 
			data[i]=0; 
		} 
	 } 
//        printf("arg:%s\n",data); 
//name=%C0%EE%CB%A7&sex=%C4%D0&phone=200
//		char name[1024];
		//char sex[1024]={};
		//char phone[1024]={};
	char *name;
		char *sex;
		char *passwd;
		strtok(data,"=&");//name
//		strcpy(name,strtok(NULL,"=&"));//lishuai
		name=strtok(NULL,"=&");//lishuai
		strtok(NULL,"=&");//sex
		//strcpy(sex,strtok(NULL,"=&"));//n
		sex=strtok(NULL,"=&");//n
		strtok(NULL,"=&");//phone
	     passwd=strtok(NULL,"=&"); //123445
//name="%E7%8E%8B%E5%A4%A7%E5%8F%91";
// sex="男";
// phone="123456";
 char p1[1024];strcpy(p1,name); 
 char p2[1024];strcpy(p2,sex); 
char p3[1024];strcpy(p3,passwd);
//urldecode(p1);
//urldecode(p2);
//urldecode(p3);
char *arr1;
char *arr2;
char *arr3; 
arr1=change(p1);
arr2=change(p2);
arr3=change(p3); 
//printf("name=%s sex=%s phone=%s\n",arr1,arr2,arr3); //往数据里面写可以\n
 insert_data(arr1,arr2,arr3); 
  
   return 0;
} 
