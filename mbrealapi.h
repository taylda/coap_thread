/*************************************************************************
*                        实时库操作API
*2017-02-13 初始创建
**************************************************************************/


#ifndef LIB_MBREALAPI_H
#define LIB_MBREALAPI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




struct  LTUAccess  { 
	char date_time[256];
	char deviceid[256];	
	int accessmodel;

	char desc[64];
	char key[256];
	char version[64];	

};




//单元信息结构
struct  LTUInfor  { 
	int id;
	char MBsoftware_version[8];// 软件版本
	char MBhardware_version[8];// 硬件版本
	char SGsoftware_version[8];// 软件版本
	char SGhardware_version[8];// 硬件版本
	char name[33];// 单元IED名称
	char addr [33];// 单元地址
	int failedcount;
	int state;// 单元状态
	char desc[256];// 单元描述
};	




//拓扑信息结构
struct  TopoInfor  { 
	int id;
	char type[16];// 设备类型
	char m_rln[64];// 设备LN实例
	char name [64];// 开关描述
	char l_link [64];// 左侧连接点
	char r_link [64];// 右侧连接点
	int power_flag;// 是否出线开关
	char iedName[32];// 所属单元IED名
	char substation[64];// 所属台区或开闭所名称
};





//数据集结构
struct  DataSet
{ 
	char iedName[33];//IED名称	
	char name[33];//数据集名称
	char ldInst [33];//所属LD名称
	char desc[256];//数据集描述
};

//数据集条目结构
struct  DataSetSub
{ 
	char fcdaName[256];// 数据集条目完整名称
	char iedName[33];//IED名称	
	char dsName[33];//数据集名称
	char ldInst[33];//LD名称
	char lnClass[5];//LN类型
	char lnInst[8];//LN名称
	char doName[33];//DO名称
	char daName[33];//DA名称
	char fc[3];//功能约束
};

//数据属性结构
struct DA
{
	char daName[256];// 数据属性完整名称
	char iedName[33];//IED名称		
	char ldName[33]; //LD名称
	char lnName[12]; //LN名称
	char doName[33]; //DO名称
	char fcName[3]; //功能约束
	char value[256];// 属性值
	char valueType[16];//值的数据类型
	char doDesc[256];//DO描述
};



enum DA_CONF_TYPE
{
	MO_NI_LIANG=1,
	DAN_DIAN_SHU_ZI_LIANG=2,
	SHUANG_DIAN_SHU_ZI_LIANG=3,
	SBO_YAO_KONG_LIANG=4,
	DIAN_DU_LEI_JIA_LIANG=5,
	SHU_ZI_SHU_CHU_LIANG=6,
	OTHER_TYPE=0
};



#define  DA_VALUE_AI_DATASET_NAME    "dsAin"
#define  DA_VALUE_DI_DATASET_NAME    "dsDin"
#define  DA_VALUE_VDI_DATASET_NAME    "dsGgio"
#define  DA_VALUE_CI_DATASET_NAME    "dsEnergy"

#define  REAL_DB_NAME "/dev/shm/realtime.db"

unsigned int RSHash(const char* str, unsigned int len);  //api内部使用函数






int kh_openRealDB();

int kh_closeRealDB();


int kh_addIEDRealDB(char *cid_file);

int kh_saveDoubDAValue(const char *daName, double val);

int kh_saveDoubDAValue1 (const char *daName, double val, int character);

int kh_saveIntDAValueValue (const char *daName, int val);

int kh_saveIntDAValueValue1 (const char *daName, int val, int character);

int kh_getIntDAValue(const char *daName, int *pValue);

int kh_getIntDAValue1(const char *daName, int *pValue, int *pCharacter);

int kh_getDoubDAValue(const char *daName, double *pValue);

int kh_getDoubDAValue1(const char *daName, double *pValue,int *pCharacter);

int kh_saveDAValuesByDataSet (const char *iedName,const char *dataSetName, const double *pDaVals,int count);

int kh_saveDAValues1ByDataSet (const char *iedName,const char *dataSetName, const double *pDaVals, const int *pCharacter, int count);

int kh_getDAValuesByDataSet (const char *iedName,const char *dataSetName,double *pDaVals,int *pCount );

int kh_getDAValues1ByDataSet (const char *iedName,const char *dataSetName,double *pDaVals,int *pCharacter,int *pCount );


int kh_addLTUInfor(const struct LTUInfor *pLTUInfor);

int kh_addLTUInfors(const struct LTUInfor *pLTUInfors,int count);

int kh_deleteAllLTUInfors ();

int kh_deleteLTUInforByName(const char *name);

int kh_getAllLTUInforsNum(int *pCount);

int kh_getAllLTUInfors(struct LTUInfor *pLTUInfors,int *pCount);

int kh_getLTUInforByName(const char *name, struct LTUInfor *pLTUInfor);

int kh_setLTUInforByName(const char *name, const struct LTUInfor *pLTUInfor);

int kh_setLTUInforStateByName(const char * name,int state);

int kh_getLTUInforStateByName(const char * name,int *pState);

int kh_getLTUInforById(int id, struct LTUInfor *pLTUInfor);


int kh_addTopoInfor (const struct TopoInfor *pTopoInfor);

int kh_addTopoInfors(const struct TopoInfor *pTopoInfors,int count);

int kh_deleteAllTopoInfors();

int kh_deleteTopoInforByLNName(const char * name);

int kh_getAllTopoInforsNum(int *pCount);

int kh_getAllTopoInfors(struct TopoInfor *pTopoInfors,int *pCount);

int kh_getTopoInforByLNName(const char *name,  struct TopoInfor *pTopoInfor);

int kh_getTopoInforPower(struct TopoInfor *pTopoInfor);

int kh_getLeftTopoInforsNumByLNName(const char *name ,int *pCount);

int kh_getLeftTopoInforsByLNName(const char *name,  struct TopoInfor *pTopoInfors ,int *pCount);

int kh_getRightTopoInforsNumByLNName(const char *name ,int *pCount);

int kh_getRightTopoInforsByLNName(const char *name,  struct TopoInfor *pTopoInfors ,int *pCount);


int kh_getAllDataSetsNum(int *pCount);

int kh_getAllDataSets(struct DataSet *pSets,int *pCount);

int kh_getDataSetByName(const char * iedName,const char *name,  struct DataSet *pSet);


int kh_getDataSetSubByName(const char *fcdaName, struct DataSetSub *pSub);

int kh_getDataSetSubsByParamsNum(const char * iedName,const char *dsName,const char *ldInst,const char *lnClass,const char *lnInst,const char *doName,const char *daName,const char *fc, int *pCount );

int kh_getDataSetSubsByParams(const char * iedName,const char *dsName,const char *ldInst,const char *lnClass,const char *lnInst,const char *doName,const char *daName,const char *fc,struct DataSetSub *pSubs, int *pCount );


int kh_getDAByName(const char *daName,struct DA *pDa);

int kh_getDAsByParamsNum(const char * iedName,const char* ldName, const char* lnName, const char* doName, const char * fcName , int *pCount);

int kh_getDAsByParams(const char * iedName,const char* ldName, const char* lnName, const char* doName, const char * fcName ,struct DA *pDas, int *pCount);


int kh_addLTUAccess(const struct LTUAccess *pLTUAccess);

int kh_deleteLTUAccessByDeviceid(const char *deviceid);

int kh_getAllLTUAccessNum(int *pCount);

int kh_getAllLTUAccess(struct LTUAccess *pLTUAccess,int *pCount);



#endif
