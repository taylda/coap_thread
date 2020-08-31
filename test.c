 /*
 *测试程序
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
#include "function.h"


extern zlog_category_t * zlogCat;
extern mqd_t CoAPRxMq;
extern str the_token;


bool TestPthread=false;



void*
testpthread(void* parameter)
{
            // struct sCoAPNewRequest tmp;
            // memcpy(tmp.respath,"3",1);
            // tmp.req=LTU_TX_BLOCK;
            
            // tmp.pthpara=malloc(sizeof(struct sPthreadPara));
            // tmp.pthpara->timing=30;
            // mq_send(CoAPRxMq,(char *)&tmp,sizeof(tmp),0);
    while(1)
    {
        char str;
        str=getchar();
        if(str=='l')
        {
            struct sCoAPNewRequest tmp;
            memcpy(tmp.respath,"3",1);
            tmp.req=LTU_TX_BLOCK;
            
            tmp.pthpara=malloc(sizeof(struct sPthreadPara));
            tmp.pthpara->timing=30;
            mq_send(CoAPRxMq,(char *)&tmp,sizeof(tmp),0);

        }
        else if(str=='a')
        {
            struct sCoAPNewRequest tmp;
            memcpy(tmp.addr,"FC0000AFBCBD",12);
            tmp.req=LTU_YC;
            tmp.pthpara=malloc(sizeof(struct sPthreadPara));
            tmp.pthpara->timing=3;

            // tmp.pthpara->askmtx=PTHREAD_MUTEX_INITIALIZER;
            pthread_mutex_init(&tmp.pthpara->askmtx,NULL);
            pthread_cond_init(&tmp.pthpara->askcond,NULL);
            mq_send(CoAPRxMq,(char *)&tmp,sizeof(tmp),0);
            while(1)
            {
                pthread_cond_wait(&tmp.pthpara->askcond,&tmp.pthpara->askmtx);
                printf("here\n");
            }

        }
        else if(str=='e')
        {
            // while(1)
            // {
            // creatTheToken();
            
            // printf("token =%lx  %lx\n", *(the_token.s+1),*the_token.s);
            // usleep(5000);
            // }

        }
        else if(str=='\n')
        {
        }
        else
            printf("error:input character error\n");
        usleep(1000);


        
    }
    
    return 0;
}

//创建CoAP新的请求

