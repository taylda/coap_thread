/*
 * YCpthread.c
 *description:请求模拟量
 *  Created on: 2020-7-28
 *      Author: tmaster  
 */ 
 
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>  //sleep
#include <stdlib.h>  //malloc
#include <string.h>   //memcpy
#include <stdint.h>   //uint8_t
#include <stdbool.h>

#include "coap.h" 
#include "wgconfig.h"
#include "zlog.h"
#include "mbmessage.h"
#include "mbrealapi.h"

#define YC_REQUEST_INTERVAL_TIME    (320)    //单位s

extern zlog_category_t * zlogCat;
extern mqd_t CoAPRxMq;
extern struct sDeviceInfo DeviceInfo;
pthread_mutex_t YCRequestmtx=PTHREAD_MUTEX_INITIALIZER;
bool YCRequest=false;

const unsigned char *DAName[]={	"Bay01_MC/MMXU1$MX$A$phsA$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$A$phsB$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$A$phsC$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$A$res$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$PhV$phsA$cVal$mag$f",	
	"Bay01_MC/MMXU1$MX$PhV$phsB$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$PhV$phsC$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$W$phsA$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$W$phsB$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$W$phsC$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$VAr$phsA$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$VAr$phsB$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$VAr$phsC$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$VA$phsA$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$VA$phsB$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$VA$phsC$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$PF$phsA$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$PF$phsB$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$PF$phsC$cVal$mag$f",
	"Bay01_MC/MMXU1$MX$Hz$mag$f",
	"Bay01_MC/GGIO1$ST$Ind2$stVal"
};




//获得数据中心中所有的注册信息
//返回 ：struct sDeviceInfo 结构体  指针?
bool GetRegisteredDeviceInfo(struct sDeviceInfo* dinfo)
{
	
	int ret;
	int num;
	//struct sDeviceInfo dinfo;
	
	ret=kh_getAllLTUInforsNum(&num);
	if(ret!=0)
	{
		zlog_warn(zlogCat,"GetRegisteredDeviceInfo: kh_getAllLTUInforsNum failed");
		dinfo->num=0;
		return false;
	}
	dinfo->num=num;
	dinfo->pLTUInfo=calloc(num,sizeof(struct LTUInfor));
	if(dinfo->pLTUInfo==NULL)
	{
		zlog_warn(zlogCat,"GetRegisteredDeviceInfo: calloc for AllLTUInfors failed");
		dinfo->num=0;
		return false;
	}
        
	ret=kh_getAllLTUInfors(dinfo->pLTUInfo,&num);
    if(ret!=0)
	{
		zlog_warn(zlogCat,"GetRegisteredDeviceInfo: kh_getAllLTUInfors failed");
		dinfo->num=0;
		return false;
	}
	//清楚所有的state位
    for(int i=0;i<num;i++)
    	(dinfo->pLTUInfo+i)->state=0;
	return  true;
}

//请求模拟量线程
void*  YCPthread(void* parameter)
{
	DIGITALOUT digMsg;
    memset(&digMsg,0,sizeof(DIGITALOUT));

	while(1)
	{
       //获取到数据库LTU信息
		while(GetRegisteredDeviceInfo(&DeviceInfo)){
		//请求遥测量，同时发送信息给CoAP线程
        // zlog_warn(zlogCat,"here");         
		pthread_mutex_lock(&YCRequestmtx);
		YCRequest=true;
		pthread_mutex_unlock(&YCRequestmtx);
		struct sCoAPNewRequest tmp;
		tmp.req=LTU_YC;
		tmp.msgtype=COAP_MESSAGE_CON;
		tmp.pthpara=malloc(sizeof(struct sPthreadPara));
        tmp.pthpara->timing=300;
		for(int i=0;i<DeviceInfo.num;i++)
		{

            strcpy(tmp.addr,(DeviceInfo.pLTUInfo+i)->name+3);
            if((DeviceInfo.pLTUInfo+i)->failedcount>=9)
            	tmp.msgtype=COAP_MESSAGE_NON;  
            // zlog_warn(zlogCat,"here");  

			mq_send(CoAPRxMq,&tmp,sizeof(tmp),0);
			usleep(20000);
		}
		//进入休眠状态，等待5分钟结束
		// sleep(YC_REQUEST_INTERVAL_TIME);
		sleep(20);
		pthread_mutex_lock(&YCRequestmtx);
		YCRequest=false;
		pthread_mutex_unlock(&YCRequestmtx);
		//查看state位，查看LTU在离线状态
		for(int i=0;i<DeviceInfo.num;i++)
		{

            if((DeviceInfo.pLTUInfo+i)->state==0)
            {
            	(DeviceInfo.pLTUInfo+i)->failedcount++;
            	if((DeviceInfo.pLTUInfo+i)->failedcount==9)
            	{
            		digMsg.bVal=0;
					strcpy(digMsg.strDa,(DeviceInfo.pLTUInfo+i)->name);	 
					kh_sendDigitalOutMsg(&digMsg);
																								//failconut save to database;
					zlog_warn(zlogCat,"%s state change:online -> offline",(DeviceInfo.pLTUInfo+i)->name );
            	}
            	else if((DeviceInfo.pLTUInfo+i)->failedcount==10)
            		(DeviceInfo.pLTUInfo+i)->failedcount=9;
            	else
            	{
            		zlog_warn(zlogCat,"%s time out",(DeviceInfo.pLTUInfo+i)->name );
            	}
            	
            }
            else
            {
            	if((DeviceInfo.pLTUInfo+i)->failedcount==9)
            	{
            		(DeviceInfo.pLTUInfo+i)->failedcount=0;
            		digMsg.bVal=1;
					strcpy(digMsg.strDa,(DeviceInfo.pLTUInfo+i)->name);	 
					kh_sendDigitalOutMsg(&digMsg);
																								//failconut save to database;
					zlog_warn(zlogCat,"%s state change:offline -> online",(DeviceInfo.pLTUInfo+i)->name );
            	}

            } 		
		}
		free(DeviceInfo.pLTUInfo);
		}
		usleep(20000);

	}
	
}
