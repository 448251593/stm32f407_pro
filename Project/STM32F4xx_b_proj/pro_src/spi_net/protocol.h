/*所有跟协议有关的结构体在此文件中定义*/

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

typedef struct  /*0x60查询物理地址命令包*/
{
unsigned char  CMD60_Respond;
unsigned short header;
unsigned short SNID;
unsigned short DNID;
unsigned short CMD;
unsigned short DLEN;
unsigned char  EMAC[6];
unsigned short CRC_TAIL;  //5A42 "ZB"
}CMD60;

typedef struct  /*0x68心跳包*/
{
unsigned short header;
unsigned short SNID;
unsigned short DNID;
unsigned short CMD;
unsigned short DLEN;
unsigned char  pro_sta;
unsigned char  exv_bat;
unsigned short CRC_TAIL;  //5A42 "ZB"
}CMD68;

typedef struct  /*无数据返回响应命令包*/
{
unsigned short header;
unsigned short SNID;
unsigned short DNID;
unsigned short CMD;
unsigned short DLEN;
unsigned short CRC_TAIL;  //5A42 "ZB"
}Respond;


//typedef struct    //设备信息结构体
//{
//	
//	
//}DEV


#endif
   



