 /*
 *程序用到的公共函数
 * author: tmaster 
 * time：2020-07-28
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>

#include "coap.h" 
#include "zlog.h"
#include "wgconfig.h"


extern zlog_category_t * zlogCat;

struct sCoAPNewRequest* creatPthread(void*(func)(void* ))
{
	struct sCoAPNewRequest* cnr=malloc(sizeof(struct sCoAPNewRequest));
	if(cnr==NULL)
	{
		zlog_error(zlogCat,"creatCoapBlockRequestPthread,malloc struct sCoAPNewRequest error");
		return NULL;
	}
	
  	memset(cnr,0,sizeof(struct sCoAPNewRequest));
  	
	if(pthread_mutex_init(&(cnr->askmtx),NULL)!=0)
	{
		zlog_error(zlogCat,"creatCoapBlockRequestPthread,pthread_mutex_init error");
		free(cnr);
		return NULL;
	}	
	if(pthread_cond_init(&(cnr->askcond),NULL)!=0)
	{
		zlog_error(zlogCat,"creatCoapBlockRequestPthread,pthread_cond_init error");
		free(cnr);
		return NULL;
	}

   //attr 分离线程
	pthread_attr_t attr;
	int s=pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if(s!=0)
		zlog_error(zlogCat,"creatCoapBlockRequestPthread,pthread detach attr creat error");
	
	
	if(pthread_create(&(cnr->pthread), NULL, func, NULL)!=0)
	{
		zlog_error(zlogCat,"creatCoapBlockRequestPthread,pthread_create error");
		free(cnr);
		return NULL;
	}

	// uint8_t* pt=malloc(CoAP_BLOCK_SIZE);
	// if(pt==NULL)
	// {
	// 	zlog_error(zlogCat,"creatCoapBlockRequestPthread,malloc CoAP_BLOCK_SIZE error");
	// 	free(cnr);
	// 	return NULL;
	// }
 //    memset(pt,0,CoAP_BLOCK_SIZE);
 //    cnr->msg=pt;
    
	return cnr;
}


void freeCreatCoapBlockRequestPthreadMalloc(struct sCoAPNewRequest* cnr)
{
 	free(cnr->msg);
    free(cnr);
}




