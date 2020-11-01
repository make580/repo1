/*
*	通讯信息接收发送
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include "standby.h"
//用于进程内存释放

AddrFdData *addrdatasta =NULL;
/*
*	功能：设置ip地址
*	参数（IPaddr）:IP地址
*	参数（port1）；端口号
*	参数（port2）：本地端口号
*	返回值：ip地址
*/
void *setIPaddr_port(const char *IPaddr,const int port,const int myport) 
{

	if(IPaddr == NULL ||port == 0||myport==0){
		myperror("ipaddr or port is error...");
		return NULL;
	}
	AddrFdData *p = NULL;
	p = (AddrFdData*)malloc(sizeof(AddrFdData));
	if(NULL == p){
		myperror("malloc error...");
		return NULL;
	}
	memset(p,0,sizeof(AddrFdData));
	//设置buff和addr的长度大小
	p->len_read = 100;
	p->len_write = 100;

	//设置服务器IP地址
	p->addr.sin_family = AF_INET;
	p->addr.sin_port = htons(port);
	//p->addr.sin_addr.s_addr = INADDR_ANY;
	p->addr.sin_addr.s_addr = inet_addr(IPaddr);
	//客户端绑定端口
	p->myAddr.sin_family = AF_INET;
	p->myAddr.sin_addr.s_addr = INADDR_ANY;
	p->myAddr.sin_port = htons(myport);
	memset(&(p->myAddr.sin_zero), 0, sizeof(p->myAddr.sin_zero));
	//设置发送的数据要跟CHECKDATA一致
	strcpy(p->buff_write,CHECKDATA);
	p->len_addr = sizeof(p->myAddr);
	
	return p;
}
/*
*	功能：搭建client负责通讯
*	参数（IPaddr）:对方的IP地址等数据
*	返回值：对方在线返回1，不在线返回0
*/
int client_host(AddrFdData *data)
{
	
	if(data == NULL){
		myperror("data is null...");
		return -1;
	}
	data->sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(data->sockfd <0){
		myperror("socket error...");
		return -1;
	}
	//绑定端口	
	if(bind(data->sockfd,(struct sockaddr*)&data->myAddr,data->len_addr)<0){
		myperror("bind failed...");
		return -1;
	}
	int retval;
	retval = connect(data->sockfd,(struct sockaddr*)&data->addr,data->len_addr);
	if(retval<0){
		myperror("socket error...");
		return -1;
	}
	return 0;
}
/*
*	功能：接收发送信息
*	参数2：数据包
*	返回值：成功返回0，失败返回-1
*/
int write_msg(AddrFdData *addrfd)
{

	if(addrfd == NULL){
		myperror("addrfd is null...");
		return -1;	
	}
	//清空上一次读取到内容
	memset(addrfd->buff_read,0,addrfd->len_read);
	int retval;

	//当对方的socket关闭时，第二次write会产生SIGPIPE信号，导致进程退出
	//不阻塞，关闭BrokePipe
	retval = send(addrfd->sockfd,addrfd->buff_write,addrfd->len_write,MSG_NOSIGNAL);
	if(retval <= 0){
		if(retval == EPIPE){
			myperror("send pipe error");
			return 1;
		}else{
			myperror("write failed...");	
			return 1;
		}
	}

	retval = recv(addrfd->sockfd,addrfd->buff_read,addrfd->len_read,0);
	if(retval< 0){
		myperror("read failed...");
		return 2;
	}
	//printf("%s,%d\n",addrfd->buff_read,retval);
	
	return 0;
}
/*
*	功能：打印错误信息
*	参数：str指名称，arg对应的错误信息
*	返回值：无
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


