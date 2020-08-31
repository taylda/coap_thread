/*
 * CoAPpthread.c
 *description:CoAP线程
 *  Created on: 2020-7-28
 *      Author: tmaster  taylor
 */
 
 
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>  //sleep
#include <stdlib.h>  //malloc
#include <string.h>   //memcpy
#include <stdint.h>   //uint8_t
#include <time.h>

#include "coap.h" 
#include "wgconfig.h"
#include "zlog.h"
#include "list.h"


extern zlog_category_t * zlogCat;
extern mqd_t CoAPRxMq;


static unsigned char _token_data[8];
static str the_token = { 0, _token_data };
static str payload = { 0, NULL};
str host={12,"fc0000abbcbd"};
str path={14,"LD0/LLN0/dsAin"};

int order_opts(void *a, void *b);
coap_list_t * new_option_node(unsigned short key, unsigned int length, unsigned char *data);
coap_pdu_t * coap_new_request(coap_context_t *ctx,uint8_t  msgtype,uint8_t method, str * payload,coap_list_t *options );





void creatTheToken()
{
  static uint64_t token=0;
  uint64_t re,me;
  
  memset(_token_data,0,8);
  token++;
  me=token;
  uint8_t i=0;
  while(me)
  {
    _token_data[i]=me%256;
    i++;
    me=me>>8;
  }
  the_token.length=i;

}



static struct timespec delay_us_at_TimesSpec(uint32_t us)
{
	struct timespec time;
	__syscall_slong_t ns;

	ns = us*1000;

	clock_gettime(CLOCK_REALTIME, &time);

	__syscall_slong_t tmpns = time.tv_nsec + ns;
	if(tmpns>999999999)
	{
		time.tv_sec  += tmpns/999999999;
		time.tv_nsec  = tmpns%999999999;
	}
	else
		time.tv_nsec = tmpns;

	return time;
}


static inline coap_opt_t *
get_block(coap_pdu_t *pdu, coap_opt_iterator_t *opt_iter) {
  coap_opt_filter_t f;
  
  assert(pdu);

  memset(f, 0, sizeof(coap_opt_filter_t));
  //这里选择2个块中的其中一个，后期可根据需要取舍
  coap_option_setb(f, COAP_OPTION_BLOCK1);
  coap_option_setb(f, COAP_OPTION_BLOCK2);

  coap_option_iterator_init(pdu, opt_iter, f);
  return coap_option_next(opt_iter);
}

void message_handler(struct coap_context_t  *ctx, 
		const coap_address_t *remote, 
		coap_pdu_t *sent,
		coap_pdu_t *received,
		const coap_tid_t id)
{

	coap_pdu_t *pdu = NULL;
	coap_opt_t *block_opt;
	coap_opt_iterator_t opt_iter;
	unsigned char buf[4];
	coap_list_t *option;
	size_t len;
	unsigned char *databuf;
	coap_tid_t tid;
	coap_opt_t * coapopt;
	struct sCoAPNewRequest cnr;
	struct sCoAPNewRequest* cnrList;

	// zlog_debug(zlogCat,"I recv messsage");
	//打印接受到的pdu

    zlog_debug(zlogCat,"** process incoming %d.%02d response:",
	  (received->hdr->code >> 5), received->hdr->code & 0x1F);
    coap_show_pdu(received);
 
	//判断接受到消息类型
	switch (received->hdr->type) {
	case COAP_MESSAGE_CON:
	  /* acknowledge received response if confirmable (TODO: check Token) */
	  coap_send_ack(ctx, remote, received);
	  break;
	case COAP_MESSAGE_RST:
	  info("got RST\n");
	  return;
	default:
	  ;
	}

    // option检查
	coapopt=coap_check_option(received, COAP_OPTION_URI_HOST, &opt_iter);
	if(coapopt!=NULL)
	{
		memset(cnr.addr,0,15);
		memcpy(cnr.addr,COAP_OPT_VALUE(coapopt),COAP_OPT_LENGTH(coapopt));
		zlog_debug(zlogCat,"recv coap request,COAP_OPTION_URI_HOST=%s",cnr.addr);
		
	}
	else  //没有COAP_OPTION_URI_HOST选项，丢掉该帧数据
		return;
	
	coapopt=coap_check_option(received, COAP_OPTION_URI_PATH, &opt_iter);
   	if(coapopt!=NULL)
	{
		memset(cnr.respath,0,256);
		memcpy(cnr.respath,COAP_OPT_VALUE(coapopt),COAP_OPT_LENGTH(coapopt));
		zlog_debug(zlogCat,"recv coap request,COAP_OPTION_URI_PATH=%s",cnr.respath);
	}
	else     //没有COAP_OPTION_URI_PATH，丢掉该帧数据
		return;
		     // 查找CoAP请求链表中是否有这个请求
	cnrList=findPthreadRequestfromListbyid(&cnr);
	if(cnrList==NULL)
		return;

		
	//根据应答码进行相应的处理
	/* output the received data, if any */
	if (received->hdr->code == COAP_RESPONSE_CODE(205)) 
	{
	
	  /* set obs timer if we have successfully subscribed a resource */
//	  if (sent && coap_check_option(received, COAP_OPTION_SUBSCRIPTION, &opt_iter)) {
//		debug("observation relationship established, set timeout to %d\n", obs_seconds);
//		set_timeout(&obs_wait, obs_seconds);
//	  }
	  /* Got some data, check if block option is set. Behavior is undefined if
		* both, Block1 and Block2 are present. */

		

			block_opt = get_block(received, &opt_iter);
			if (!block_opt) {
			/* There is no block option set, just read the data and we are done. */
				if (coap_get_data(received, &len, &databuf))
				{
					memcpy(cnrList->pthpara->msg,databuf,len);
					cnrList->pthpara->megleg=len;
				}
				cnrList->pthpara->timeout=false;
				pthread_cond_signal(&(cnrList->pthpara->askcond));
				deletePthreadRequestfromList(cnrList);
			} 
			else //块应答
			{
				if(cnrList->req==LTU_TX_BLOCK)//块发送请求
				{
					unsigned short blktype = opt_iter.type;
					//得到数据放进buf，这里是否需要判断请求的块num正确性？
		      		if (coap_get_data(received, &len, &databuf))
		      		{
						//注意指针的变化
						memcpy(cnrList->pthpara->msg+cnrList->pthpara->megleg,databuf,len);
						cnrList->pthpara->megleg += len;
						zlog_debug(zlogCat,"get data ");
					}

					if (COAP_OPT_BLOCK_MORE(block_opt))
					{
			
						/* more bit is set */
						zlog_debug(zlogCat,"found the M bit, block size is %u, block nr. %u",COAP_OPT_BLOCK_SZX(block_opt), coap_opt_block_num(block_opt));

						
						//创建一个optionlist
	           			coap_list_t *optlist = NULL;
						coap_insert( &optlist,new_option_node(COAP_OPTION_URI_HOST,strlen(cnrList->addr),cnrList->addr),order_opts);
						coap_insert( &optlist,new_option_node(COAP_OPTION_URI_PATH,strlen(cnrList->respath),cnrList->respath),order_opts);
						//块选项
						coap_insert( &optlist,new_option_node(blktype, coap_encode_var_bytes(buf, ((coap_opt_block_num(block_opt) + 1) << 4) | COAP_OPT_BLOCK_SZX(block_opt)), buf),order_opts);
						/* create pdu with request for next block */
						pdu=coap_new_request(ctx,COAP_MESSAGE_CON,COAP_REQUEST_GET, &payload,optlist);
						coap_show_pdu(pdu);
						if(pdu)
						{
						  tid = coap_send_confirmed(ctx, remote, pdu);
						  zlog_debug(zlogCat,"send more block");
						  if (tid == COAP_INVALID_TID) 
						  {
						  	zlog_debug(zlogCat,"LTU_RX_BLOCK,error sending new request");
					        coap_delete_pdu(pdu);
						  } 
						}
						coap_delete_list(optlist);

						
					}
					else//没有后续的块传输，回传响应的请求
					{
						cnrList->pthpara->timeout=false;
						pthread_cond_signal(&(cnrList->pthpara->askcond));
						zlog_debug(zlogCat,"block done");
						deletePthreadRequestfromList(cnrList);

					}

				}
				if(cnrList->req==LTU_RX_BLOCK)
				{

				}

			}//end block

	}//end 205
	else 
	{			/* no 2.05 */
	    /* check if an error was signaled and output payload if so */
	    if (COAP_RESPONSE_CLASS(received->hdr->code) >= 4) {
	      zlog_debug(zlogCat,"recv coap request,code=%d.%02d error",(received->hdr->code >> 5),(received->hdr->code & 0x1F));
    
  		}
  	}

}

	




int readCoAPFrame(coap_context_t  *ctx)
{
	ssize_t readsize;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET( ctx->sockfd, &readfds);
	struct timeval tv;
   
	tv.tv_sec = UDP_NET_RX_TIMEOUT / 1000;
	tv.tv_usec = (UDP_NET_RX_TIMEOUT % 1000) * 1000;
	
	int ret = select(ctx->sockfd + 1, &readfds, NULL, NULL, &tv);
	if (ret == -1) {
		return -1;
	}
	else if (ret == 0)
		return -1;
	else {
		coap_read( ctx );
		return 0;
   }
}


extern struct sCoAPNewRequest* PthreadRequestList;
 void*  CoapPthread(void* parameter)
 {
	 
	coap_context_t  *ctx = NULL;
	coap_pdu_t  *pdu;
	coap_address_t dst;
	coap_tid_t tid = COAP_INVALID_TID;
	dst.size = 16;
	
	struct sockaddr_in tmpip;
    bzero(&tmpip, sizeof(tmpip));
	tmpip.sin_family = AF_INET;
	tmpip.sin_addr.s_addr = inet_addr("0.0.0.0");
	// tmpip.sin_addr.s_addr = inet_addr(COAP_LISTERN_IP);
	tmpip.sin_port = htons(5683);
	 memcpy(&dst.addr.sa,&tmpip,16);
	ctx=coap_new_context(&dst);
	if(ctx==NULL)
	{
		zlog_error(zlogCat,"coap_new_context failed");
		exit(-1);
	}
	coap_register_response_handler(ctx, message_handler);
	// coap_set_log_level(10);
	
	uint8_t CoAPBuf[COAP_MAX_PDU_SIZE];
	memset(CoAPBuf,0,sizeof(CoAPBuf));
	coap_tick_t now;
	coap_queue_t *nextpdu;
	struct sCoAPNewRequest tmp;

	tmpip.sin_addr.s_addr= inet_addr("127.0.0.11");
	// tmpip.sin_addr.s_addr= inet_addr("134.102.218.18");             //coap.me
	memcpy(&dst.addr.sa,&tmpip,16);

	int ret;
	while(1)
	{		
		unsigned int prior=0;
	    struct timespec tmspec = delay_us_at_TimesSpec(20000);
		ssize_t mqret=mq_timedreceive(CoAPRxMq,CoAPBuf,sizeof(CoAPBuf),&prior,&tmspec); 
		time_t time_now; 

		if(mqret>0)
		{
			if(tmp.req!=LTU_NOACK)
			{
			
			memcpy(&tmp,CoAPBuf,mqret);
			tmp.time=time(NULL);
			addPthreadRequesttoList(&tmp);

			}
			if(tmp.req==LTU_YC)
			{
				host.s=malloc(sizeof(tmp.addr));    //12
				memcpy(host.s,tmp.addr,sizeof(tmp.addr));	
				coap_list_t *optlist = NULL;
			 	coap_insert( &optlist, 	   new_option_node(COAP_OPTION_URI_HOST,host.length,host.s),		   order_opts); 
				coap_insert( &optlist, 	   new_option_node(COAP_OPTION_URI_PATH,path.length,path.s),	   		order_opts); 
				//no payload
				pdu=coap_new_request(ctx,COAP_MESSAGE_CON,COAP_REQUEST_GET, &payload,optlist);   //payload =NULL

				if (pdu->hdr->type == COAP_MESSAGE_CON)
				{
				    tid = coap_send_confirmed(ctx, &dst, pdu);
				    // coap_delete_pdu(pdu);
				    zlog_debug(zlogCat,"sendconfirm");
				}
				else 
				{
				    tid = coap_send(ctx, &dst, pdu);
				    zlog_debug(zlogCat,"send");
				}
				if (pdu->hdr->type != COAP_MESSAGE_CON || tid == COAP_INVALID_TID)
				{
					coap_delete_list(optlist);
			    	coap_delete_pdu(pdu);
			    	zlog_debug(zlogCat,"delete");
				}

			}
			if(tmp.req==LTU_TX_BLOCK)
			{
				path.length=strlen(tmp.respath);
				path.s=malloc(path.length);         //  not free
				strcpy(path.s,tmp.respath);	
				// host.length=0;                        //test

				coap_list_t *optlist = NULL;
			 	// coap_insert( &optlist, 	   new_option_node(COAP_OPTION_URI_HOST,host.length,host.s),		   order_opts); 
				coap_insert( &optlist, 	   new_option_node(COAP_OPTION_URI_PATH,path.length,path.s),	   		order_opts); 
				//no payload
				uint8_t tmp[]={0x12};    //1,0,128
				coap_insert(&optlist, new_option_node(COAP_OPTION_BLOCK2,1,tmp),   order_opts);
				
				pdu=coap_new_request(ctx,COAP_MESSAGE_CON,COAP_REQUEST_GET, &payload,optlist);   //payload =NULL

				// zlog_debug(zlogCat,"new req");
				if(!pdu)
					zlog_error(zlogCat,"creat pdu");				

				tid = coap_send_confirmed(ctx, &dst, pdu);

				coap_delete_list(optlist);   //dont know  right
			}
		}
		else 
		{
			nextpdu = coap_peek_next( ctx );
			// zlog_debug(zlogCat,"recvtime out");
			coap_ticks(&now);
			while (nextpdu && nextpdu->t <= now - ctx->sendqueue_basetime) {
			  coap_retransmit( ctx, coap_pop_next( ctx ));
			  // zlog_debug(zlogCat,"coap_retransmit");
			  nextpdu = coap_peek_next( ctx );
			}
		}

		ret=readCoAPFrame(ctx);
		if(ret==0)	    
	    	coap_dispatch( ctx );

	    time_now=time( NULL);
	    struct sCoAPNewRequest *pt;
	    LL_FOREACH(PthreadRequestList,pt)
	    {
	    	// zlog_debug(zlogCat,"time now=%d  pt time= %d",time_now,pt->time);
	    	if(time_now-pt->time>=pt->pthpara->timing)            //timing empt   kongde!
	    	{
	    		pt->pthpara->timeout=true;
	    		pthread_cond_signal(&pt->pthpara->askcond);
	    		deletePthreadRequestfromList(pt);

	    		zlog_debug(zlogCat,"pdu time out");
	    	}

	    }
	    // zlog_debug(zlogCat,"loop done");
		usleep(2000);
	}
	
 }


int
order_opts(void *a, void *b) {
  if (!a || !b)
    return a < b ? -1 : 1;

  if (COAP_OPTION_KEY(*(coap_option *)a) < COAP_OPTION_KEY(*(coap_option *)b))
    return -1;

  return COAP_OPTION_KEY(*(coap_option *)a) == COAP_OPTION_KEY(*(coap_option *)b);
}


coap_list_t *
new_option_node(unsigned short key, unsigned int length, unsigned char *data) {
  coap_option *option;
  coap_list_t *node;

  option = coap_malloc(sizeof(coap_option) + length);
  if ( !option )
    goto error;

  COAP_OPTION_KEY(*option) = key;
  COAP_OPTION_LENGTH(*option) = length;
  memcpy(COAP_OPTION_DATA(*option), data, length);

  /* we can pass NULL here as delete function since option is released automatically  */
  node = coap_new_listnode(option, NULL);

  if ( node )
    return node;

 error:
  perror("new_option_node: malloc");
  coap_free( option );
  return NULL;
}


coap_pdu_t *
coap_new_request(coap_context_t *ctx,uint8_t  msgtype,uint8_t method, str * payload,coap_list_t *options ) {
  coap_pdu_t *pdu;
  coap_list_t *opt;

  if ( ! ( pdu = coap_new_pdu() ) )
  {
  	zlog_error(zlogCat,"new_pdu");
  	return NULL;
  }
    

  pdu->hdr->type = msgtype;
  pdu->hdr->id = coap_new_message_id(ctx);
  pdu->hdr->code = method;
  creatTheToken();
  pdu->hdr->token_length = the_token.length;
  if ( !coap_add_token(pdu, the_token.length, the_token.s)) {
	zlog_debug(zlogCat,"cannot add token to request");

  }
  for (opt = options; opt; opt = opt->next) {
    coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *)opt->data),
		    COAP_OPTION_LENGTH(*(coap_option *)opt->data),
		    COAP_OPTION_DATA(*(coap_option *)opt->data));
  }
// zlog_debug(zlogCat,"here");
  if (payload->length) {
  //  if ((flags & FLAGS_BLOCK) == 0)
      coap_add_data(pdu, payload->length, payload->s);
  //  else
  //    coap_add_block(pdu, payload.length, payload.s, block.num, block.szx);
  }

  coap_show_pdu(pdu);  

  return pdu;
}
