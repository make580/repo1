/*
*	作用于active
*/
#ifndef _ACTIVE_H_
#define _ACTIVE_H_
#include <netdb.h>
//设置验证码,长度不超过16
#define CHECKDATA "ticket1234"
/*
*	功能:设置服务器的ip地址和端口号打包
*/
typedef struct addr{
	int sockfd;//存放描述符
	struct sockaddr_in addr;//存放地址
	socklen_t len;//地址长度
}AddrData;
//定义个静态全局变量，用于进程结束释放内存
extern AddrData *addrdatasta;
/*
*	功能：把ip地址和端口号打包，并返回；
*	参数：IPaddr是个地址，port是个端口号
*	返回值：错误返回NULL
*/
extern void *setIPaddrPort(const char *IPaddr,const int Port);
/*
*	功能：搭建通讯server
*	参数：一个AddrData
*	返回值：成功返回一个socket，错误小于0
*/
extern int serverhost();
/*
*	功能：读写操作
*/
extern void readywrite(int fd,fd_set *rset,fd_set *allset);
/*
*	功能：接收消息并返回消息
*	返回值：成功大于0，失败0；
*/
extern int replyg_msg(char *buff);
/*
*	功能：打印信息
*/
extern void myprintf(const char *str);
/*
*	功能：错误信息打印和提示	
*/
extern void myperror(const char *str);
/*
*	功能：清内存
*/
extern void myclose();
/*
*	功能：active总接口
*/
extern int mainactive(int argc,char** argv);
/*
*	功能：时间打印
*	返回值：一个malloc的char类型（用完注意内存释放）
*/
char *fun_time(void);
/*
*	功能：进程结束清空内存
*/
extern void sig_handler(int signal);
#endif
