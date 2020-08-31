/*
 * list.h
 *
 *  author: tmaster daifx zhaonj
 *  date:   20200707
 *
 */

#ifndef _LTUDevELLIST_H_
#define _LTUDevELLIST_H_


#include <stdint.h>
#include "config.h"


//向链表中添加LTU元素信息
//return: NULL，添加不成功
//        非零，添加成功的链表元素指针
// LTUDevEList* addLTUdevtoList();
//从链表中删除元素
// void deleteLTUdevfromList(LTUDevEList* el);
//根据mac地址查询链表
// LTUDevEList* findLTUdevfromListbyid(uint8_t* id);
struct sCoAPNewRequest* addPthreadRequesttoList(struct sCoAPNewRequest* element);
void deletePthreadRequestfromList(struct sCoAPNewRequest* el);
struct sCoAPNewRequest* findPthreadRequestfromListbyid(struct sCoAPNewRequest* el);

#endif 



