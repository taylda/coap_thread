/*
配置文件
author: tmaster
date:2020-07-28
*/

#ifndef _WGCONFIG_H
#define _WGCONFIG_H


//#include <pthread.h>
//#include <stdint.h>
#include <arpa/inet.h>
#include <mqueue.h>
#include "mbmessage.h"
#include "mbrealapi.h"
#include "coap.h" 
#include "zlog.h"


extern zlog_category_t * zlogCat;
extern mqd_t CoAPRxMq;

/**********define***************/

#define	COAP_LISTERN_IP			"127.0.0.2"
#define	CoAPRX_MQmaxmsg			(16)
#define	CoAPRX_MQmsgsize			(520)
#define	UDP_NET_RX_TIMEOUT		(200)       //单位ms
#define	LTUNUM_MAX					(500)
#define	CoAP_BLOCK_SIZE			(1024)  //
#define	OUTTIME					(1000)



typedef enum
{
	LTU_YC,
	LTU_YK,
	LTU_RX_BLOCK,
	LTU_TX_BLOCK,
	LTU_NOACK

} LTURequest;

struct sPthreadPara
{
	pthread_mutex_t  askmtx;   //线程用来判断消息是否有应答
	pthread_cond_t   askcond;
	pthread_t pthread;
	uint8_t* msg;
	uint32_t megleg;	
	time_t timing;
	bool timeout;
	
	
};

struct sCoAPNewRequest
{
	uint8_t addr[15];
	uint8_t respath[256];
	LTURequest  req;
	uint8_t  msgtype;
	time_t time;
	
	struct sPthreadPara* pthpara;
	struct sCoAPNewRequest* next;
};

struct sDeviceInfo
{
	uint16_t num;  //数据库中的LTU个数
	time_t time;  //发送模拟量请求的时间
	uint16_t count;// 接受到的LTU模拟量计数
	struct LTUInfor* pLTUInfo;
};

#endif /* UTLIST_H */

