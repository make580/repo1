/*
*	用于standby主机,判活
*/
#include "stan_state.h"
#include "standby.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

MessageDatas *msgdatasta = NULL;
/*
*	功能：(判活)校验 验证码是否正确,回调函数sourceState
*	参数1：验证码,参数2：函数sourceState
*	返回值：正确返回1，错误返回0
*/
int msg_check(char *checknum,int (*funcState)(),MessageDatas *msg)
{
	if(strncmp(checknum,CHECKDATA,10)==0){
		myprintf(checknum);
		myprintf(">>>onlive success...");
		msg->result = 1;
		//对方在线
		funcState(msg);
		return 1;
	}else{
		//对方不在线
		myprintf(checknum);
		myprintf(">>>onlive failed...");
		msg->result = 2;
		funcState(msg);
	}
	
	return 0;
}
/*
*	功能：状态接收(逻辑重启)
*	参数1：接收到的验证状态
*	state:1代表验证成功(是活着)，2代表验证失败(没活着)
*/
int sourceState(MessageDatas *data)
{
	if(data->state != data->result){//验证对方在线
		data->state = data->result;
		if(data->state == 2){//2对方没再线，启动
			myprintf(">>>state changed success...2");
			
		}else{//1对方上线，启动
			myprintf(">>>state changed success...1");
			
		}
	}else{
		printf(">>>state changed failed...%d\n",data->state);
	}
	
	return 0;

}
/*
*	功能：进程结束清空free内存
*/
void sig_handler(int signo)
{
	if(signo ==SIGINT||signo==SIGTERM){
		myprintf("End of the process...");
		//释放内存
		myprintf("free ok...");
		myclose();		
		exit(1);
	}	
}

/*
*	功能:standby总接口
*/
int mainStandby(int argc,char **avg)
{

	//没有ip地址时退出进程
	if(argc<4){
		myprintf("addr or port use error...");
		exit(1);
	}
	//接受进程关闭信号
	if(signal(SIGINT,sig_handler)==SIG_ERR){
		myperror("signal SIGINT error...");
		return 1;
	}
	if(signal(SIGTERM,sig_handler)==SIG_ERR){
		myperror("signal SIGTERM error...");
		return 1;
	}
	//配置ip地址和端口号、信息数据打包
	//avg[2]是server的端口，avg[3]是本地的端口
	addrdatasta = setIPaddr_port(avg[1],atoi(avg[2]),atoi(avg[3]));
	if(addrdatasta == NULL){
		myperror("addr create failed...");
		exit(1);
	}
	//状态数据存放
	msgdatasta = (MessageDatas*)malloc(sizeof(MessageDatas));
	memset(msgdatasta,0,sizeof(MessageDatas));
	//第一次链接失败直接退出
	if(client_host(addrdatasta)!=0){
			myperror("client create failed...");
			close(addrdatasta->sockfd);
			return 1;
	}
	while(1){
	//信息传送
		if(write_msg(addrdatasta)!=0){//信息传送失败
			myperror("write_msg failed...");
			close(addrdatasta->sockfd);
			while(1){
				if(client_host(addrdatasta)!=0){//链接失败
					myperror("client create failed...");
					close(addrdatasta->sockfd);
					//判活
					msg_check(addrdatasta->buff_read,sourceState,msgdatasta);
					sleep(5);
				}else{
					break;
				}
			}
		}else{//信息传送成功
		//判活
			msg_check(addrdatasta->buff_read,sourceState,msgdatasta);
		}
		sleep(5);
	}
	
	myclose();
	return 0;
}
/*
*	功能：清空内存
*	返回值：无
*/
void myclose()
{
	if(addrdatasta!=NULL){
		close(addrdatasta->sockfd);
		free(addrdatasta);
	}
	if(msgdatasta!=NULL)free(msgdatasta);
}