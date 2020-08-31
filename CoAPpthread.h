/*
 * CoAPpthread.h
 *description:CoAP线程头文件
 *  Created on: 2020-7-28
 *      Author: tmaster  
 */
 
 #ifndef _COAP_PTHREAD_H
#define  _COAP_PTHREAD_H

#include "wgconfig.h"



// coap_pdu_t* creatnewquest(coap_context_t *ctx, str * host,str *path,str * payload,uint8_t msgtype,LTURequest req);
// int readCoAPFrame(coap_context_t  *ctx);

void*  CoapPthread(void* parameter);




#endif
 
 
 
 
 