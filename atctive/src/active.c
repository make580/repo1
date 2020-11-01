
#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<memory.h>
#include<signal.h>
#include<time.h>
#include<errno.h>
#include<pthread.h>
#include"active.h"
#include<arpa/inet.h>
#include<time.h>
#include<sys/time.h>
#include<signal.h>
AddrData *addrdatasta=NULL;
/*
*	功能：把ip地址和端口号打包，并返回；
*	参数：IPaddr是个地址，port是个端口号
*	返回值：错误返回NULL
*/
void *setIPaddrPort(const char *IPaddr,const int Port)
{
	if(IPaddr==NULL||Port==0){
		myperror("IPaddr or Port is NULL...");
		return NULL;
	}
	AddrData *p = NULL;
	p = (AddrData *)malloc(sizeof(AddrData));
	if(p == NULL){
		myperror("malloc failed...");
		return NULL;
	}
	memset(p,0,sizeof(AddrData));
	//设置server地址
	p->addr.sin_family = AF_INET;
	p->addr.sin_port = htons(Port);
	//p->addr.sin_addr.s_addr = INADDR_ANY;
	p->addr.sin_addr.s_addr = inet_addr(IPaddr);
	
	p->len = sizeof(p->addr);
		
	memset(p->addr.sin_zero,0,sizeof(p->addr.sin_zero));

	return p;
}
/*
*	功能：搭建通讯server
*	参数：一个AddrData
*	返回值：成功返回一个socket，错误小于0
*/
int serverhost()
{
	fd_set rset,allset;		//定义读集合，备份集合allset
	int ret,maxfd=0;
	int i,connfd;
	struct sockaddr_in clie_addr;		
	socklen_t clie_addr_len;
	if((addrdatasta->sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		myperror("create socket failed...");
		return -1;
	}
	int opt = 1;
	setsockopt(addrdatasta->sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	if(bind(addrdatasta->sockfd,(struct sockaddr*)&addrdatasta->addr,addrdatasta->len)<0){
		myperror("bind failed...");
		return -1;
	}
	//设置监听列表
	if(listen(addrdatasta->sockfd,100)<0){
		myperror("listen failed...");
		return -1;
	}

	maxfd = addrdatasta->sockfd;	//最大文件描述符
	
	FD_ZERO(&allset);		//清空监听集合
	FD_SET(addrdatasta->sockfd,&allset);		//将代监听fd添加到监听集合中
	

	while (1)
	{
		rset = allset;		//备份
		ret = select(maxfd+1,&rset,NULL,NULL,NULL);		//使用select监听
		if(ret<0){
			perror("select error...");
		}

		if(FD_ISSET(addrdatasta->sockfd,&rset)){		//listenfd满足监听的读事件
			clie_addr_len=sizeof(clie_addr);
			connfd=accept(addrdatasta->sockfd,(struct sockaddr*)&clie_addr,&clie_addr_len);	//建立链接...不会阻塞
			if(connfd<0)continue;
			FD_SET(connfd,&allset);		//将信产生的fd，添加到监听集合中，监听数据读写事件
			if(maxfd<connfd)maxfd = connfd; 	//修改maxfd
			printf("\nThere is a new client connection ipaddr:%s，port：%d...\n",inet_ntoa(clie_addr.sin_addr),
					ntohs(clie_addr.sin_port));
			if(ret == 1)continue;	//说明select只返回了一个，并且是listenfd，后续执行无须执行
		}

		for(i=addrdatasta->sockfd+1;i<=maxfd;i++){		//处理满足读事件的fd
			if(FD_ISSET(i,&rset)){		//找到满足读事件的fd
				readywrite(i,&rset,&allset);
			}
		}

	}
	return 0;
}
/*
*	功能：读写操作
*/
void readywrite(int fd,fd_set *rset,fd_set *allset)
{
	extern int errno;
	char *str_time=NULL;
		//客户端的读写数据
	char buff_read[100];
	char buff_write[200];
	char buff_time[100];
	memset(buff_time,0,100);
	memset(buff_read,0,100);
	memset(buff_write,0,100);
	str_time = fun_time();
	strcpy(buff_time,str_time);
	//用完释放fun_time返回的内存
	if(str_time!=NULL)free(str_time);
	//时间显示
	//printf("%s\n",buff_time);
	int size;
	if((size=recv(fd,buff_read,100,0))<0){
		myperror("protocal error...");
		
		return;
	}else if(size == 0){//客户断开退出
		perror("client interrupt...");
		close(fd);
		FD_CLR(fd,allset);	
		return;
	}else{
		//buff_read[size] = '\0';
		//printf("%s>>>%d\n",buff_read,size);
		if(replyg_msg(buff_read)==1234){
			sprintf(buff_write,"%s\n%s",buff_read,buff_time);
		}else{
			sprintf(buff_write,"error\n%s",buff_time);
		}
		if((size=send(fd,buff_write,100,MSG_NOSIGNAL))<0){
			if(errno == EPIPE){
				myperror("send pipe error...");
				close(fd);
				FD_CLR(fd,allset);	
			}else{
				myperror("protocal error...");
			}
		}
	}
}
/*
*	功能：接收消息并返回消息
*	参数：AddrData类型
*	返回值：成功大于0，失败小于0；
*/
int replyg_msg(char *buff)
{
	if(strncmp(buff,CHECKDATA,10)==0){
		return 1234;
	}
	return 0;
}



/*
*	功能：打印信息
*/
void myprintf(const char *str)
{
	printf("%s\n",str);
}
/*
*	功能：错误信息打印和提示	
*/
void myperror(const char *str)
{
	printf("%s",str);
	perror("");
	
}
/*
*	功能：清内存
*/
void myclose()
{
	if(addrdatasta!=NULL){
		close(addrdatasta->sockfd);
		free(addrdatasta);
	}
}
/*
*	功能：active总接口
*/
int  mainactive(int argc,char** argv)
{
	//没有ip地址时退出进程
	if(argc<3){
		myprintf("addr er port use error...");
		return 1;
	}
	

	int ret;

	//接受进程关闭信号
	if(signal(SIGINT,sig_handler)==SIG_ERR){
		myperror("signal sigint error...");
		return 1;
	}
	if(signal(SIGTERM,sig_handler)==SIG_ERR){
		myperror("signal sigterm error...");
		return 1;
	}
	//设置ip地址和端口号
	addrdatasta = setIPaddrPort(argv[1],atoi(argv[2]));
	if(addrdatasta==NULL){
		myperror("setIPaddrPort return NULL...");
		return 1;
	}
	//srever通讯
	ret=serverhost(addrdatasta);
	if(ret != 0){
		myperror("server failed...");
		return 1;
	}	
	myclose();	
	return 0;
}
/*
*	功能：时间打印
*	返回值：一个malloc的char类型（用完注意内存释放）
*/
char *fun_time()
{
	time_t dwCurTime1;
	dwCurTime1 = time(NULL);
   
	struct tm* pTime;
	pTime = localtime(&dwCurTime1);
    
	char *p;
	p = (char*)malloc(100);
	memset(p,0,100);
	strftime(p, 100, "time: %Y,%b %d %a, %r", pTime);
	   
	return p;
}
/*
*	功能：进程结束清空free内存
*/
void sig_handler(int signo)
{
	if(signo ==SIGINT){
		myprintf("End of the process...");
		if(addrdatasta!= NULL){
			//释放内存
			myprintf("free ok...");
			myclose();
		}
		exit(1);
	}	
}
