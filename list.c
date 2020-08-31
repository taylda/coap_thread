/*
 *list.c 
 *链表
 *
 *author:tmaster daifx zhaonj
 *  
 *date；20200707 
 *
 *  
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

//#include <types.h>
#include "utlist.h"

#include "coap.h" 
#include "wgconfig.h"
#include "zlog.h"


struct sCoAPNewRequest* PthreadRequestList=NULL;
pthread_mutex_t PthreadRequestListmtx=PTHREAD_MUTEX_INITIALIZER;


//向链表中添加LTU元素信息
//return: NULL，添加不成功
//        非零，添加成功的链表元素指针
struct sCoAPNewRequest* addPthreadRequesttoList(struct sCoAPNewRequest* element)
{
	struct sCoAPNewRequest* el=malloc(sizeof(struct sCoAPNewRequest));
	if(el==NULL)
	{
		zlog_error(zlogCat,"addPthreadRequesttoList,malloc error");
		return el;
	}
	memset(el,0,sizeof(struct sCoAPNewRequest));
	//获取struct LTUInfor信息
	memcpy(el,element,sizeof(struct sCoAPNewRequest));
	pthread_mutex_lock(&PthreadRequestListmtx);
	LL_APPEND(PthreadRequestList,el);
	pthread_mutex_unlock(&PthreadRequestListmtx);
	return el;	
}

//从链表中删除元素
void deletePthreadRequestfromList(struct sCoAPNewRequest* el)
{
	pthread_mutex_lock(&PthreadRequestListmtx);
	LL_DELETE(PthreadRequestList,el);
	pthread_mutex_unlock(&PthreadRequestListmtx);

	free(el);
}


static int idCmp(struct sCoAPNewRequest* out,struct sCoAPNewRequest* el )
{
	return (strcmp(out->addr,el->addr)||strcmp(out->respath,el->respath));
}

//根据mac地址查询链表
struct sCoAPNewRequest* findPthreadRequestfromListbyid(struct sCoAPNewRequest* el)
{
	struct sCoAPNewRequest* out;

	pthread_mutex_lock(&PthreadRequestListmtx);
	LL_SEARCH(PthreadRequestList,out,el,idCmp);
	if(out==NULL)
	{
	   zlog_notice(zlogCat,"findPthreadRequestfromListbyid,no elements to find");
	}
	pthread_mutex_unlock(&PthreadRequestListmtx);
	return out;
}





