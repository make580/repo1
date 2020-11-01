/*
*	用于standby主机,判活
*/
#ifndef _STAN_STATE_H_
#define _STAN_STATE_H_
#include<stdio.h>
#include"standby.h"
/*
*	功能：数据协议包
*	给对方发送数据包验证其是否在线
*/
typedef struct{
	//消息
	char buff[100];
	//消息验证标志
	int state;
	//判断其是否在线
	int result;//1为在线，0为不在线
}MessageDatas;
/*
*	用于内存释放
*/
extern MessageDatas *msgdatasta;
/*
*	功能：校验 验证码是否正确
*	参数1：验证码
*	放回值：正确返回1，错误返回0
*/
extern int msg_check(char *checknum,int (*funcState)(MessageDatas*),MessageDatas *msg);
/*
*	功能：接收读取到的消息验证返回状态	
*
*/
extern int sourceState(MessageDatas *data);
/*
*	功能:standby总接口
*/
extern int mainStandby(int arg,char **avg);
/*
*	功能：线程函数信息接收
*/
extern void *th_receivePthread(void *arg);
/*
*	功能：进程结束清空free内存
*/
extern void sig_handler(int signo);
/*
*	功能：清空内存
*	返回值：无
*/
extern void myclose();
#endif
