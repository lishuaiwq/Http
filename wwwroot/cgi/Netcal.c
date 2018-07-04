#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
//strtol
//
void mycal(char *buf)
{
	//	firsdata=10000000000&seconddata=20000000000
//	printf("%s",buf); 
   int x,y;
   sscanf(buf,"firsdata=%d&seconddata=%d",&x,&y);
   //printf("%d\n",x);  
 // printf("%d\n",y);  
  printf("<html>\n");		   
   printf("<body>\n");  
   printf("<h3>%d-%d=%d</h3>",x,y,x-y);   
   printf("<h3>%d*%d=%d</h3>",x,y,x*y);   
   printf("<h3>%d/%d=%d</h3>",x,y,x/y);   
   printf("</body>\n");
   printf("</html>\n");
} 
int main()
{
   char buf[1024]; 		
   if(getenv("METHOD"))
   {
      if(strcasecmp(getenv("METHOD"),"GET")==0)//如果是get方法的话
	  {
	          strcpy(buf,getenv("QUERY_STRING")); 
	  } else
	  {
	        int n=atoi(getenv("CONTENT_LENGTH")); //拿到post的方法
			int i=0;
			char c;
			for(;i<n;i++)
			{
			   read(0,&c,1); 
			   buf[i]=c; 
			} 
			buf[i]='\0';  
	  } 
   } 
   //上述的内容就是拿数据,拿的是请求报文中的数据并不能直接去使用！
   //下面需要提取我们需要的数据
   mycal(buf); 
   printf("%s",buf); 
   return 0;
} 
