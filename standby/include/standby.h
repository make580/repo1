/*
*	通讯信息接收发送
*/
#ifndef _STANDBY_H_
#define _STANDBY_H_
#include <netdb.h>
#include <pthread.h>
//设置验证码,长度不超过16
#define CHECKDATA "ticket1234"
/*
*	功能：把ip地址和描述符数据打包
*/
typedef struct addr{
	//本地的描述符
	int sockfd;
	//server的ip地址存放
	struct sockaddr_in addr;
	//本地端口存放
	struct sockaddr_in myAddr;
	int len_read;
	int len_write;
	int len_addr;
	char buff_write[100];//数据自定义
	char buff_read[100];//数据存放	
}AddrFdData;
	//用于进程内存释放
extern AddrFdData *addrdatasta;
/*
*	功能：设置ip地址和端口号(主机2)
*	参数（IPaddr）:IP地址
*	参数（port1）；端口号
*	参数（port2）：本地端口号
*	返回值：ip地址
*/
extern void *setIPaddr_port(const char *IPaddr,const int port ,const int myport); 
/*
*	功能：负责通讯(搭建client）
*	参数：ip地址包
*	放回值：成功返回描述符，失败小于0
*/
extern int client_host(AddrFdData *addr);

/*
*	功能：发送消息并返回接收到的消息
*	参数2：数据包
*	返回值：成功返回0，失败返回-1
*/
extern int write_msg(AddrFdData *addr);
/*
*	功能：打印错误信息
*	返回值：无
*/
extern void myprintf(const char *str);
/*
*	功能：错误信息打印和提示	
*/
extern void myperror(const char *str);
/*
*	功能：进程结束清空内存
*/
extern void sig_handler(int signal);

#endif
