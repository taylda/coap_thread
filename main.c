/*
 * main.c
 *description:
 *  Created on: 2020-7-28
 *      Author: tmaster  
 */

#include <stdio.h>
#include <unistd.h>  //sleep
#include <stdlib.h>  //malloc
#include <string.h>   //memcpy
#include <stdint.h>   //uint8_t
#include <stdbool.h>  //false

#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "zlog.h"
#include "CoAPpthread.h"
#include "wgconfig.h"
#include "test.h"
#include "YCpthread.h"

#include "function.h"
//外部变量
zlog_category_t * zlogCat;
mqd_t CoAPRxMq;

struct sDeviceInfo DeviceInfo;


extern bool TestPthread;


int main(void)
{
	int ret;
	int rc = zlog_init("zlog.conf");
	if (rc) {
		printf("error:zlog init failed\n");
		exit(-1);
	}

	zlogCat = zlog_get_category("my_cat");
	if (!zlogCat) {
		printf("error:zlog zlog_get_category fail\n");
		zlog_fini();
		exit(-1);
	}
	
    //创建消息队列，通信进程
     struct mq_attr attr;
     attr.mq_maxmsg=CoAPRX_MQmaxmsg ;
     attr.mq_msgsize=CoAPRX_MQmsgsize;
     CoAPRxMq = mq_open("/CoAPRecvMq",O_CREAT|O_RDWR|O_NONBLOCK,0777,&attr);
     if(CoAPRxMq==-1)
     {
		 zlog_error(zlogCat,"creat /CoAPRecvMq failed");
		 exit(-1);

     }
     ret = kh_openRealDB();
     ret = kh_publisherMsg();

	//CoAP线程	
	pthread_t CoAPpthreadt;
	pthread_create(&CoAPpthreadt, NULL, CoapPthread, NULL);
	//创建测试线程
	pthread_t testpthreadt;
	pthread_create(&testpthreadt, NULL, testpthread, NULL);
	//初始化日志系统
	pthread_t YCpthreadt;
	pthread_create(&YCpthreadt, NULL, YCPthread, NULL);

	
	
	while(1)
	{
		if(TestPthread==true)
		{
    //         uint8_t* pt=malloc(CoAP_BLOCK_SIZE);
    //         if(pt==NULL)
    //         	zlog_error(zlogCat,"malloc error");
		  //   struct sCoAPNewRequest* cnr=malloc(sizeof(struct sCoAPNewRequest));
		  //   if(cnr==NULL)
		  //   	zlog_error(zlogCat,"malloc error");
    //         pthread_mutexattr_t attr;
    //         if(pthread_mutex_init(&(cnr->askmtx),NULL)!=0)
    //         {
				// zlog_error(zlogCat,"pthread_mutex_init error");

    //         }
            
		    	
			TestPthread=false;
			

		}
		sleep(100);
	}
	return 0;
}



