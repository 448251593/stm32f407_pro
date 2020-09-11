//add by bcg,2020-09-11 15:49:12

#ifndef  _IS62_65WVS5128FALL_BLL_SPI_H_
#define  _IS62_65WVS5128FALL_BLL_SPI_H_


#define     IS62_65WVS5128_MAX_PAGE_NUM             16384  // max page num
#define     IS62_65WVS5128_PAGE_SIZE                32//bytes

#define     IS62_65WVS5128_DIE0_START_ADDR          0x0
#define     IS62_65WVS5128_DIE0_END_ADDR            0x3ffff
#define     IS62_65WVS5128_DIE1_START_ADDR          0x4ffff
#define     IS62_65WVS5128_DIE1_END_ADDR            0x7ffff

#define     IS62_65WVS5128_CMD_READ           0x03 //add by bcg,2020-09-11 15:50:53 Read data to memory array beginning at selected address
#define     IS62_65WVS5128_CMD_WRITE          0x02 //add by bcg,2020-09-11 15:50:44 Write data to memory array beginning at selected address
#define     IS62_65WVS5128_CMD_ESDI           0x3b //add by bcg,2020-09-11 15:50:26 Enter SDI mode
#define     IS62_65WVS5128_CMD_ESQI           0x38 //add by bcg,2020-09-11 15:50:03 Enter SQI mode

#define     IS62_65WVS5128_CMD_RSTDQI         0xff //add by bcg,2020-09-11 15:49:55 Reset SDI/SQI mode
#define     IS62_65WVS5128_CMD_RDMR           0x05 //add by bcg,2020-09-11 15:49:23 Read Mode Register
#define     IS62_65WVS5128_CMD_WRMR           0x01 //add by bcg,2020-09-11 15:49:42 Write Mode Register


// #define    IS62_65WVS5128_OPERAT_MODE_BYTE  0x00 //Byte mode
// #define    IS62_65WVS5128_OPERAT_MODE_PAGE  0x01 //Page mode
// #define    IS62_65WVS5128_OPERAT_MODE_SEQU  0x02  //Sequential mode (default operation)
// #define    IS62_65WVS5128_OPERAT_MODE_RESE  0x03  //Reserved
typedef enum
{
   IS62_65WVS5128_OPERAT_MODE_BYTE = 0x00, //Byte mode
   IS62_65WVS5128_OPERAT_MODE_PAGE = 0x01, //Page mode
   IS62_65WVS5128_OPERAT_MODE_SEQU = 0x02,  //Sequential mode (default operation)
   IS62_65WVS5128_OPERAT_MODE_RESE = 0x03,  //Reserved
}IS62_OPERATE_MODE_ENUM;


#endif










