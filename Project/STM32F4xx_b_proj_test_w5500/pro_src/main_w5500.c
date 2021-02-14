/***************************************************************************************
 * ������  ��W5500ģ�飭�ͻ���ģʽ����
 * ����    ��W5500�Ķ˿�0�����ڿͻ���ģʽ,�����롶TCP&UDP���Թ��ߡ��ϴ����ķ��������,
 *			 ������500ms��ʱ������ʱ������˷����ַ���"\r\nWelcome To ChuangWeiElec!\r\n",ͬʱ����
 *			 �յ�����˷��������ݻط�������ˡ�
 * ʵ��ƽ̨��STM32������ + W5500��̫��(TCP/IP)ģ��
 * Ӳ�����ӣ�  PC5 -> W5500_RST       
 *             PA4 -> W5500_SCS      
 *             PA5 -> W5500_SCK    
 *             PA6 -> W5500_MISO    
 *             PA7 -> W5500_MOSI    
 * ��汾  ��ST_v3.5
***************************************************************************************/

/*�����������*/
//���أ�192.168.1.1
//����:	255.255.255.0
//������ַ��0C 29 AB 7C 00 01
//����IP��ַ:192.168.1.199
//�˿�0�Ķ˿ںţ�5000
//�˿�0��Ŀ��IP��ַ��192.168.1.190
//�˿�0��Ŀ�Ķ˿ںţ�6000

// #include "stm32f10x.h"		
#include "W5500_taobao.h"			
#include <string.h>


unsigned int W5500_Send_Delay_Counter=0; //W5500������ʱ��������(ms)

/*******************************************************************************
* ������  : W5500_Initialization
* ����    : W5500��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//��ʼ��W5500�Ĵ�������
	Detect_Gateway();	//������ط����� 
	Socket_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
}

/*******************************************************************************
* ������  : Load_Net_Parameters
* ����    : װ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢������ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
void Load_Net_Parameters(void)
{
	Gateway_IP[0] = 192;//�������ز���
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 0;
	Gateway_IP[3] = 1;

	Sub_Mask[0]=255;//������������
	Sub_Mask[1]=255;
	Sub_Mask[2]=255;
	Sub_Mask[3]=0;

	Phy_Addr[0]=0x0c;//����������ַ
	Phy_Addr[1]=0x29;
	Phy_Addr[2]=0xab;
	Phy_Addr[3]=0x7c;
	Phy_Addr[4]=0x00;
	Phy_Addr[5]=0x01;

	IP_Addr[0]=192;//���ر���IP��ַ
	IP_Addr[1]=168;
	IP_Addr[2]=0;
	IP_Addr[3]=100;

	S0_Port[0] = 0x13;//���ض˿�0�Ķ˿ں�5000 
	S0_Port[1] = 0x88;

	S0_DIP[0]=192;//���ض˿�0��Ŀ��IP��ַ
	S0_DIP[1]=168;
	S0_DIP[2]=0;
	S0_DIP[3]=112;

	S0_DPort[0] = (6800 >> 8); //���ض˿�0��Ŀ�Ķ˿ں�6000
	S0_DPort[1] = (6800 % 256);
	// S0_DPort[0] = 0x17;//���ض˿�0��Ŀ�Ķ˿ں�6000
	// S0_DPort[1] = 0x70;

	S0_Mode=TCP_CLIENT;//���ض˿�0�Ĺ���ģʽ,TCP�ͻ���ģʽ
}

/*******************************************************************************
* ������  : W5500_Socket_Set
* ����    : W5500�˿ڳ�ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �ֱ�����4���˿�,���ݶ˿ڹ���ģʽ,���˿�����TCP��������TCP�ͻ��˻�UDPģʽ.
*			�Ӷ˿�״̬�ֽ�Socket_State�����ж϶˿ڵĹ������
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//�˿�0��ʼ������
	{
		if(S0_Mode==TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
		}
	}
}

/*******************************************************************************
* ������  : Process_Socket_Data
* ����    : W5500���ղ����ͽ��յ�������
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : �������ȵ���S_rx_process()��W5500�Ķ˿ڽ������ݻ�������ȡ����,
*			Ȼ�󽫶�ȡ�����ݴ�Rx_Buffer������Temp_Buffer���������д�����
*			������ϣ������ݴ�Temp_Buffer������Tx_Buffer������������S_tx_process()
*			�������ݡ�
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
	unsigned short size;
	size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
	memcpy(Tx_Buffer, Rx_Buffer, size);			
	Write_SOCK_Data_Buffer(s, Tx_Buffer, size);
}

/*******************************************************************************
* ������  : main
* ����    : ���������û������main������ʼ����
* ����    : ��
* ���    : ��
* ����ֵ  : int:����ֵΪһ��16λ������
* ˵��    : ��
*******************************************************************************/
int main_loop(void)
{
		sNET_Init();
	Load_Net_Parameters();		//װ���������	
	// W5500_Hardware_Reset();		//Ӳ����λW5500
	W5500_Initialization();		//W5500��ʼ������
	unsigned int   i = 0;
	for (  i = 0; i < sizeof(Tx_Buffer); i++)
	{
		Tx_Buffer[i] = i+1;
	}
	i = 0;
	// memcpy(Tx_Buffer, "Welcome To ChuangWeiElec!", 27);
	while (1)
	{
		W5500_Socket_Set();//W5500�˿ڳ�ʼ������

		W5500_Interrupt_Process();//W5500�жϴ���������

		if((S0_Data & S_RECEIVE) == S_RECEIVE)//���Socket0���յ�����
		{
			S0_Data&=~S_RECEIVE;
			Process_Socket_Data(0);//W5500���ղ����ͽ��յ�������
		}
		
		// if(W5500_Send_Delay_Counter >= 1)//��ʱ�����ַ���
		{

			if(S0_State == (S_INIT|S_CONN))
			{
				i++;
				if (i < 10000)
				{
					
					S0_Data&=~S_TRANSMITOK;
					// memcpy(Tx_Buffer, "\r\nWelcome To ChuangWeiElec!\r\n", 27);
					Write_SOCK_Data_Buffer(0, Tx_Buffer, sizeof(Tx_Buffer));//ָ��Socket(0~7)�������ݴ���,�˿�0����27�ֽ�����
					//printf("send message \n");
				}
			
			}
			// W5500_Send_Delay_Counter=0;
		}
	}
}
