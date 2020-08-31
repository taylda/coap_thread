/*************************************************************************
*                        消息处理API
*	2017-02-13 初始创建
**************************************************************************/

#ifndef LIB_MBMESSAGE_H
#define LIB_MBMESSAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




//故障消息
typedef struct
{ 
    char 	 strDa[256]; //DA属性名称
    int   value; //故障值
    int  minutes;//相对于 2000-1-1 0:0:0 
    int   msec;  //0-59999 毫秒 
}FAULTINFO,*LPFAULTINFO;

//拓扑消息
typedef struct
{ 
    char 	 strDa[256]; //DA属性名称
    int   value; //拓扑信息值
}TOPOINFO,*LPTOPOINFO;

//注册信息
typedef struct  {
char devcieid[32]; //LTUID
char key[1024]; //密钥
int result; //0成功，1失败

char desc[64];
char version[64];

} REGISTERINFO, *LPREGISTERINFO;

//文件消息数据结构
typedef struct
{
	char iedName[512];//设备IED名称，多个以逗号隔开
	int all_flag;  //是否发给所有设备，为1给所有设备升级，为0给iedName内设备升级
int fileType;//文件类型
char strFileName[128];//文件名称
}UPDATEFILE, *LPUPDATEFILE;








//文件消息数据结构
typedef struct
{
char iedName[128];//设备IED名称
int fileType;//文件类型
char strFileName[128];//文件名称
}MSGFILE, *LPMSGFILE;


//SOE消息
typedef struct
{ 
	int    type;//单点:1，双点:2
    char 	 strDa[256]; //DA属性名称
    int   value; //单点时值:0分，1合 双点时值: 2 表示双分；3 表示双合；4 表示双分不确定；5 表示双合不确定
    int  minutes;//相对于 2000-1-1 0:0:0 
    int   msec;  //0-59999 毫秒 
}SOEINFO,*LPSOEINFO;


//保护信息数据结构
typedef struct{ 
char 	 strDa[256]; //DA属性名称
int  type;    //保护类型 
int  minutes;  //相对于 2000-1-1 0:0:0 
int  msec;    //0-59999 毫秒 
int  value;   //保护相关值 
}PROTECTINFO,*LPPROTECTINFO;



typedef struct 
{ 
char 	 strDa[256]; //DA属性名称
int	 type;	//1-选择分闸、2-选择合闸、3-执行分闸、4-执行合闸、5-直接执行分闸、6-直接执行合闸、7-取消
int duration;// 输出继电器闭合时间
int source;// 遥控执行来源
}SBOCTRL, *LPSBOCTRL;

typedef struct
{ 
 char 	      strDa[256]; //DA属性名称 
      int       function;       //操作类型 
      int        count;           //控制输出的重复次数，一般为 1 
      int        duration;       //输出脉冲宽度 
      int        timeOut;         //超时时间 
      int        source;         //遥控执行来源 
  } SBOCTRLRET,*LPSBOCTRLRET;

typedef struct 
{ 
char 	 strDa[256]; //DA属性名称
int	 bVal;	//数字输出值 1：合操作  0：分操作
int 	 source;// 遥控执行来源
}DIGITALOUT, *LPDIGITALOUT;

typedef struct
{ 
 char 	      strDa[256]; //DA属性名称 
int	 bVal;	//数字输出值 1：合操作  0：分操作
  } DIGITALOUTRET,*LPDIGITALOUTRET;

typedef struct 
{ 
   int  minutes; // 相对于 2000-1-1 0:0:0
   int   msec;   // 0-59999
   char 	 strDa[256]; 
int	 type;	//1-选择分闸、2-选择合闸、3-执行分闸、4-执行合闸、5-直接执行分闸、6-直接执行合闸、7-取消
   int     rtuNo;  // 哪一台 RTU 送来的
   int     source;  // YK 或 DO 源 区别是遥控来的还是自己产生的
   int    value;  // Bit15:0/1=分/合；Bit14:0/1 = SBO/DO; bit0-bit7: index
}SBODOSAVEINFO, *LPSBODOSAVEINFO;


typedef struct
{
	int   type;//消息类型
char   buf[1000]; //消息指针
int  dataLen;//消息长度
}GENERALMSG,*LPGENERALMSG;
  



//所有消息联合
typedef union
{
	MSGFILE            msgFile;//文件消息
	SOEINFO            soeInfo;//soe消息
	PROTECTINFO        protectInfo;//保护消息
	SBODOSAVEINFO      sbodoInfo;//sbo do消息
	GENERALMSG         generalMsg;//通用消息
	SBOCTRL			   sboCtrlInfo;//sbo控制输出
	SBOCTRLRET		   sboCtrlRetInfo;//sbo控制返校
	DIGITALOUT		   digitalOutInfo;//数字输出
	DIGITALOUTRET	   digitalOutRetInfo;//数字输出返校

	FAULTINFO    faultInfo;//故障消息
	TOPOINFO    topoInfo;//拓扑消息
	REGISTERINFO   registerInfo;//注册消息
	UPDATEFILE			updateFile;//升级文件消息

}UNIONMSG, *LPUNIONMSG;


//总消息类型
typedef struct{ 
int         msgType; //类型 1:文件 2:soe  3:保护 4:sbo do  5:通用，6：sboctrl 7：sboctrlret 8：digitalout 9：digitaloutret 10: 故障消息11: 拓扑消息12: 注册消息13: 升级文件消息
        UNIONMSG 	msgInfo; //消息
}MSGINFO,*LPMSGINFO;


 
 
 
 
enum MsgType 
{
	 MSGFILE_TYPE=1,
	 SOEINFO_TYPE,
	 PROTECTINFO_TYPE,
	 SBODOSAVEINFO_TYPE,
	 GENERALMSG_TYPE,
	 SBOCTRL_TYPE,
	 SBOCTRLRET_TYPE,
	 DIGITALOUT_TYPE,
	 DIGITALOUTRET_TYPE,
	 
	 FAULTINFO_TYPE,
	 TOPOINFO_TYPE,
	 REGISTERINFO_TYPE,
	 UPDATEFILE_TYPE
	 
}; 
 
 
 
 
 
/*************************************************************************
功能说明	消息发送初始化函数。需要发送消息前，首先调用本函数进行初始化，然后随时调用相应的消息发送函数进行消息发送。
输入	无
输出	无
返回值	0:执行成功; 非0:错误码

**************************************************************************/
int kh_publisherMsg();

/*************************************************************************
*回调函数功能说明	通过消息分发机制，处理接收到的消息
*输入	lpMsgInfo:消息数据结构
*输出	无
*返回值	无
**************************************************************************/
//void (*fptr)(LPMSGINFO lpMsgInfo);


/*************************************************************************	
功能说明	订阅消息函数。需要接收消息的进程初始化本函数，存在消息时自动调用参数中提供的函数指针。正常情况下函数不返回，始终循环接受处理消息。
输入	(*fptr)(LPMSGINFO lpMsgInfo)消息处理函数指针

输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_subscriberMsg_sync(void funptr(LPMSGINFO lpMsgInfo));


/*************************************************************************
功能说明	订阅消息函数。需要接收消息的进程初始化本函数，存在消息时自动调用参数中提供的函数指针。函数立即返回，会创建专有线程，在线程内循环接受处理消息。
输入	(*fptr)(LPMSGINFO lpMsgInfo)消息处理函数指针

输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_subscriberMsg_async(void funptr(LPMSGINFO lpMsgInfo));


/*************************************************************************	
功能说明	根据LPMSGINFO结构发送消息
输入	lpMsgInfo:消息数据结构
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendMsg(const LPMSGINFO lpMsgInfo);

/*************************************************************************	
功能说明	文件发送消息函数，收到消息后根据文件名称读取文件
输入	msgFile:文件数据结构
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendFileMsg (const LPMSGFILE  msgFile);

/*************************************************************************	
功能说明	单点soe发送函数
输入	soeInfo:soe数据结构
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendSoeMsg (const LPSOEINFO  soeInfo);

/*************************************************************************	
功能说明	双点soe消息发送函数
输入	soeInfo:soe数据结构
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendDoubSoeMsg (const LPSOEINFO  soeInfo);

/*************************************************************************	
功能说明	保护消息发送函数
输入	proMsg:保护数据结构
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendProMsg (const LPPROTECTINFO  proMsg);

/*************************************************************************	
功能说明	SBO/DO消息发送函数
输入	sbodoInfo:sbo do消息
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendSBODOSaveMsg (const LPSBODOSAVEINFO  sbodoInfo);

/*************************************************************************
功能说明	通用消息发送函数
输入	generalMsg:通用数据消息
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendGeneralMsg (const LPGENERALMSG  generalMsg);

/*************************************************************************
功能说明	SBO控制消息发送函数
输入	sboCtrlMsg: SBO控制消息
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendSBOCtrlMsg (const LPSBOCTRL sboCtrlMsg);


/*************************************************************************
功能说明	SBO控制返校消息发送函数
输入	sboCtrlRetMsg: SBO控制返校消息
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendSBOCtrlRetMsg (const LPSBOCTRLRET sboCtrlRetMsg);

/*************************************************************************
功能说明	数字输出消息发送函数
输入	digitalOutMsg: 数字输出消息
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendDigitalOutMsg (const LPDIGITALOUT digitalOutMsg);

/*************************************************************************
功能说明	数字输出返校消息发送函数
输入	digitalOutRetMsg: 数字输出返校消息
输出	无
返回值	0:执行成功; 非0:错误码
**************************************************************************/
int kh_sendDigitalOutRetMsg (const LPDIGITALOUTRET digitalOutRetMsg);


int kh_sendFaultMsg (const LPFAULTINFO faultInfo);
int kh_sendTopoMsg (const LPTOPOINFO topoInfo);
int kh_sendRegisterMsg (const LPREGISTERINFO registerInfo);
int kh_sendUpdateFileMsg (const LPUPDATEFILE updateMsgFile);

#endif
