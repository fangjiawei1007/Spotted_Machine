#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "def.h"
#include "2416addr.h"
#include "2416lib.h"
#include "option.h"
#include "LCD.h"
#include "mmu.h"
#include "Nand.h"
#include "..\inc\download.h"
#include "..\inc\systemconf.h"
#include "..\inc\systemmenu.h"
#include "..\inc\function.h"
#include "..\inc\jiema.h"
#include "..\inc\communication.h"
#include "..\inc\test.h"
#include "..\inc\Rtc.h"
#include "..\inc\encrypt.h"

#include "..\inc\appBase.h"
#include "..\inc\app.h"

#include "includes.h"

extern INTERALMEMORY g_InteralMemory;


int InPulseCount[7]={0};		//��¼6·���������������

int OutPulse_PreSet[4]={0};	// 3·���������  ����Ԥ��ֵ
volatile unsigned int OutPulse_Counter[4]={0};	// 3·���������  ��ǰ���������

unsigned int frequence_Y1 = 0;
int pulse_PreSet_Y1 = 0;

unsigned int frequence_Y2 = 0;
int pulse_PreSet_Y2 = 0;

unsigned int frequence_Y3 = 0;
int pulse_PreSet_Y3 = 0;

/* //������ʦ���ƹܻ�
int k_motor,motor_fac;
unsigned char signal1,signal2;
volatile unsigned char backward_status;
volatile int	pulse_get_num;
volatile int	pulse_send_num;
volatile int pulse_send_num_record;//,pulse_knife_record;
unsigned char sebiaojiadingchang_flag=1;
volatile unsigned char error_times;
unsigned char sebiao_status;
volatile int pulse_get_total_num;
volatile unsigned char knife_round_finish_flag; */
///////////////////////////////////////////////////////////////////
////	������������X1 �жϷ������    ///////////////
////	InPulseCount[1] ΪX1  �����������	//////////////////
///////////////////////////////////////////////////////////////////
void __irq PulseIn_1_Process(void)	   //���봫����
{
	int i;
	for(i=0; i<20; i++);

	if((rGPFDAT >> 1) & 0x1)
	{
		//�˴���ʼ����жϳ���
		InPulseCount[1]++;	
	}

	rEINTPEND=(1<<1);
	ClearPending(BIT_EINT1);
}
///////////////////////////////////////////////////////////////////
////	������������X2 �жϷ������    ///////////////
////	InPulseCount[2] ΪX2  �����������	//////////////////
///////////////////////////////////////////////////////////////////
void __irq PulseIn_2_Process(void)	
{
	int i;
	for(i=0; i<20; i++);

	if((rGPFDAT >> 2) & 0x1)
	{
		InPulseCount[2]++;
	}

	rEINTPEND=(1<<2);
	ClearPending(BIT_EINT2);
}
///////////////////////////////////////////////////////////////////
////	������������X3 �жϷ������    ///////////////
////	InPulseCount[3] ΪX3  �����������	//////////////////
///////////////////////////////////////////////////////////////////
void __irq PulseIn_3_Process(void)	
{
	int i;
	for(i=0; i<20; i++);

	if((rGPFDAT >> 3) & 0x1)
	{
		InPulseCount[3]++;
	}

	rEINTPEND=(1<<3);
	ClearPending(BIT_EINT3);
}


//////////////////////////////////////////////////////////////////////
////	������������X4-X7  �жϷ������     //////////////
////	InPulseCount[4] ΪX4  �����������	//////////////////
////	InPulseCount[5] ΪX5  �����������	//////////////////
////	InPulseCount[6] ΪX6  �����������	//////////////////
////	InPulseCount[7] ΪX7  �����������	//////////////////
/////////////////////////////////////////////////////////////////////
void __irq PulseIn_4_7_Process(void)
{   
	//	int i;
	//	for(i=0; i<20; i++);
	
	if (rEINTPEND & (1<<4))	// Eint4	// ��������X4
	{
	 
		if((rGPFDAT >> 4) & 0x1)
		{
			InPulseCount[4]++;
		}
     
	   
		rEINTPEND=(1<<4);		
	}
	else if (rEINTPEND & (1<<5))	// Eint6	// ��������X5
	{ 
		if((rGPFDAT >> 5) & 0x1)
		{
			InPulseCount[5]++;
		}
 
		rEINTPEND=(1<<5);		
	}
	else if (rEINTPEND & (1<<6))	// Eint6	// ��������X6
	{
		 
		if((rGPFDAT >> 6) & 0x1)
		{
			InPulseCount[6]++;
		}
		 
		rEINTPEND=(1<<6);		
	}
 
	ClearPending(BIT_EINT4_7);
}


//////////////////////////////////////////////////////////////////////
////	������������X8-X23  �жϷ������     //////////////

////////////////////    ��϶������ x8    /////////////////////
/////////////////////////////////////////////////////////////////////
void __irq PulseIn_8_23_Process(void)
{ 
	 
	 // Eint10	 ����X10----��λ������ ��������������
	if ( !( rEINTMASK >>10  & 0x1 )  && (rEINTPEND & (1<<10)) )
	{ 
		//�˴���ʼ���X_10�жϵ��жϺ�����������ע��by����ΰ��ԭ��Ϊ������ʦ���ƹܻ�����
		rEINTPEND=(1<<10);		
	}
	else if ( !( rEINTMASK >>11  & 0x1 )  && (rEINTPEND & (1<<11)) )	// Eint11	// ��������X11----- ��ɫ�촫����  �½��ش���   
	{ 
		//�˴���ʼ���X_11�жϵ��жϺ�����������ע��by����ΰ
			
		
		rEINTPEND=(1<<11);		
	}
	
	
	/*
	else if (rEINTPEND & (1<<13))	// Eint13	// ��������X13-------ɫ�괫����
	{
				 
		rEINTPEND=(1<<13);		
	}
  
   */
	ClearPending(BIT_EINT8_23);

}



///////////////////////////////////////////////////////////////
////	������������X1  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse1_Start(void)
{
	rEINTPEND=(1<<1);
	ClearPending(BIT_EINT1);
	
	rEINTMASK &= ~BIT_EINT1;
	rINTMSK1 &= ~BIT_EINT1;
}
///////////////////////////////////////////////////////////////
////	������������X1  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse1_Stop(void)
{
	rEINTMASK |=(1<<1);
	rINTMSK1 |=BIT_EINT1;
	
	rEINTPEND=(1<<1);
	ClearPending(BIT_EINT1);
}

///////////////////////////////////////////////////////////////
////	������������X2  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse2_Start(void)
{
	rEINTPEND=(1<<2);
	ClearPending(BIT_EINT2);
	
	rEINTMASK &= ~(1<<2);
	rINTMSK1 &= ~BIT_EINT2;
	
}
///////////////////////////////////////////////////////////////
////	������������X2  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse2_Stop(void)
{
	rEINTMASK |=(1<<2);
	rINTMSK1 |=BIT_EINT2;
	
	rEINTPEND=(1<<2);
	ClearPending(BIT_EINT2);
}

///////////////////////////////////////////////////////////////
////	������������X3  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse3_Start(void)
{
	rEINTPEND=(1<<3);
	ClearPending(BIT_EINT3);

	rEINTMASK &= ~(1<<3);
	rINTMSK1 &= ~BIT_EINT3;
}
///////////////////////////////////////////////////////////////
////	������������X3  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse3_Stop(void)
{
	rEINTMASK |=(1<<3);
	rINTMSK1 |=BIT_EINT3;
	
	rEINTPEND=(1<<3);
	ClearPending(BIT_EINT3);
}

///////////////////////////////////////////////////////////////
////	������������X4  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse4_Start(void)
{
	rEINTPEND=(1<<4);
	ClearPending(BIT_EINT4_7);
	
	rEINTMASK &= ~(1<<4);
	rINTMSK1 &= ~BIT_EINT4_7;
	
}
///////////////////////////////////////////////////////////////
////	������������X4  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse4_Stop(void)
{
	rEINTMASK |=(1<<4);
	//rINTMSK1 |=BIT_EINT4_7;
	
	rEINTPEND=(1<<4);
	ClearPending(BIT_EINT4_7);
}
///////////////////////////////////////////////////////////////
////	������������X5  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse5_Start(void)
{
	rEINTPEND=(1<<5);
	ClearPending(BIT_EINT4_7);
	
	rEINTMASK &= ~(1<<5);
	rINTMSK1 &= ~BIT_EINT4_7;
}
///////////////////////////////////////////////////////////////
////	������������X5  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse5_Stop(void)
{
	rEINTMASK |=(1<<5);
	//rINTMSK1 |=BIT_EINT4_7;
	
	
	rEINTPEND=(1<<5);
	ClearPending(BIT_EINT4_7);
}
///////////////////////////////////////////////////////////////
////	������������X6  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse6_Start(void)
{
	rEINTPEND=(1<<6);
	ClearPending(BIT_EINT4_7);
	
	rEINTMASK &= ~(1<<6);
	rINTMSK1 &= ~BIT_EINT4_7;
}
///////////////////////////////////////////////////////////////
////	������������X6  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse6_Stop(void)
{
	rEINTMASK |=(1<<6);
	//rINTMSK1 |=BIT_EINT4_7;
	
	rEINTPEND=(1<<6);
	ClearPending(BIT_EINT4_7);
}

///////////////////////////////////////////////////////////////
////	������������X10  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse10_Start(void)
{
	rEINTPEND=(1<<10);
	ClearPending(BIT_EINT8_23);
	
	rEINTMASK &= ~(1<<10);
	rINTMSK1 &= ~BIT_EINT8_23;
}
///////////////////////////////////////////////////////////////
////	������������X10  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse10_Stop(void)
{
	rEINTMASK |=(1<<10);
	//rINTMSK1 |=BIT_EINT8_23;
	
	rEINTPEND=(1<<10);	
	ClearPending(BIT_EINT8_23);			
}

///////////////////////////////////////////////////////////////
////	������������X11  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse11_Start(void)
{
	rEINTPEND=(1<<11);
	ClearPending(BIT_EINT8_23);
	
	rEINTMASK &= ~(1<<11);
	//rINTMSK1 &= ~BIT_EINT8_23;
//	sebiao_status=1;	������ʦ���ƹܻ�
		
}
///////////////////////////////////////////////////////////////
////	������������X11  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse11_Stop(void)
{
	rEINTMASK |=(1<<11);
	//rINTMSK1 |=BIT_EINT8_23;
	
	rEINTPEND=(1<<11);	
	ClearPending(BIT_EINT8_23);	
//	sebiao_status=0; ������ʦ���ƹܻ�
}

///////////////////////////////////////////////////////////////
////	������������X12  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse12_Start(void)
{
	rEINTPEND=(1<<12);
	ClearPending(BIT_EINT8_23);		

	rEINTMASK &= ~(1<<12);
	rINTMSK1 &= ~BIT_EINT8_23;
}
///////////////////////////////////////////////////////////////
////	������������X12  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse12_Stop(void)
{
	rEINTMASK |=(1<<12);
	//rINTMSK1 |=BIT_EINT8_23;
	
	rEINTPEND=(1<<12);
	ClearPending(BIT_EINT8_23);		
	 
}

///////////////////////////////////////////////////////////////
////	������������X13  ����//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse13_Start(void)
{	
	rEINTPEND=(1<<13);
	ClearPending(BIT_EINT8_23);		
	
	rEINTMASK &= ~(1<<13);
	rINTMSK1 &= ~BIT_EINT8_23;
}
///////////////////////////////////////////////////////////////
////	������������X13  ֹͣ//////////////////////////
///////////////////////////////////////////////////////////////
void InPulse13_Stop(void)
{
	rEINTMASK |=(1<<13);
	//rINTMSK1 |=BIT_EINT8_23;
	
	rEINTPEND=(1<<13);
	ClearPending(BIT_EINT8_23);		
}







/////////////////////////////////////////////////
//////	�������������ʼ��     //////////////
//////	IN 1~6   6·��������	    //////////////
/////   IN 7\8 �����������ж����� //////////////
////////////////////////////////////////////////
void Pulse_In_Init(void)	
{
	DWORD tmp;

	//���裺���㣬����
	tmp = rGPFCON & (~(0x3<< 2)) & (~(0x3<< 6)) ; //  & (~(0x3<< 14)) & (~(0x3<< 4))
	rGPFCON = tmp | (0x2<<2) | (0x2<<6);//set GPF1,GPF as EINT	X1  | (0x2<<14) | (0x2<<4)   GPF3Ҳ����Ϊ���жϡ�����������ΰ
	
	tmp = rGPFCON & (~(0x3<< 4))  ;//& (~(0x3<< 8))
	rGPFCON = tmp | (0x0<<4)  ;	//set GPF2 as INPUT	| (0x0<<6) | (0x0<<14)
	
   
	tmp = rGPGCON &(~(0x3<< 4)) &(~(0x3<< 6));
	rGPGCON = tmp | (0x2<<4) | (0x2<<6);//SET GPG2 AS EINT X10 X11
	
	
	rGPGUDP &= (~(0x3<< 4)) & (~(0x3<< 6)) & (~(0x3<< 8)); //GPF2~4 up down disable
	//tmp = rGPGUDP & (~(0x1<< 0));
	//rGPGUDP  = tmp | (0x1<<0) ;
	//////GPG0---X10��X11��X12=======GPG 2\3\4\5---�ⲿ�ж�
	///////////////////////
	
	rGPFUDP &=  (~(0x3<< 2)) & (~(0x3<< 4)) & (~(0x3<< 6)) & (~(0x3<< 8)) & (~(0x3<< 10)) & (~(0x3<< 12)) & (~(0x3<< 14)); //GPF1~7 up down disable
	
	//rINTMOD1 |= (1<<1);
	//rEXTINT0 = (rEXTINT0 & (~(0x7<<4))) | (0x4<<4);	// Eint1	Rising edge triggered
	rEXTINT0 = (rEXTINT0 & (~(0x7<<4))) | (0x6<<4);		// Eint1	BOTH edge triggered	�͹ܵ�������������ж�����
	rEXTINT0 = (rEXTINT0 & (~(0x7<<8))) | (0x6<<8);		// Eint2	BOTH edge triggered	
	rEXTINT0 = (rEXTINT0 & (~(0x7<<12)))| (0x4<<12);	// Eint3	Rising edge triggered	���ܱ����������ж�����
	rEXTINT1 = (rEXTINT1 & (~(0x7<<8))) | (0x2<<8);	    // Eint10	�½��ش���		������λ�ж�����
	rEXTINT1 = (rEXTINT1 & (~(0x7<<12))) | (0x2<<12);	// Eint11	�½��ش���		ɫ���ж�����
	
	
	
	
	rEXTINT0 = (rEXTINT0 & (~(0x7<<16))) | (0x4<<16);	// Eint4	Rising edge triggered
	rEXTINT0 = (rEXTINT0 & (~(0x7<<20))) | (0x4<<20);	// Eint5	Rising edge triggered	
	rEXTINT0 = (rEXTINT0 & (~(0x7<<24))) | (0x4<<24);	// Eint6	Rising edge triggered	
	rEXTINT0 = (rEXTINT0 & (~(0x7<<28)))| (0x4<<28);	// Eint7	Rising edge triggered	
	rEXTINT1 = (rEXTINT1 & (~(0x7<<0))) | (0x2<<0);   	// Eint8	FALLING edge triggered	
	//
	
	//
	//rEXTINT1 = (rEXTINT1 & (~(0x7<<12))) | (0x4<<12);	// Eint11	�����ش���	
		
	rEXTINT1 = (rEXTINT1 & (~(0x7<<16))) | (0x2<<16);	// Eint12	�½��ش���	
	//rEXTINT1 = (rEXTINT1 & (~(0x7<<20))) | (0x2<<20);	// Eint13	�½��ش���	
	 
	pISR_EINT1= (U32)PulseIn_1_Process;		// X1
	pISR_EINT2= (U32)PulseIn_2_Process;		// X2
	pISR_EINT3= (U32)PulseIn_3_Process;		// X3
	pISR_EINT4_7= (U32)PulseIn_4_7_Process;	// X4_7

	pISR_EINT8_23= (U32)PulseIn_8_23_Process;	// GPG0---X10��X11��X12��X13=======GPG 2\3\4\5
	
	rEINTPEND = 0xffffff;

	rSRCPND1 |= BIT_EINT1|BIT_EINT2|BIT_EINT3|BIT_EINT4_7|BIT_EINT8_23; //to clear the previous pending states
	rINTPND1 |= BIT_EINT1|BIT_EINT2|BIT_EINT3|BIT_EINT4_7|BIT_EINT8_23;

	//rEINTMASK &= ~((1<<2)|(1<<3)|(1<<4)|(1<<6));
	//rINTMSK1 &= ~(BIT_EINT2|BIT_EINT3|BIT_EINT4_7);
	
	InPulse1_Stop();
	InPulse2_Stop();
	InPulse3_Stop();
	InPulse4_Stop();
	InPulse5_Stop();
	InPulse6_Stop();
	//InPulse7_Stop();
	//InPulse8_Stop();
	InPulse10_Stop();
	InPulse11_Stop();
	InPulse12_Stop();
	//InPulse13_Stop();
	InPulse1_Start();
	InPulse2_Start();
	InPulse3_Start();
	InPulse10_Start();
	InPulse11_Start();
}





///////////////////////////////////////////////////////////////////////////
////	Y1 ���巢���жϷ������////////
///////////////////////////////////////////////////////////////////////////
void __irq PulseOut_1_Process(void)   
{
	OutPulse_Counter[1]++;
	
	rGPBDAT ^= (1<<1);
	
	if(x_dingchang_pb == 1 || x_dingchang_nb == 1|| (x_get_zero == 1 && x_get_zero_status == 0))// 
	{
		
		//accelerate(OutPulse_PreSet[1],frequence_Y1);
		 //����
		 if((OutPulse_Counter[1]/2) == 0)
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/6);
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/90)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/5);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/70)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/4);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/50)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/3);	
		}
		
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/40)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/2);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/30)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.5);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/25)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.3);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/20)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.1);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/18)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1);	
		}
		// if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/3)))
		// {
			// rTCNTB1= 300300/(frequence_Y1);	
		// }
		
		
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/20)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.1);	
		}
		
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/25)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.3);	
		}
		
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/30)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.5);
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/40)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/2);	
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/50)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/3);	
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/70)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/4);	
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/90)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/5);	
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/100)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/6);	
		} 
		
		
	/* 	if((OutPulse_Counter[1]/2) == 0)
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/5);
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/10)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/4);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/9)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/3);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/8)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/2);	
		}
		
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/7)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.8);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/6)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.5);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/5)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.3);	
		}
		
		if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/4)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1);	
		}
		
		
		// if((OutPulse_Counter[1]/2) == ((OutPulse_PreSet[1]/3)))
		// {
			// rTCNTB1= 300300/(frequence_Y1);	
		// }
		
		
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/3)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.3);	
		}
		
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/4)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.5);	
		}
		
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/5)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/1.8);
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/6)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/2);	
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/7)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/3);	
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/8)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/4);	
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/9)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/5);	
		}
		
		if((OutPulse_Counter[1]/2) == (OutPulse_PreSet[1]-(OutPulse_PreSet[1]/10)))
		{
			rTCNTB1= 300300/(unsigned int)(frequence_Y1/6);	
		}  */
	}
	
 	//�Ӵ�����жϳ���
	if(OutPulse_Counter[1]%2 == 0)
	{
		//�궨��������¼// 2017/5/8
		if(x_biaoding_move == 1 && biaoding_status[0] == 1)
		{
			++Y1_pulse_remember;
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
				
		}
		
		
		
/////////////////////////�������ӵķ��ش���	
		if(x_get_zero_keep_monitor == 1)
		{
			++Y1_pulse_remember;
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
		}
		//************�㶯��������¼************//	
		if((diandong_x_p == 1) && (jog_monitor_status[0] == 1))
		{
			++Y1_pulse_remember;
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
		}
		
		else if((diandong_x_n == 1) && (jog_monitor_status[0] == 1))
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
				
		}
		
		
		//********************************************//

		
		//*************������������¼*************//
		if  (x_dingchang_pb == 1 && dingchang_status[0] == 1)
		{
			++Y1_pulse_remember;
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
				
		}
		
		else if(x_dingchang_nb == 1 && dingchang_status[0] == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
		}
		
		//*****************************************//
		//****************������������¼**********//
			if(get_zero_status[0] == 1 && x_get_zero == 1 && x_get_zero_keep_monitor == 0)
			{
				--Y1_pulse_remember;
				if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
				{
					Y1_pulse_remember_add--;
					Y1_pulse_remember--;
				}
			}
		
			

			//ϵͳλ��������
		//**********ϵͳλ����������¼**********//
		
		//����λ��X����������¼
		if( gongzuo_b == 1 && xitong_status[0] == 1 )
		{
			if(monitor_gongzuo_flag[0] == 0)
				{
					++Y1_pulse_remember;
					if(Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember = 1;
						Y1_pulse_remember_add++;
					}
					
				}
				
			if (monitor_gongzuo_flag[0] == 1)
				{
					--Y1_pulse_remember;
					if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember_add--;
						Y1_pulse_remember--;
					}
					
				}		
		} 
		
		//����ȷ�� ���巵�ؼ�¼
		
		if(confirm_status[0] == 1 && confirm_monitor_x_status[0] == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
		}
		
		
	///////////////////////��ϴλ��//////////////////////////////
		if( clean_b == 1 && xitong_status[1] == 1 )
		{
			if(monitor_clean_flag[0] == 0)
				{
					++Y1_pulse_remember;	
					
					if(Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember = 1;
						Y1_pulse_remember_add++;
					}
				}
				
			if (monitor_clean_flag[0] == 1)
				{
					--Y1_pulse_remember;
					if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember_add--;
						Y1_pulse_remember--;
					}
				}		
		} 
		
		
		if(confirm_status[1] == 1 && confirm_monitor_x_status[1] == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
		}
		
		
	//////////////////ȡ��λ��//////////////////////
		if( quyang_b == 1 && xitong_status[2] == 1 )
		{
			if(monitor_quyang_flag[0] == 0)
				{
					++Y1_pulse_remember;
					
					if(Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember = 1;
						Y1_pulse_remember_add++;
					}
				}
				
			if (monitor_quyang_flag[0] == 1)
				{
					--Y1_pulse_remember;
					if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember_add--;
						Y1_pulse_remember--;
					}
				}		
		} 
		
		
		if(confirm_status[2] == 1 && confirm_monitor_x_status[2] == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
		}
		
	///////////////////////պ��//////////////////////	
		if( zhangan_b == 1 && xitong_status[3] == 1 )
		{
			if(monitor_zhangan_flag[0] == 0)
				{
					++Y1_pulse_remember;
					
					if(Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember = 1;
						Y1_pulse_remember_add++;
					}
				}
				
			if (monitor_zhangan_flag[0] == 1)
				{
					--Y1_pulse_remember;
					if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember_add--;
						Y1_pulse_remember--;
					}
				}		
		} 
		
		
		if(confirm_status[3] == 1 && confirm_monitor_x_status[3] == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
		}
		
	//////////////////����////////////////////////
		if( pingshan_b == 1 && xitong_status[4] == 1 )
		{
			if(monitor_pingshan_flag[0] == 0)
				{
					++Y1_pulse_remember;
					
					if(Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember = 1;
						Y1_pulse_remember_add++;
					}
				}
				
			if (monitor_pingshan_flag[0] == 1)
				{
					--Y1_pulse_remember;
					
					if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember_add--;
						Y1_pulse_remember--;
					}
				}		
		} 
		
		if(confirm_status[4] == 1 && confirm_monitor_x_status[4] == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
		}
		
	//////////////��Һ/////////////////
		if( tuye_b == 1 && xitong_status[5] == 1 )
		{
			if(monitor_tuye_flag[0] == 0)
				{
					++Y1_pulse_remember;
					
					if(Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember = 1;
						Y1_pulse_remember_add++;
					}
				}
				
			if (monitor_tuye_flag[0] == 1)
				{
					--Y1_pulse_remember;
					if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
					{
						Y1_pulse_remember_add--;
						Y1_pulse_remember--;
					}
				}		
		} 
		
		
		if(confirm_status[5] == 1 && confirm_monitor_x_status[5] == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
		}

	//////////////////////////////////////////////////////////

		
	//************΢�������¼*******************//
		 if((weitiao_p == 1) && (weitiao_status[0] == 1) && (xitong_x == 1))
		{ 
			++Y1_pulse_remember;
			
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
			
			if(weitiao_x_status[0] == 1)
			{
				++gongzuo_x;
			}
			
			if(weitiao_x_status[1] == 1)
			{
				++clean_x;
			}
			
			if(weitiao_x_status[2] == 1)
			{
				++quyang_x;
			}
			
			if(weitiao_x_status[3] == 1)
			{
				++zhangan_x;
			}
			
			if(weitiao_x_status[4] == 1)
			{
				++pingshan_x;
			}
			
			if(weitiao_x_status[5] == 1)
			{
				++tuye_x;
			} 
		} 
		
		
		 if((weitiao_n == 1) && (weitiao_status[0] == 1) && (xitong_x == 1))
		 {
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
			
			if(weitiao_x_status[0] == 1)
			{
				--gongzuo_x;
			}
			
			if(weitiao_x_status[1] == 1)
			{
				--clean_x;
			}
			
			if(weitiao_x_status[2] == 1)
			{
				--quyang_x;
			}
			
			if(weitiao_x_status[3] == 1)
			{
				--zhangan_x;
			}
			
			if(weitiao_x_status[4] == 1)
			{
				--pingshan_x;
			}
			
			if(weitiao_x_status[5] == 1)
			{
				--tuye_x;
			} 
		 }
		
		 
	////////////////�������������¼////////////////////// 
		if(work_zero_x_monitor_status  == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
		}

	/////////////////����clean�����¼/////////////
		if(work_clean_x_monitor_status == 1)
		{
			++Y1_pulse_remember;
			
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
			
		}
	///////////////ȡ��quyang�����¼/////////////
		if(work_quyang_x_monitor_status == 1)
		{
			++Y1_pulse_remember;
			
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
			
		}

	///////////////Ԥ���������¼////////////////
		if(work_pre_dianyang_x_monitor_status == 1)
		{
			++Y1_pulse_remember;
			
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
			
		}
	///////////////պ�������¼/////////////////
		if(work_zhangan_x_monitor_status == 1)
		{
			--Y1_pulse_remember;
			
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
			//Beep(1);
		}
		///////////////պ�ɻ��������¼/////////////////
		if(work_zhangan_x_back_monitor_status == 1)
		{
			++Y1_pulse_remember;
			
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
		}
	//////////////////////////////////////////////////	
		///////////////����s�����¼/////////////////
		if(work_dianyang_x_s_monitor_status == 1)
		{
			++Y1_pulse_remember;
			
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
			
		}
	//////////////////////////////////////////////////
		
	///////////////����m�����¼/////////////////
		if(work_dianyang_x_m_monitor_status == 1)
		{
			++Y1_pulse_remember;
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
			
			++Y1_dianyang_x_back_remember;
			if(Y1_dianyang_x_back_remember == 65535)
			{
				Y1_dianyang_x_back_remember = 1;
				Y1_pulse_remember_back_add++;
			}
				
		}
		
	///////////////����l�����¼/////////////////
		if(work_dianyang_x_l_monitor_status == 1)
		{
			++Y1_pulse_remember;
			if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
			
			++Y1_dianyang_x_back_remember;
			if(Y1_dianyang_x_back_remember == 65535)
			{
				Y1_dianyang_x_back_remember = 1;
				Y1_pulse_remember_back_add++;
			}
				
		}
	/////////////////////////////////////////////
		
	///////////////����x_back�����¼/////////////////
		if(work_dianyang_x_back_monitor_status == 1)
		{
			--Y1_pulse_remember;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
			//--Y1_dianyang_x_back_remember;
		}
	////////////////����x_s�����¼/////////////////////////////
		if(work_dianyang_x_s_hang_monitor_status == 1)
			{
				++Y1_pulse_remember;
				if(Y1_pulse_remember == 65535)
				{
					Y1_pulse_remember = 1;
					Y1_pulse_remember_add++;
				}
			
			}

	////////////////���������¼/////////////////////////////
		if(work_dianyang_x_get_zero_monitor_status == 1)
			{
				--Y1_pulse_remember;
				if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
				{
					Y1_pulse_remember_add--;
					Y1_pulse_remember--;
				}
			}
			
	///////////////����������ʼ�����///////////////////////
		 
	//////////////////����ת��x����(��ʼ��)////////////////////
		 if(work_dianyang_c_x_s_monitor_status == 1)
		 {
			 Y1_pulse_remember ++;
			 if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
		 }

	//////////////////����ת��x_s����////////////////////	 
		 if(work_dianyang_c_x_s_hang_monitor_status == 1)
		 {
			 Y1_pulse_remember ++;
			 if(Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			}
		 }
		 
	//////////////////��������ת��x����(��ʼ��)////////////////////
		 if(work_dianyang_c_x_m_monitor_status == 1)
		 {
			 Y1_pulse_remember ++;
			 if(Y1_pulse_remember == 65535)
			 {
				Y1_pulse_remember = 1;
				Y1_pulse_remember_add++;
			 }
		 }

	/////////////////��������ת��������//////////////////////
		if(work_dianyang_c_x_back_monitor_status == 1)
		{
			Y1_pulse_remember --;
			if(Y1_pulse_remember_add > 0 && Y1_pulse_remember == 65535)
			{
				Y1_pulse_remember_add--;
				Y1_pulse_remember--;
			}
				
		}
	}
	 //��������
	if((OutPulse_PreSet[1] > 0)&&(OutPulse_Counter[1] >= OutPulse_PreSet[1]*2))// ���ط����� Y1  
	{
		
		PulseOut_1_Stop(); 
		
		
		if(x_get_zero_status == 1 && x_get_zero_keep_status == 1)
		{
			x_get_zero_keep_status = 0;
			x_get_zero_keep_monitor = 0;
			x_get_zero = 0;
			x_get_zero_status = 0;
			
			Y1_pulse_remember = 0;
			Y1_pulse_remember_add = 0;
		}
		
	//********�ֶ����԰�������*********//
		x_dingchang_nb = 0;		
		x_dingchang_pb = 0;
		x_biaoding_move = 0;//2017/5/18 biaoding
		if(get_zero_status[0] == 1)
			x_get_zero_status = 1;
	//************************//
		/* //���������ر�
		if(dingchang_monitor_status[0]==1)
		{
			dingchang_monitor_status[0]=0;
		}
		
		if(dingchang_status[0] == 1)
		{
			dingchang_status[0] = 0;
		} */
		
		
	////////////////////////////////////
		
	//**********ϵͳλ�ð�������*****//
		if (xitong_status[0] == 1)
			gongzuo_flag_x = 1;
		
		if (xitong_status[1] == 1)
			clean_flag_x = 1;
		
		if (xitong_status[2] == 1)
			quyang_flag_x = 1;
		
		if (xitong_status[3] == 1)
			zhangan_flag_x = 1;
		
		if (xitong_status[4] == 1)
			pingshan_flag_x = 1;
		
		if (xitong_status[5] == 1)
			tuye_flag_x = 1;
		
		
	//********************************//

	//////////////ȷ�ϰ�ť�ر�΢������/////////
		if(confirm_status[0] == 1)
		{
		
		weitiao_x_status[0] = 0;	//ͬ��΢����־λ
		
		confirm_monitor_x_status[0] = 0;
		//xitong_c_status[0] = 0;
		}
		
		if(confirm_status[1] == 1)
		{
		
		weitiao_x_status[1] = 0;	//ͬ��΢����־λ
		
		confirm_monitor_x_status[1] = 0;
		//xitong_c_status[1] = 0;
		}
		
		if(confirm_status[2] == 1)
		{
		
		weitiao_x_status[2] = 0;	//ͬ��΢����־λ
		
		confirm_monitor_x_status[2] = 0;
		//xitong_c_status[2] = 0;
		}
		
		if(confirm_status[3] == 1)
		{
		
		weitiao_x_status[3] = 0;	//ͬ��΢����־λ
		
		confirm_monitor_x_status[3] = 0;
		//xitong_c_status[3] = 0;
		}
		
		if(confirm_status[4] == 1)
		{
		
		weitiao_x_status[4] = 0;	//ͬ��΢����־λ
		
		confirm_monitor_x_status[4] = 0;
	//	xitong_c_status[4] = 0;
		}
		
		if(confirm_status[5] == 1)
		{
		
		weitiao_x_status[5] = 0;	//ͬ��΢����־λ
		
		confirm_monitor_x_status[5] = 0;
	//	xitong_c_status[5] = 0;
		}
	
		
///////////////���������־λ����//////////
		if(work_zero_x_monitor_status  == 1)
		{
			work_zero_x_monitor_status = 0;
			work_zero_x_finish = 1;
		}
		
///////////////X����ϴλ�õ���֮��//////	
		if(work_clean_x_monitor_status == 1)
		{
		   work_clean_x_monitor_status = 0;
		}
		
		
		if(work_clean_x_status == 1)
		{
			work_clean_x_finish = 1;
			work_clean_x_status = 0;
		}
		
///////////////X��ȡ��λ�õ���֮��/////////
		if(work_quyang_x_monitor_status == 1)
		{
		   work_quyang_x_monitor_status = 0;
		}
		
		
		if(work_quyang_x_status == 1)
		{
			work_quyang_x_finish = 1;
			work_quyang_x_status = 0;
		}
////////////////////////////////////////////////////////////
		
///////////////X��Ԥ����λ�õ���֮��/////////
		if(work_pre_dianyang_x_monitor_status == 1)
		{
		   work_pre_dianyang_x_monitor_status = 0;
		}
		
		
		if(work_pre_dianyang_x_status == 1)
		{
			work_pre_dianyang_x_finish = 1;
			work_pre_dianyang_x_status = 0;
		}
////////////////////////////////////////////////////////////
///////////////X��պ��λ�õ���֮��/////////
		if(work_zhangan_x_monitor_status == 1)
		{
		   work_zhangan_x_monitor_status = 0;
		}	
		
		if(work_zhangan_x_status == 1)
		{
			work_zhangan_x_finish = 1;
			work_zhangan_x_status = 0;
		}
		
///////////////X��պ��λ�÷��ع���ԭ��֮��/////////
		if(work_zhangan_x_back_monitor_status == 1)
		{
		   work_zhangan_x_back_monitor_status = 0;
		}	
		
		if(work_zhangan_x_back_status == 1)
		{
			work_zhangan_x_back_finish = 1;
			work_zhangan_x_back_status = 0;
		}		
		
/////////////////////////���������̬������������/////////////////
	///////////////////�������x��s�����//////////////////////////
		
		if(work_dianyang_x_s_monitor_status == 1)
		{
			work_dianyang_x_s_monitor_status = 0;
		}
		
		if(work_dianyang_x_s_status == 1)
		{
			work_dianyang_x_s_status = 0;
			work_dianyang_x_s_finish = 1;
			
		}
		///////////////////�������x��m�����//////////////////////////
		
		if(work_dianyang_x_m_monitor_status == 1)
		{
			work_dianyang_x_m_monitor_status = 0;
		}
		
		if(work_dianyang_x_m_status == 1)
		{
			work_dianyang_x_m_status = 0;
			work_dianyang_x_m_finish = 1;
			
			//tmp_dianyang_jiance_hangshu++;
			
			work_dianyanging_m_status = 0;
			work_dianyanging_m_finish = 0;
			
		}
		
		///////////////////�������x��l�����//////////////////////////
		
		if(work_dianyang_x_l_monitor_status == 1)
		{
			work_dianyang_x_l_monitor_status = 0;
		}
		
		if(work_dianyang_x_l_status == 1)
		{
			work_dianyang_x_l_status = 0;
			
			work_dianyang_x_l_dianyang_status = 1;
			
			tmp_dianyang_jiance_lieshu = 1;	//��һ�� ���ǵ�0��
			
			tmp_dianyang_danyuan_lieshu++;
			
		}
		
		///////////////////�������x��back�����//////////////////////////
		if(work_dianyang_x_back_monitor_status == 1)
		{
			work_dianyang_x_back_monitor_status = 0;
		
			Y1_dianyang_x_back_remember = 0;
			Y1_pulse_remember_back_add = 0;
		}
		
		if(work_dianyang_x_back_status == 1)
		{
			work_dianyang_x_back_status = 0;
			work_dianyang_x_back_finish = 1;
			   
			if((tmp_dianyang_jiance_hangshu  < jiance_hangshu) && (tmp_dianyang_dianzhen_memory%2 == 0) && (tmp_dianyang_dianzhen_memory != 2 || tmp_dianyang_dianzhen_memory != 1))
			{
				 ++ tmp_dianyang_jiance_hangshu;
				 
				// if(tmp_dianyang_jiance_hangshu == jiance_hangshu)
					// work_dianyang_again_status = 1;
			}
			
				work_dianyang_x_s_finish = 0;
				work_dianyang_w_s_lie_finish = 0;
				work_dianyang_x_s_hang_finish = 0; 
				work_dianyang_w_m_lie_finish = 0;
				
				work_dianyang_y_down_finish = 0;
				work_dianyanging_s_finish = 0;
				work_dianyang_y_up_finish = 0;
				work_dianyang_x_back_finish = 0;
				work_dianyang_x_l_finish = 0;
				work_dianyanging_m_finish = 0;
				work_dianyang_x_l_dianyang_status = 0;
				work_dianyang_again_finish = 0;
				
			
				
			if(work_dianyang_again_status == 1)
			{
				work_dianyang_again_counter++;
			}
			
			
			if(work_dianyang_again_counter == 2)
			{
				work_dianyang_again_status = 0;
				work_dianyang_again_finish = 1;
				
				work_dianyang_again_counter = 0;
				
				work_dianyang_w_l_lie_finish = 0;
			}
			
			//��ֹ��һ��++�����ڼ��
			if(tmp_dianyang_jiance_hangshu == jiance_hangshu && work_dianyang_again_finish == 0)//&& tmp_dianyang_danyuan_lieshu < danyuan_lieshu && start_b == 1
			{
				// if(tmp_dianyang_danyuan_hangshu < danyuan_hangshu)
					// tmp_dianyang_danyuan_lieshu++;
				
				work_dianyang_again_status = 1;
				
				//work_dianyang_w_l_lie_finish = 0; �����Ҫ����work_dianyang_again_finish������
			}
			//�����������֮�󲻽������
			if(tmp_dianyang_danyuan_hangshu == danyuan_hangshu && tmp_dianyang_danyuan_lieshu == danyuan_lieshu
			   && tmp_dianyang_jiance_hangshu == jiance_hangshu && tmp_dianyang_jiance_lieshu == jiance_lieshu && work_dianyang_again_finish == 1)//
			{	
				work_dianyang_allow = 1;
			}
			
			tmp_dianyang_jiance_lieshu = 1;
			tmp_dianyang_danyuan_lieshu = 1;
			
			
			tmp_dianyang_dianzhen_memory++;
		} 
		
		///////////////////�������x_s��11 12���󵽴��//////////////////////////
		
		if(work_dianyang_x_s_hang_monitor_status == 1)
		{
			work_dianyang_x_s_hang_monitor_status = 0;
		}
		
		if(work_dianyang_x_s_hang_status == 1)
		{
			work_dianyang_x_s_hang_status = 0;
			work_dianyang_x_s_hang_finish = 1;
			
		}
		
	///////////////////�������x����㵽���//////////////////////////
		
		if(work_dianyang_x_get_zero_monitor_status == 1)
		{
			work_dianyang_x_get_zero_monitor_status = 0;
		}
		
		if(work_dianyang_x_get_zero_status == 1)
		{
			work_dianyang_x_get_zero_status = 0;
			work_dianyang_x_get_zero_finish = 1;
			
			if (work_dianyang_w_get_zero_finish == 1 && work_dianyang_x_get_zero_finish == 1 )//&& tmp_dianyang_dianzhen_memory%2 == 0
			{
				start_b = 0;
				work_dianyang_allow = 0;
				
				tmp_dianyang_danyuan_hangshu = 1;
				tmp_dianyang_danyuan_lieshu = 1;
				
				tmp_dianyang_jiance_hangshu = 1;
				tmp_dianyang_jiance_lieshu = 1;
				
				// work_dianyang_w_s_finish = 0;
				// work_dianyang_x_s_finish = 0;
				
				 work_get_zero_finish();
			}
			
		}
		
		
/////////////////////////���������̬������������/////////////////
/////////////////////////������������x��s�����(��ʼ��)///////////////////
		
		if(work_dianyang_c_x_s_monitor_status == 1)
		{
			work_dianyang_c_x_s_monitor_status = 0;
		}
		
		if(work_dianyang_c_x_s_status == 1)
		{
			work_dianyang_c_x_s_status = 0;
			work_dianyang_c_x_s_finish = 1;
			
			
		}
	

/////////////////////////������������x��m�����///////////////////
		
		if(work_dianyang_c_x_m_monitor_status == 1)
		{
			work_dianyang_c_x_m_monitor_status = 0;
		}
		
		if(work_dianyang_c_x_m_status == 1)//��ⴰ����֮�����˺���
		{
			work_dianyang_c_x_m_status = 0;
			work_dianyang_c_x_m_finish = 1;
			
			
			if(tmp_dianyang_jiance_lieshu < jiance_lieshu)
				tmp_dianyang_jiance_lieshu++;
			
			if(tmp_dianyang_danyuan_lieshu < danyuan_lieshu)
			{
				work_dianyang_c_x_l_finish = 0;
			}
			
			c_m_move_allow = 0;//���ܼ�����x���ƶ�
			c_s_move_allow = 0;//���ܼ�����x���ƶ�
			
			work_dianyang_c_start_status = 0;//���¿�ʼ����	
		}		
	
/////////////////////////������������x��l�����(��ʼ��)///////////////////		
		if(work_dianyang_c_x_l_monitor_status == 1)
		{
			work_dianyang_c_x_l_monitor_status = 0;
		}
		
		if(work_dianyang_c_x_l_status == 1)
		{
			work_dianyang_c_x_l_status = 0;
			work_dianyang_c_x_l_finish = 1;

			tmp_dianyang_danyuan_lieshu++;
			tmp_dianyang_jiance_lieshu = 1;
			
			c_l_move_allow = 0;
			c_m_move_allow = 0;
			c_s_move_allow = 0;
			
			c_jiance_lieshu_status = 0;
			
			tmp_dianyang_c_x_s_hang_remember = 0;
			
			tmp_dianyang_times_remember = 1;
			
			work_dianyang_c_start_status = 0;
			
		}
		
		
/////////////////////////������������x��s_hang�����(11/12����Ʒ��)///////////////////
		
		if(work_dianyang_c_x_s_hang_monitor_status == 1)
			work_dianyang_c_x_s_hang_monitor_status = 0;
		
		if(work_dianyang_c_x_s_hang_status == 1)
		{
			work_dianyang_c_x_s_hang_status = 0;
			work_dianyang_c_x_s_hang_finish = 1;
				
			//tmp_dianyang_c_x_s_hang_remember++;
			
			c_s_move_allow = 0;
			c_m_move_allow = 0;
			c_l_move_allow = 0;
			
			work_dianyang_c_start_status = 0;//���¿�ʼ����
		}
		
/////////////////////////������������x��back�����///////////////////	
		
		if(work_dianyang_c_x_back_monitor_status == 1)
		{
			work_dianyang_c_x_back_monitor_status = 0;
		}

		if(work_dianyang_c_x_back_status == 1)
		{
			work_dianyang_c_x_back_status = 0;
			work_dianyang_c_x_back_finish = 1;
			
		}
		
		OutPulse_Counter[1] = 0;	//��������Ŀ��Ҫ ��Pulseout_1_start()�еĴ��������˴����������
		
	 	ClearPending((U32)BIT_TIMER1);
	 	return ;
		
	}


	
	
	ClearPending((U32)BIT_TIMER1);

}



///////////////////////////////////////////////////////////////////////////
////	Y1 �����������//////////////////////////////
////	 ÿ����һ������ PulseOut_1_Start ֻ������һ��//////
////	frequence: ����Ƶ��///////////////////////////////
////	pulse_PreSet: ���������� ///////////////////////////
///////////////////////////////////////////////////////////////////////////
void PulseOut_1_Start(unsigned int frequence,   int pulse_PreSet)
{
	DWORD tmp;
	
	/* tmp = rGPBCON & (~(0x3<< 2));
	rGPBCON = tmp | (0x2<<2);	//set GPB1 as TOUT */
	
	if(x_dingchang_pb == 1 || x_dingchang_nb == 1)
	{
		frequence_Y1 = frequence;
		pulse_PreSet_Y1 = pulse_PreSet;
	}
	
	if(0 == pulse_PreSet)
	{
		PulseOut_1_Stop();
		
		if(work_zero_x_status == 1 && work_zero_x_monitor_status == 1)
		{
			work_zero_x_finish = 1;
			work_zero_x_monitor_status = 0;
			
		}
		//��pulse_PreSetΪ0�Ļ�����ôҲ��ʾ����
		
		return;
	}
	else
	{
		OutPulse_PreSet[1] = pulse_PreSet;
		//OutPulse_Counter[1] = 0;	// ȷ��ÿ������PulseOut_1_Start ������������ pulse_PreSet	//�����ǽ��˴�
	}
	
	
	if(0 == frequence)
	{
		frequence = 1;
		PulseOut_1_Stop();
		return;
	}
	rTCNTB1= 300300/frequence;		// 100909  	100K  	100000
	rTCMPB1 = rTCNTB1/2;

	rSRCPND1 = rSRCPND1 | ((U32)0x1<<11);   //��ն�ʱ��1Դ����
    rINTPND1 = rINTPND1 | ((U32)0x1<<11);    //��ն�ʱ��1�ж�����
   
	rINTMSK1 &=~(BIT_TIMER1);//�򿪶�ʱ��1�ж� 
	tmp = rTCON & (~(0xf << 8));	// dead zone Disable
	rTCON = tmp | (2 << 8);		// update TCVNTB0, stop			 
	rTCON = tmp | (9 << 8);		// interval mode,  start	
	/* rTCON |= (1<<9) | (1<<11);
	rTCON &= (~(1<<9));
	rTCON |= (1<<8); */
}

///////////////////////////////////////////////////////////////////////////
////	Y1 ���巢��ֹͣ////////
///////////////////////////////////////////////////////////////////////////
void PulseOut_1_Stop(void)
{
	//DWORD tmp;
	Set_Y_Value(1,0);	//ֱ�����͵�ƽ������ֱ��λ����
	
	rTCON &= ~(1 << 8);			/* Timer1, stop							*/
	rINTMSK1 |= BIT_TIMER1;
	ClearPending((U32)BIT_TIMER1);

	//OutPulse_Counter[1] = 0;

}


///////////////////////////////////////////////////////////////////////////
////	Y2 ���巢���жϷ������////////
///////////////////////////////////////////////////////////////////////////
void __irq PulseOut_2_Process(void) 
{
	OutPulse_Counter[2]++;
	
	rGPBDAT ^= (1<<2);
	
	if(y_dingchang_pb == 1 || y_dingchang_nb == 1)
	{
		
		//����
		if((OutPulse_Counter[2]/2) == 0)
		{
			rTCNTB2= 300300/(frequence_Y2/5);	
		}
		
		if((OutPulse_Counter[2]/2) == ((OutPulse_PreSet[2]/10)))
		{
			rTCNTB2= 300300/(frequence_Y2/3);	
		}
		
		if((OutPulse_Counter[2]/2) == ((OutPulse_PreSet[2]/8)))
		{
			rTCNTB2= 300300/(frequence_Y2/2);	
		}
		
		if((OutPulse_Counter[2]/2) == ((OutPulse_PreSet[2]/6)))
		{
			rTCNTB2= 300300/(frequence_Y2);	
		}
		
		
		//����
		if((OutPulse_Counter[2]/2) == (OutPulse_PreSet[2]-(OutPulse_PreSet[2]/6)))
		{
			rTCNTB2= 300300/(frequence_Y2/2);	
		}
		
		if((OutPulse_Counter[2]/2) == (OutPulse_PreSet[2]-(OutPulse_PreSet[2]/8)))
		{
			rTCNTB2= 300300/(frequence_Y2/3);	
		}
		
		if((OutPulse_Counter[2]/2) == (OutPulse_PreSet[2]-(OutPulse_PreSet[2]/10)))
		{
			rTCNTB2= 300300/(frequence_Y2/5);	
		}
	}
	
	if(OutPulse_Counter[2]%2 == 0)
{
	//y��궨	
	if(y_biaoding_move == 1 && biaoding_status[1] == 1)
		{
			++Y2_pulse_remember;
			if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
				
		}
		
		

	//�������ӵ�	
	if(y_get_zero_keep_monitor == 1)
		{
			++Y2_pulse_remember;
			
			if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
				
		}
	
//************�㶯��������¼************//		
	if((diandong_y_p == 1) && (jog_monitor_status[1] == 1))
	{
		++Y2_pulse_remember;
		if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
	}
	
	else if((diandong_y_n == 1) && (jog_monitor_status[1] == 1))
	{
		--Y2_pulse_remember;
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	}
	
	
	//********************************************//
	
	
	
	//*************������������¼*************//
	if  (y_dingchang_pb == 1 && dingchang_status[1] == 1)
	{
		++Y2_pulse_remember;
		 if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
	}
	
	else if(y_dingchang_nb == 1 && dingchang_status[1] == 1)
	{
		--Y2_pulse_remember;
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	}
	
	//*****************************************//
	
	//*************���������¼*********//
	if(get_zero_status[1] == 1 && y_get_zero == 1 && y_get_zero_keep_monitor == 0)
		{
			--Y2_pulse_remember;

			if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
		}
		
			//ϵͳλ��������
	//**********ϵͳλ����������¼**********//
 	
	//����λ��Y����������¼
	if( gongzuo_b == 1 && xitong_status[0] == 1 )
	{
		if(monitor_gongzuo_flag[1] == 0)
			{
				++Y2_pulse_remember;	
				if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
			
			}
			
		if (monitor_gongzuo_flag[1] == 1)
			{
				--Y2_pulse_remember;
				
				
				if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
				
			}		
	} 
	
	//����ȷ�� ���巵�ؼ�¼
	
	if(confirm_status[0] == 1 && confirm_monitor_y_status[0] == 1)
	{
		--Y2_pulse_remember;
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	}
	
	
///////////////////////��ϴλ��//////////////////////////////
	if( clean_b == 1 && xitong_status[1] == 1 )
	{
		if(monitor_clean_flag[1] == 0)
			{
				++Y2_pulse_remember;
				 if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
			
			}
			
		if (monitor_clean_flag[1] == 1)
			{
				--Y2_pulse_remember;
				
				if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
				
			}		
	} 
	
	
	if(confirm_status[1] == 1 && confirm_monitor_y_status[1] == 1)
	{
		--Y2_pulse_remember;
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	}
	
	
//////////////////ȡ��λ��//////////////////////
	if( quyang_b == 1 && xitong_status[2] == 1 )
	{
		if(monitor_quyang_flag[1] == 0)
			{
				++Y2_pulse_remember;	
				 if(Y2_pulse_remember == 65535)
				{
					Y2_pulse_remember = 1;
					Y2_pulse_remember_add++;
				}
			}
			
		if (monitor_quyang_flag[1] == 1)
			{
				--Y2_pulse_remember;
				
				if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
				{
					Y2_pulse_remember_add--;
					Y2_pulse_remember--;
				}
			}		
	} 
	
	
	if(confirm_status[2] == 1 && confirm_monitor_y_status[2] == 1)
	{
		--Y2_pulse_remember;
		
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	}
	
///////////////////////պ��//////////////////////	
	if( zhangan_b == 1 && xitong_status[3] == 1 )
	{
		if(monitor_zhangan_flag[1] == 0)
			{
				++Y2_pulse_remember;
				
				if(Y2_pulse_remember == 65535)
				{
					Y2_pulse_remember = 1;
					Y2_pulse_remember_add++;
				}
			}
			
		if (monitor_zhangan_flag[1] == 1)
			{
				--Y2_pulse_remember;
				
				
				if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
			}		
	} 
	
	
	if(confirm_status[3] == 1 && confirm_monitor_y_status[3] == 1)
	{
		--Y2_pulse_remember;
		
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	}
	
//////////////////����////////////////////////
	if( pingshan_b == 1 && xitong_status[4] == 1 )
	{
		if(monitor_pingshan_flag[1] == 0)
			{
				++Y2_pulse_remember;
			if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
			
			}
			
		if (monitor_pingshan_flag[1] == 1)
			{
				--Y2_pulse_remember;
				
				if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
				{
					Y2_pulse_remember_add--;
					Y2_pulse_remember--;
				}
			}		
	} 
	
	if(confirm_status[4] == 1 && confirm_monitor_y_status[4] == 1)
	{
		--Y2_pulse_remember;
		
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	}
	
//////////////��Һ/////////////////
	if( tuye_b == 1 && xitong_status[5] == 1 )
	{
		if(monitor_tuye_flag[1] == 0)
			{
				++Y2_pulse_remember;	
			if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
			}
			
		if (monitor_tuye_flag[1] == 1)
			{
				--Y2_pulse_remember;
				
				
				if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
			}		
	} 
	
	
	if(confirm_status[5] == 1 && confirm_monitor_y_status[5] == 1)
	{
		--Y2_pulse_remember;
		
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	}

//////////////////////////////////////////////////////////
	
//************΢�������¼*******************//
	 if((weitiao_p == 1) && (weitiao_status[1] == 1) && (xitong_y == 1))
	{ 
		++Y2_pulse_remember;
		if(Y2_pulse_remember == 65535)
		{
			Y2_pulse_remember = 1;
			Y2_pulse_remember_add++;
		}
		
		if(weitiao_y_status[0] == 1)
		{
			++gongzuo_y;
		}
		
		if(weitiao_y_status[1] == 1)
		{
			++clean_y;
		}
		
		if(weitiao_y_status[2] == 1)
		{
			++quyang_y;
		}
		
		if(weitiao_y_status[3] == 1)
		{
			++zhangan_y;
		}
		
		if(weitiao_y_status[4] == 1)
		{
			++pingshan_y;
		}
		
		if(weitiao_y_status[5] == 1)
		{
			++tuye_y;
		} 
	} 
	
	
	 if((weitiao_n == 1) && (weitiao_status[1] == 1) && (xitong_y == 1))
	 {
		--Y2_pulse_remember;
		
		
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
		
		if(weitiao_y_status[0] == 1)
		{
			--gongzuo_y;
		}
		
		if(weitiao_y_status[1] == 1)
		{
			--clean_y;
		}
		
		if(weitiao_y_status[2] == 1)
		{
			--quyang_y;
		}
		
		if(weitiao_y_status[3] == 1)
		{
			--zhangan_y;
		}
		
		if(weitiao_y_status[4] == 1)
		{
			--pingshan_y;
		}
		
		if(weitiao_y_status[5] == 1)
		{
			--tuye_y;
		} 
	 }

	 ////////////////////////////////////////////////////////////////

	////////////////////�������������¼///////////////////
	 if(work_zero_y_monitor_status == 1)
	 {
		 --Y2_pulse_remember;
		 
		 
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	 }
	
	///////////////////��ϴY���¼/////////////////
	 if(work_clean_y_monitor_status == 1)
	 {
		 ++Y2_pulse_remember;
		 
		if(Y2_pulse_remember == 65535)
		{
			Y2_pulse_remember = 1;
			Y2_pulse_remember_add++;
		}
	 }
	 
	 if(work_clean_back_monitor_status == 1)
	 {
		 --Y2_pulse_remember;
		 
		 
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	 }
	 
	 ///////////////////ȡ��Y���¼/////////////////
	 if(work_quyang_y_monitor_status == 1)
	 {
		 ++Y2_pulse_remember;
		 
		if(Y2_pulse_remember == 65535)
		{
			Y2_pulse_remember = 1;
			Y2_pulse_remember_add++;
		}
	 }
	 
	 if(work_quyang_back_monitor_status == 1)
	 {
		 --Y2_pulse_remember;
		 
		 
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	 }
	 
///////////////////Ԥ����Y���¼/////////////////
	 if(work_pre_dianyang_y_monitor_status == 1)
	 {
		 ++Y2_pulse_remember;
		  if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
	 }
	 
	 if(work_pre_dianyang_back_monitor_status == 1)
	 {
		 --Y2_pulse_remember;
		 
		 
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	 }
////////////////////////////////////////////////
	 
///////////////////պ��Y���¼/////////////////
	 if(work_zhangan_y_monitor_status == 1)
	 {
		 ++Y2_pulse_remember;
		  if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
	 }
	 
	 if(work_zhangan_back_monitor_status == 1)
	 {
		 --Y2_pulse_remember;
		 
		 
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	 }
	 
//////////////////����Y���¼//////////////////////
	 if(work_dianyang_y_down_monitor_status == 1)
	 {
		 ++Y2_pulse_remember;
		  if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
	 }
	 
//////////////////����Y������¼//////////////////////
	 if(work_dianyang_y_up_monitor_status == 1)
	 {
		 --Y2_pulse_remember;
		 
		 
		if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
	 }	 

	 
/////////////////////////Y������������������//////////////////////
/////////////////////////Y������س�λ�õ���֮��///////////////////////
		if(work_dianyang_c_y_down_monitor_status == 1)
		{
			++Y2_pulse_remember;	
			 if(Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember = 1;
				Y2_pulse_remember_add++;
			}
		}
		
		
/////////////////////////Y������س�λ�õ���֮��///////////////////////		
		if(work_dianyang_c_y_up_monitor_status == 1)
		{
			--Y2_pulse_remember;	
			
				
			if(Y2_pulse_remember_add > 0 && Y2_pulse_remember == 65535)
			{
				Y2_pulse_remember_add--;
				Y2_pulse_remember--;
			}
				
		}
		
		
}		
	//���������¼
	if((OutPulse_PreSet[2] > 0)&&(OutPulse_Counter[2] >= OutPulse_PreSet[2]*2))
{
		
		//Beep(1);
		
		PulseOut_2_Stop();
		
		if(y_get_zero_status == 1 && y_get_zero_keep_status == 1)
		{
			y_get_zero_keep_status = 0;
			y_get_zero_keep_monitor = 0;
			y_get_zero = 0;
			y_get_zero_status = 0;
			
			Y2_pulse_remember = 0;
			Y2_pulse_remember_add = 0;
		}
		
		//*******��������********//
			y_dingchang_nb = 0;		
			y_dingchang_pb = 0;
			y_biaoding_move = 0;//Y��궨
			if(get_zero_status[1] == 1)
				y_get_zero_status = 1;
		//**********************//
		if(dingchang_monitor_status[1]==1)
		{
			dingchang_monitor_status[1]=0;
		}
		
		if(dingchang_status[1] == 1)
		{
			dingchang_status[1] = 0;
		}
			
		//////////Y��㶯��عر�//////
		if(jog_status[1] == 1)
		 jog_status[1] = 0;
		
		
		//*******ϵͳλ�õ���******//	
			if (xitong_status[0] == 1)
			gongzuo_flag_y = 1;
			
			if (xitong_status[1] == 1)
			clean_flag_y = 1;
			
			if (xitong_status[2] == 1)
			quyang_flag_y = 1;
			
			if (xitong_status[3] == 1)
			zhangan_flag_y = 1;
			
			if (xitong_status[4] == 1)
			pingshan_flag_y = 1;
			
			if (xitong_status[5] == 1)
			tuye_flag_y = 1;
			
		//********************************//

	//////////////ȷ�ϰ�ť�ر�΢������/////////
		if(confirm_status[0] == 1)
		{
			weitiao_y_status[0] = 0;		//��Ҫ����Y2��
	
			confirm_monitor_y_status[0] = 0;
		}
		
		
		if(confirm_status[1] == 1)
		{
			weitiao_y_status[1] = 0;

			confirm_monitor_y_status[1] = 0;
		}
		
		if(confirm_status[2] == 1)
		{
			weitiao_y_status[2] = 0;
			
			confirm_monitor_y_status[2] = 0;
		}
		
		if(confirm_status[3] == 1)
		{
			weitiao_y_status[3] = 0;

			confirm_monitor_y_status[3] = 0;
		}
		
		if(confirm_status[4] == 1)
		{
			weitiao_y_status[4] = 0;
		
			confirm_monitor_y_status[4] = 0;
		}
		
		if(confirm_status[5] == 1)
		{
			weitiao_y_status[5] = 0;
			
			confirm_monitor_y_status[5] = 0;

		}
		////////////////////���������־λ����//////��Ӧ���ڴ˴����㣬���ظ����뺯��,���finish_flag��֤�����ظ����뺯��
		if(work_zero_y_monitor_status == 1)
		{
			work_zero_y_monitor_status = 0;
			work_zero_y_finish = 1;
		}
		
		
	//////////////��ϴλ�õ���֮��//////////////
		if(work_clean_y_monitor_status == 1)
		{
			work_clean_y_monitor_status = 0;
		}
		
		if(work_clean_back_monitor_status == 1)
		{
			work_clean_back_monitor_status = 0;
		}
		
		if(work_clean_y_status == 1)
		{
			work_clean_y_status = 0;
			work_clean_y_finish = 1;
		}
		
		if(work_clean_back_status == 1)
		{
			work_clean_back_status = 0;
			
			work_clean_back_finish = 1;
		}
		
		//////////////quyangλ�õ���֮��//////////////
		if(work_quyang_y_monitor_status == 1)
		{
			work_quyang_y_monitor_status = 0;
		}
		
		if(work_quyang_back_monitor_status == 1)
		{
			work_quyang_back_monitor_status = 0;
		}
		
		if(work_quyang_y_status == 1)
		{
			work_quyang_y_status = 0;
			work_quyang_y_finish = 1;
		}
		
		if(work_quyang_back_status == 1)
		{
			work_quyang_back_status = 0;
			
			work_quyang_back_finish = 1;
		}
//////////////////////////////////////////////////////////////////////////////	
		
/////////////////////Ԥ����λ�õ���֮��////////////////////////////
		if(work_pre_dianyang_y_monitor_status == 1)
		{
			work_pre_dianyang_y_monitor_status = 0;
		}
		
		if(work_pre_dianyang_back_monitor_status == 1)
		{
			work_pre_dianyang_back_monitor_status = 0;
		}
		
		if(work_pre_dianyang_y_status == 1)
		{
			work_pre_dianyang_y_status = 0;
			work_pre_dianyang_y_finish = 1;
		}
		
		if(work_pre_dianyang_back_status == 1)
		{
			work_pre_dianyang_back_status = 0;
			
			work_pre_dianyang_back_finish = 1;
		}
//////////////////////////////////////////////////////////////////////////////
		
/////////////////////zhanganλ�õ���֮��////////////////////////////
		if(work_zhangan_y_monitor_status == 1)
		{
			work_zhangan_y_monitor_status = 0;
		}
		
		if(work_zhangan_back_monitor_status == 1)
		{
			work_zhangan_back_monitor_status = 0;
		}
		
		if(work_zhangan_y_status == 1)
		{
			work_zhangan_y_status = 0;
			work_zhangan_y_finish = 1;
		}
		
		if(work_zhangan_back_status == 1)
		{
			work_zhangan_back_status = 0;
			work_zhangan_back_finish = 1;
		}
//////////////////////////////////////////////////////////////////////////////
/////////////////////////Y�����λ�õ���֮��///////////////////////
		if(work_dianyang_y_down_monitor_status == 1)
			work_dianyang_y_down_monitor_status = 0;
		
		if(work_dianyang_y_down_status == 1)
		{
			//tmp_dianyang_jiance_hangshu = 1;
			
			
			work_dianyang_y_down_status = 0;
			work_dianyang_y_down_finish = 1;
		}
		
/////////////////////////Y������س�λ�õ���֮��///////////////////////
		if(work_dianyang_y_up_monitor_status == 1)
			work_dianyang_y_up_monitor_status = 0;
		
		if(work_dianyang_y_up_status == 1)
		{			
			work_dianyang_y_up_status = 0;
			work_dianyang_y_up_finish = 1;
		}

/////////////////////////Y������������������//////////////////////
/////////////////////////Y�����λ�õ���֮��///////////////////////
		if(work_dianyang_c_y_down_monitor_status == 1)
			work_dianyang_c_y_down_monitor_status = 0;
		
		if(work_dianyang_c_y_down_status == 1)
		{			
			work_dianyang_c_y_down_status = 0;
			work_dianyang_c_y_down_finish = 1;
		}		
/////////////////////////Y������س�λ�õ���֮��///////////////////////	
		if(work_dianyang_c_y_up_monitor_status == 1)
			work_dianyang_c_y_up_monitor_status = 0;
		
		
		if(work_dianyang_c_y_up_status == 1)
		{			
			work_dianyang_c_y_up_status = 0;
			work_dianyang_c_y_up_finish = 1;
			
			//Beep(1);
		}
		
		OutPulse_Counter[2] = 0;//��������Ҫ
		
			ClearPending((U32)BIT_TIMER2);
			return;
		}
		

		
		
	ClearPending((U32)BIT_TIMER2);

}



///////////////////////////////////////////////////////////////////////////
////	Y2 �����������//////////////////////////////
////	 ÿ����һ������ PulseOut_2_Start ֻ������һ��//////
////	frequence: ����Ƶ��///////////////////////////////
////	pulse_PreSet: ���������� ///////////////////////////
///////////////////////////////////////////////////////////////////////////
void PulseOut_2_Start(unsigned int frequence,  int pulse_PreSet)
{
	DWORD tmp;
	
	if(y_dingchang_pb == 1 || y_dingchang_nb == 1)
	{
		frequence_Y2 = frequence;
		pulse_PreSet_Y2 = pulse_PreSet;		
	}
	if(0 == pulse_PreSet)
	{
		PulseOut_2_Stop();
		
		if(work_zero_y_status == 1 && work_zero_y_monitor_status == 1)
		{
			work_zero_y_finish = 1;
			work_zero_y_monitor_status = 0;
		}
			
		return;
	}
	else
	{
		OutPulse_PreSet[2] = pulse_PreSet;
		//OutPulse_Counter[2] = 0;	// ȷ��ÿ������PulseOut_2_Start ������������ pulse_PreSet
	}
	
	/* OutPulse_PreSet[2] = pulse_PreSet;
	OutPulse_Counter[2] = 0;
	InPulseCount[3]=0; */
	
	if(0 == frequence)
	{
		frequence = 1;
		PulseOut_2_Stop();
		return;
	}
	
	rTCNTB2= 300300/frequence;	// 100909  	100K  	100000
	rTCMPB2 = rTCNTB2/2;
	
	rSRCPND1 = rSRCPND1 | ((U32)0x1<<12);   //��ն�ʱ��2Դ����
    rINTPND1 = rINTPND1 | ((U32)0x1<<12);    //��ն�ʱ��2�ж�����
  
	rINTMSK1 &=~(BIT_TIMER2);//�򿪶�ʱ��2�ж� 
	tmp = rTCON & (~(0xf << 12));	// dead zone Disable
	rTCON = tmp | (2 << 12)	;/* update TCVNTB0, stop					*/
	rTCON = tmp | (9 << 12)	;/* interval mode,  start				*/
	/* rTCON |= (1<<13) | (1<<15);
	rTCON &= (~(1<<13));
	rTCON |= (1<<12); */
}

///////////////////////////////////////////////////////////////////////////
////	Y2 ���巢��ֹͣ////////
///////////////////////////////////////////////////////////////////////////
void PulseOut_2_Stop(void)
{
	//DWORD tmp;
	
	Set_Y_Value(2,0);
	
	rTCON &= ~(1 << 12);		/* Timer2, stop	*/
	rINTMSK1 |= BIT_TIMER2;
	//OutPulse_Counter[2] = 0;
}


/////////////����Y3δ����////////////////////
///////////////////////////////////////////////////////////////////////////
////	Y3 ���巢���жϷ������////////
///////////////////////////////////////////////////////////////////////////
void __irq PulseOut_3_Process(void)
{
	
	OutPulse_Counter[3]++; 
	
	rGPBDAT ^= (1<<3);
	
	if(w_dingchang_pb == 1 || w_dingchang_nb == 1|| (w_get_zero == 1 && w_get_zero_status == 0))// 
	{
			if((OutPulse_Counter[3]/2) == 0)
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/3);
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/10)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/2);	
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/9)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.25);	
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/8)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1);	
		}
		
		// if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/7)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.25);	
		// }
		
		// if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/7)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.8);	
		// }
		
		// if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/6)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.5);	
		// }
		
		// if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/5)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.3);	
		// }
		
		// if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/4)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1);	
		// }
		
		
		// if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/3)))
		// {
			// rTCNTB3= 300300/(frequence_Y3);	
		// }
		
		
		
		// if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/3)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.3);	
		// }
		
		
		// if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/4)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.5);	
		// }
		
		
		// if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/5)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.8);
		// }
		
		// if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/6)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/2);	
		// }
		
		// if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/7)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/3);	
		// }
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/8)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.25);	
		}
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/9)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/2);	
		}
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/10)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/3);	
		} 
		/* //accelerate(OutPulse_PreSet[3],frequence_Y3);
		 //����
		if((OutPulse_Counter[3]/2) == 0)
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/3);
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/10)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/2);	
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/9)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.8);	
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/8)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.6);	
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/7)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.4);	
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/6)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.2);	
		}
		
		if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/5)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1);	
		}
		
		// if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/4)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.2);	
		// }
		
		// if((OutPulse_Counter[3]/2) == ((OutPulse_PreSet[3]/3)))
		// {
			// rTCNTB3= 300300/(frequence_Y3);	
		// }
		
		
		
		//����
		
		// if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/3)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.2);	
		// }
		
		
		// if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/4)))
		// {
			// rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.5);	
		// }
		
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/5)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.2);
		}
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/6)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.4);	
		}
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/7)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.6);	
		}
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/8)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/1.8);	
		}
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/9)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/2);	
		}
		
		if((OutPulse_Counter[3]/2) == (OutPulse_PreSet[3]-(OutPulse_PreSet[3]/10)))
		{
			rTCNTB3= 300300/(unsigned int)(frequence_Y3/3);	
		}  */
		
		
		
		
	}
	
	if(OutPulse_Counter[3]%2 == 0)
{

	//w��궨
	if(w_biaoding_move == 1 && biaoding_status[2] == 1)
		{
			++Y3_pulse_remember;
			if(Y3_pulse_remember == 65535)
			{
				Y3_pulse_remember = 1;
				Y3_pulse_remember_add++;
			}
				
		}
	//�������ӵķ�������
	if(w_get_zero_keep_monitor == 1)
	{
		++Y3_pulse_remember;
		
		if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	}
	
	//�ֶ�����������
	//************�㶯��������¼************//
	
	if((diandong_w_p == 1) && (jog_monitor_status[2] == 1))
	{
		++Y3_pulse_remember;
		
		 if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	}
	
	else if((diandong_w_n == 1) && (jog_monitor_status[2] == 1))
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
			
	}
	
	
	//********************************************//
	
	
	
	//*************������������¼*************//
	if  (w_dingchang_pb == 1 && dingchang_status[2] == 1)
	{
		++Y3_pulse_remember;
		
		 if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	}
	
	else if(w_dingchang_nb == 1 && dingchang_status[2] == 1)
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}
	
	//*****************************************//
	//************���������¼
	if(get_zero_status[2] == 1 && w_get_zero == 1 && w_get_zero_keep_monitor == 0)
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}
	
		//**********ϵͳλ����������¼**********//
 	
	//����λ��Y����������¼
	if( gongzuo_b == 1 && xitong_status[0] == 1 )
	{
		if(monitor_gongzuo_flag[2] == 0)
			{
				++Y3_pulse_remember;
				
				 if(Y3_pulse_remember == 65535)
				 {
					 Y3_pulse_remember = 1;
					 Y3_pulse_remember_add++;
				 }
			}
			
		if (monitor_gongzuo_flag[2] == 1)
			{
				--Y3_pulse_remember;
				
				if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
				{
					Y3_pulse_remember_add--;
					Y3_pulse_remember--;
				}
				
			}		
	} 
	
	//����ȷ�� ���巵�ؼ�¼
	
	if(confirm_status[0] == 1 && confirm_monitor_w_status[0] == 1)
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}
	
	
///////////////////////��ϴλ��//////////////////////////////
	if( clean_b == 1 && xitong_status[1] == 1 )
	{
		if(monitor_clean_flag[2] == 0)
			{
				++Y3_pulse_remember;	
				
				 if(Y3_pulse_remember == 65535)
				 {
					 Y3_pulse_remember = 1;
					 Y3_pulse_remember_add++;
				 }
			}
			
		if (monitor_clean_flag[2] == 1)
			{
				--Y3_pulse_remember;
				
				if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
				{
					Y3_pulse_remember_add--;
					Y3_pulse_remember--;
				}
				
			}		
	} 
	
	
	if(confirm_status[1] == 1 && confirm_monitor_w_status[1] == 1)
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}
	
	
//////////////////ȡ��λ��//////////////////////
	if( quyang_b == 1 && xitong_status[2] == 1 )
	{
		if(monitor_quyang_flag[2] == 0)
			{
				++Y3_pulse_remember;
				
				 if(Y3_pulse_remember == 65535)
				 {
					 Y3_pulse_remember = 1;
					 Y3_pulse_remember_add++;
				 }
			}
			
		if (monitor_quyang_flag[2] == 1)
			{
				--Y3_pulse_remember;
				
				if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
				{
					Y3_pulse_remember_add--;
					Y3_pulse_remember--;
				}
			}		
	} 
	
	
	if(confirm_status[2] == 1 && confirm_monitor_w_status[2] == 1)
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}
	
///////////////////////պ��//////////////////////	
	if( zhangan_b == 1 && xitong_status[3] == 1 )
	{
		if(monitor_zhangan_flag[2] == 0)
			{
				++Y3_pulse_remember;
				
				 if(Y3_pulse_remember == 65535)
				 {
					 Y3_pulse_remember = 1;
					 Y3_pulse_remember_add++;
				 }
			}
			
		if (monitor_zhangan_flag[2] == 1)
			{
				--Y3_pulse_remember;
				
				if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
				{
					Y3_pulse_remember_add--;
					Y3_pulse_remember--;
				}
			}		
	} 
	
	
	if(confirm_status[3] == 1 && confirm_monitor_w_status[3] == 1)
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}
	
//////////////////����////////////////////////
	if( pingshan_b == 1 && xitong_status[4] == 1 )
	{
		if(monitor_pingshan_flag[2] == 0)
			{
				++Y3_pulse_remember;	
				
				 if(Y3_pulse_remember == 65535)
				 {
					 Y3_pulse_remember = 1;
					 Y3_pulse_remember_add++;
				 }
			}
			
		if (monitor_pingshan_flag[2] == 1)
			{
				--Y3_pulse_remember;
				
				if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
				{
					Y3_pulse_remember_add--;
					Y3_pulse_remember--;
				}
			}		
	} 
	
	if(confirm_status[4] == 1 && confirm_monitor_w_status[4] == 1)
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}
	
//////////////��Һ/////////////////
	if( tuye_b == 1 && xitong_status[5] == 1 )
	{
		if(monitor_tuye_flag[2] == 0)
			{
				++Y3_pulse_remember;
				
				 if(Y3_pulse_remember == 65535)
				 {
					 Y3_pulse_remember = 1;
					 Y3_pulse_remember_add++;
				 }
			}
			
		if (monitor_tuye_flag[2] == 1)
			{
				--Y3_pulse_remember;
				
				if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
				{
					Y3_pulse_remember_add--;
					Y3_pulse_remember--;
				}
			}		
	} 
	
	
	if(confirm_status[5] == 1 && confirm_monitor_w_status[5] == 1)
	{
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}

//////////////////////////////////////////////////////////
	
//************΢�������¼*******************//
	 if((weitiao_p == 1) && (weitiao_status[2] == 1) && (xitong_w == 1))
	{ 
		++Y3_pulse_remember;
		
		 if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
		
		if(weitiao_w_status[0] == 1)
		{
			++gongzuo_w;
		}
		
		if(weitiao_w_status[1] == 1)
		{
			++clean_w;
		}
		
		if(weitiao_w_status[2] == 1)
		{
			++quyang_w;
		}
		
		if(weitiao_w_status[3] == 1)
		{
			++zhangan_w;
		}
		
		if(weitiao_w_status[4] == 1)
		{
			++pingshan_w;
		}
		
		if(weitiao_w_status[5] == 1)
		{
			++tuye_w;
		} 
	} 
	
	
	 if((weitiao_n == 1) && (weitiao_status[2] == 1) && (xitong_w == 1))
	 {
		--Y3_pulse_remember;
		
		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
		
		if(weitiao_w_status[0] == 1)
		{
			--gongzuo_w;
		}
		
		if(weitiao_w_status[1] == 1)
		{
			--clean_w;
		}
		
		if(weitiao_w_status[2] == 1)
		{
			--quyang_w;
		}
		
		if(weitiao_w_status[3] == 1)
		{
			--zhangan_w;
		}
		
		if(weitiao_w_status[4] == 1)
		{
			--pingshan_w;
		}
		
		if(weitiao_w_status[5] == 1)
		{
			--tuye_w;
		} 
	 }
	 
///////////////////////////////////////////////////////////////////	
	 
/////////////////////�������������¼///////////////////////////////
	 if(work_zero_w_monitor_status == 1)
	 {
		 --Y3_pulse_remember;
		 
		 if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	 }
	 
///////////////////Ԥ���������¼//////////////////
	 if(work_pre_dianyang_w_monitor_status == 1)
	 {
		 ++Y3_pulse_remember;
		 
		  if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	 }
///////////////////////////////////////////////////////////////////
	 
///////////////////պ�������¼//////////////////
	 if(work_zhangan_w_monitor_status == 1)
	 {
		if(zhangan_w > gongzuo_w)
			{
				++Y3_pulse_remember;
				
				 if(Y3_pulse_remember == 65535)
				 {
					 Y3_pulse_remember = 1;
					 Y3_pulse_remember_add++;
				 }
			}
		if(zhangan_w <= gongzuo_w)	
			{
				--Y3_pulse_remember;
				
				if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
				{
					Y3_pulse_remember_add--;
					Y3_pulse_remember--;
				}
			}
	 }
///////////////////պ�ɻ��������¼//////////////////	 
	 if(work_zhangan_w_back_monitor_status == 1)
	 {
		if(zhangan_w > gongzuo_w)
		{
			--Y3_pulse_remember;

			if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
			{
				Y3_pulse_remember_add--;
				Y3_pulse_remember--;
			}
		}
			
		
		if(zhangan_w <= gongzuo_w)
		{
			++Y3_pulse_remember;
			
			 if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
		}
			
	 }
	 
	 
/////////////////////�����������//////////////////////////////////// 
	
//////////////////����ת��W����(��ʼ��)////////////////////
	 if(work_dianyang_w_s_monitor_status == 1)
	 {
		 Y3_pulse_remember ++;
		 
		  if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	 }

//////////////////����ת��W_s����(����11 12)////////////////////
	 if(work_dianyang_w_s_lie_monitor_status == 1)
	 {
		 Y3_pulse_remember ++;
		 
		  if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	 }
	 
//////////////////����ת��W_m����(����11 12)////////////////////
	 if(work_dianyang_w_m_lie_monitor_status == 1)
	 {
		 Y3_pulse_remember ++;
		 
		  if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	 }
	
//////////////////����ת��W_L����(����11 12)////////////////////
	 if(work_dianyang_w_l_lie_monitor_status == 1)
	 {
		 Y3_pulse_remember ++;
		 
		  if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	 }

//////////////////����ת��������////////////////////
	 if(work_dianyang_w_get_zero_monitor_status == 1)
	 {
		 Y3_pulse_remember --;
		 
		 if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	 }

	 
///////////////����������ʼ�����///////////////////////
	 
//////////////////����ת��W����(��ʼ��)////////////////////
	 if(work_dianyang_c_w_s_monitor_status == 1)
	 {
		 Y3_pulse_remember ++;
		 
		  if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
	 }
	
//////////////////����ת��W������ת�����////////////////////	 
	 if(work_dianyang_c_w_start_monitor_status == 1)
	{
		 Y3_pulse_remember ++;
		 
		 if(Y3_pulse_remember == 65535)
		 {
			 Y3_pulse_remember = 1;
			 Y3_pulse_remember_add++;
		 }
			
	} 
	 
	if(Y3_pulse_remember == spotted_c_remember && work_dianyang_c_start_status== 0 && work_dianyang_c_w_start_status == 1)//�˴���Ҫ��������
	{
		work_dianyang_c_start_status = 1;
		work_dianyang_c_start = 1;
	}
	
	

/////////////////ת�������еĽ��е���.�ƶ�/////////////////
	if(work_dianyang_c_start == 1)
	{
		if(Y3_pulse_remember == spotted_c_remember)//��¼���˶���������
			{
				tmp_dianyang_times ++;
			}
			
		if(tmp_dianyang_times == (dianyang_times+1))//ÿ��ֹͣ����
		{
			//Beep(1);
			work_dianyang_c_start = 0;//ֹͣ��ǰ����
			
			 if(dianzhen_bianhao == 11 || dianzhen_bianhao == 12) 
			{
				tmp_dianyang_c_x_s_hang_remember++;	
			}
			
			c_s_move_allow = 1;//��ʼ����s
			c_m_move_allow = 1;//m/l�ƶ�
			
			if(tmp_dianyang_jiance_lieshu == jiance_lieshu)//ÿһ��Ԫ���������һ���������
			{
				c_jiance_lieshu_status = 1;//��������������֮�󣬲���m�ƶ�
				c_l_move_allow = 1;//��������������֮�󣬿�ʼL�ƶ���־λ
				
			} 	
			
			if(tmp_dianyang_jiance_lieshu == jiance_lieshu && tmp_dianyang_danyuan_lieshu == danyuan_lieshu)
			{
				 c_m_move_allow = 0; 
				 work_dianyang_c_start = 0;
				 // work_dianyang_c_x_m_finish = 0;
				 // work_dianyang_c_x_s_hang_finish = 0;

				 work_dianyang_c_working_finish  = 1;
			}
			
				//2017/4/22
			work_dianyang_c_x_m_finish = 0;
			work_dianyang_c_x_s_hang_finish = 0;
						
			tmp_dianyang_times_remember = 1;//ǰһ�ֵ�������֮��׼����ʼ�µ�һ�ֵ���
			tmp_dianyang_times = 0;
			
			
			
		}
		
		
		if(dianzhen_bianhao != 11 && dianzhen_bianhao != 12 && tmp_dianyang_jiance_hangshu <= jiance_hangshu 
		  && tmp_dianyang_danyuan_hangshu <= danyuan_hangshu && tmp_dianyang_times <= dianyang_times && c_m_move_allow == 0)
		{
			
			if((spotted_c_remember+((tmp_dianyang_jiance_hangshu - 1))*(c_m_distance)+(tmp_dianyang_danyuan_hangshu - 1)*(c_l_distance) == Y3_pulse_remember)
				||(spotted_c_remember+(tmp_dianyang_jiance_hangshu - 1)*(c_m_distance)+(tmp_dianyang_danyuan_hangshu - 1)*(c_l_distance)+ c_s_distance == Y3_pulse_remember))
			{
				
				work_fn_dianyang();
			}
	
		}

		
		if((dianzhen_bianhao == 11 || dianzhen_bianhao == 12) && tmp_dianyang_jiance_hangshu <= jiance_hangshu 
		  && tmp_dianyang_danyuan_hangshu <= danyuan_hangshu && tmp_dianyang_times <= dianyang_times && (c_m_move_allow == 0 || c_s_move_allow == 0))
		{
			if((spotted_c_remember+(tmp_dianyang_jiance_hangshu - 1)*(c_m_distance)+(tmp_dianyang_danyuan_hangshu - 1)*(c_l_distance) == Y3_pulse_remember))
			{
				work_fn_dianyang();
			}
			
		}
	}
	 
	 
	 
	 
	 
	 
	 
/////////////��������W��������////////////////////
	if(work_dianyang_c_w_stop_status_monitor == 1)
	{
		Y3_pulse_remember--;

		if(Y3_pulse_remember_add > 0 && Y3_pulse_remember == 65535)
		{
			Y3_pulse_remember_add--;
			Y3_pulse_remember--;
		}
	}
		
	
	 
	 //***********��������Ȧ��λ�ü�¼*********//
	/* if(start_b == 1 && start_status[0] == 1)
	{
		w_pulse_number++;
		
		
		
	}
	 */
		
}
	if((OutPulse_PreSet[3] > 0)&&(OutPulse_Counter[3] >= OutPulse_PreSet[3]*2))
	{	
		PulseOut_3_Stop();
		
		if(w_get_zero_status == 1 && w_get_zero_keep_status == 1)
		{
			w_get_zero_keep_status = 0;
			w_get_zero_keep_monitor = 0;
			w_get_zero = 0;
			w_get_zero_status = 0;
			
			Y3_pulse_remember = 0;
			Y3_pulse_remember_add = 0;
		}
		//********��������********//
			w_dingchang_nb = 0;		
			w_dingchang_pb = 0;
			w_biaoding_move = 0;
			if(get_zero_status[2] == 1)
				w_get_zero_status = 1;
		//**********************//
		
		//�رն������
		if(dingchang_monitor_status[2]==1)
		{
			dingchang_monitor_status[2]=0;
		}
		
		if(dingchang_status[2] == 1)
		{
			dingchang_status[2] = 0;
		}
			
		//////////W��㶯��عر�//////
		if(jog_status[2] == 1)
		 jog_status[2] = 0;

		
		//*********ϵͳλ������***//
		if (xitong_status[0] == 1)
			gongzuo_flag_w = 1;
		
		if (xitong_status[1] == 1)
		clean_flag_w = 1;
		
		if (xitong_status[2] == 1)
		quyang_flag_w = 1;
		
		if (xitong_status[3] == 1)
		zhangan_flag_w = 1;
		
		if (xitong_status[4] == 1)
		pingshan_flag_w = 1;
		
		if (xitong_status[5] == 1)
		tuye_flag_w = 1;
		
		
		//////////////ȷ�ϰ�ť�ر�΢������/////////
		if(confirm_status[0] == 1)
		{
			weitiao_w_status[0] = 0;		//��Ҫ����Y2��
	
			confirm_monitor_w_status[0] = 0;
		}
		
		
		if(confirm_status[1] == 1)
		{
			weitiao_w_status[1] = 0;

			confirm_monitor_w_status[1] = 0;
		}
		
		if(confirm_status[2] == 1)
		{
			weitiao_w_status[2] = 0;
			
			confirm_monitor_w_status[2] = 0;
		}
		
		if(confirm_status[3] == 1)
		{
			weitiao_w_status[3] = 0;

			confirm_monitor_w_status[3] = 0;
		}
		
		if(confirm_status[4] == 1)
		{
			weitiao_w_status[4] = 0;
		
			confirm_monitor_w_status[4] = 0;
		}
		
		if(confirm_status[5] == 1)
		{
			weitiao_w_status[5] = 0;
			
			confirm_monitor_w_status[5] = 0;

		}
		
////////////////////////////////////////////////////////////////////////
		
//////////////////���������־λ����///////////////////////////////
		if(work_zero_w_monitor_status == 1)
		{
			work_zero_w_monitor_status = 0;
			work_zero_w_finish = 1;
		}
//////////////////////////////////////////////////////////////////
		
///////////////W�� Ԥ���� λ�õ���֮��/////////
		if(work_pre_dianyang_w_monitor_status == 1)
		{
		   work_pre_dianyang_w_monitor_status = 0;
		}
				
		if(work_pre_dianyang_w_status == 1)
		{
			work_pre_dianyang_w_status = 0;
			work_pre_dianyang_w_finish = 1;
		}
		
/////////////////////////////////////////////////////////////////////
		
///////////////W�� պ�� λ�õ���֮��/////////
		if(work_zhangan_w_monitor_status == 1)
		{
		   work_zhangan_w_monitor_status = 0;
		}
				
		if(work_zhangan_w_status == 1)
		{
			work_zhangan_w_status = 0;
			work_zhangan_w_finish = 1;
		}
		
/////////////////////////////////////////////////////////////////////
		
///////////////W�� պ�� λ�û��㵽��֮��/////////
		if(work_zhangan_w_back_monitor_status == 1)
		{
		   work_zhangan_w_back_monitor_status = 0;
		}
				
		if(work_zhangan_w_back_status == 1)
		{
			work_zhangan_w_back_status = 0;
			work_zhangan_w_back_finish = 1;
		}
		
///////////////////�������W���ʼ�������//////////////////////////
		
		if(work_dianyang_w_s_monitor_status == 1)
		{
			work_dianyang_w_s_monitor_status = 0;
		}
		
		if(work_dianyang_w_s_status == 1)
		{
			work_dianyang_w_s_status = 0;
			work_dianyang_w_s_finish = 1;
			
		}

	///////////////////�������W_s���11 12���󵽴��//////////////////////////
		
		if(work_dianyang_w_s_lie_monitor_status == 1)
		{
			work_dianyang_w_s_lie_monitor_status = 0;
		}
		
		if(work_dianyang_w_s_lie_status == 1)
		{
			work_dianyang_w_s_lie_status = 0;
			work_dianyang_w_s_lie_finish = 1;
			
		}

	///////////////////�������W_m���󵽴��//////////////////////////
		
		if(work_dianyang_w_m_lie_monitor_status == 1)
		{
			work_dianyang_w_m_lie_monitor_status = 0;
		}
		
		if(work_dianyang_w_m_lie_status == 1)
		{
			work_dianyang_w_m_lie_status = 0;
			work_dianyang_w_m_lie_finish = 1;
			
		}

		///////////////////�������W_L���󵽴��//////////////////////////
		
		if(work_dianyang_w_l_lie_monitor_status == 1)
		{
			work_dianyang_w_l_lie_monitor_status = 0;
		}
		
		if(work_dianyang_w_l_lie_status == 1)
		{
			work_dianyang_w_l_lie_status = 0;
			work_dianyang_w_l_lie_finish = 1;
			
			tmp_dianyang_danyuan_hangshu ++;
			
			tmp_dianyang_jiance_hangshu = 1; //ת��һ����֮�����¿�ʼ��С��	
			
			work_dianyang_x_s_finish = 0;
			work_dianyang_w_s_lie_finish = 0;
			work_dianyang_x_s_hang_finish = 0; 
			work_dianyang_w_m_lie_finish = 0;
				
			work_dianyang_y_down_finish = 0;
			work_dianyanging_s_finish = 0;
			work_dianyang_y_up_finish = 0;
			work_dianyang_x_back_finish = 0;
			work_dianyang_x_l_finish = 0;
			work_dianyanging_m_finish = 0;
			work_dianyang_x_l_dianyang_status = 0;

			
		}
		
			///////////////////�������w����㵽���//////////////////////////
		
		if(work_dianyang_w_get_zero_monitor_status == 1)
		{
			work_dianyang_w_get_zero_monitor_status = 0;
		}
		
		if(work_dianyang_w_get_zero_status == 1)
		{
			work_dianyang_w_get_zero_status = 0;
			work_dianyang_w_get_zero_finish = 1;
			
			if (work_dianyang_x_get_zero_finish == 1 && work_dianyang_w_get_zero_finish == 1)// && tmp_dianyang_dianzhen_memory%2 == 0
			{
				start_b = 0;
				work_dianyang_allow = 0;
				
				tmp_dianyang_danyuan_hangshu = 1;
				tmp_dianyang_danyuan_lieshu = 1;
				
				tmp_dianyang_jiance_hangshu = 1;
				tmp_dianyang_jiance_lieshu = 1;
				
				// work_dianyang_w_s_finish = 0;
				// work_dianyang_x_s_finish = 0;
				
				 work_get_zero_finish();
			}
			
		}
		
		
	/////////////////////////���������̬������������/////////////////
	///////////////////������������w��s�����(��ʼ����//////////////////////////		
		if(work_dianyang_c_w_s_monitor_status == 1)
		{
			work_dianyang_c_w_s_monitor_status = 0;
			
			spotted_c_remember = Y3_pulse_remember;		//��ʼ��֮����Ҫ��λ�ý��м�¼
		}
		
		if(work_dianyang_c_w_s_status == 1)
		{
			work_dianyang_c_w_s_status = 0;
			work_dianyang_c_w_s_finish = 1;
		}
		
	///////////////////������������w����㵽���(��ʼ����//////////////////////////	
		if(work_dianyang_c_w_stop_status_monitor == 1)
		{
			work_dianyang_c_w_stop_status_monitor = 0;
		}
		
		if(work_dianyang_c_w_stop_status == 1)
		{
			work_dianyang_c_w_stop_status = 0;
			work_dianyang_c_w_stop_finish = 1;
			
			
			start_once = 0;
					
			work_dianyang_allow = 0;
					
			tmp_dianyang_danyuan_hangshu = 1;
			tmp_dianyang_danyuan_lieshu = 1;
					
			tmp_dianyang_jiance_hangshu = 1;
			tmp_dianyang_jiance_lieshu = 1;
					
			work_get_zero_finish_c();
			
		}
		
	
	
		OutPulse_Counter[3] = 0;	//��������Ҫ
		
	  	ClearPending((U32)BIT_TIMER3);
		return;
	}
  	ClearPending((U32)BIT_TIMER3);

}
///////////////////////////////////////////////////////////////////////////
////	Y3 �����������//////////////////////////////
////	 ÿ����һ������ PulseOut_3_Start ֻ������һ��//////
////	frequence: ����Ƶ��///////////////////////////////
////	pulse_PreSet: ���������� ///////////////////////////
///////////////////////////////////////////////////////////////////////////
void PulseOut_3_Start(unsigned int frequence, int pulse_PreSet)
{
	DWORD tmp;
	
	if(w_dingchang_pb == 1 || w_dingchang_nb == 1)
	{
		frequence_Y3 = frequence;
		pulse_PreSet_Y3 = pulse_PreSet;
	}
	
	if(0 == pulse_PreSet)
	{
		PulseOut_3_Stop();
		
		if(work_zero_w_status == 1 && work_zero_w_monitor_status == 1)
		{
			work_zero_w_finish = 1;
			work_zero_w_monitor_status = 0;
		}
		
		return;
	}
	else
	{
		OutPulse_PreSet[3] = pulse_PreSet;
		//OutPulse_Counter[3] = 0;	// ȷ��ÿ������PulseOut_3_Start ������������ pulse_PreSet
	}
	
	if(0 == frequence)
	{
		frequence = 1;
		PulseOut_3_Stop();
		return;
	}
	
	rTCNTB3= 300300/frequence;	// // Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
	rTCMPB3 = rTCNTB3/2;
	
	rSRCPND1 = rSRCPND1 | ((U32)0x1<<13);   //��ն�ʱ��3Դ����
	rINTPND1 = rINTPND1 | ((U32)0x1<<13);    //��ն�ʱ��3�ж�����
  
	
	rINTMSK1 &=~(BIT_TIMER3);
	tmp = rTCON & (~(0xf << 16));	// dead zone Disable
	rTCON = tmp | (2 << 16);		//update TCVNTB0, stop				
	rTCON = tmp | (9 << 16);		// interval mode,  start
}

///////////////////////////////////////////////////////////////////////////
////	Y3 ���巢��ֹͣ////////
///////////////////////////////////////////////////////////////////////////
void PulseOut_3_Stop(void)
{
	//DWORD tmp;

	Set_Y_Value(3,0);
	
	rINTMSK1 |= BIT_TIMER3;
	rTCON &= ~(1 << 16);		// Timer3, stop		

	//OutPulse_Counter[2] = 0;
	
	
///////////////////������������w�������˶�ֹͣ��//////////////////////////				
		if(work_dianyang_c_w_start_monitor_status == 1)
		{
			work_dianyang_c_w_start_monitor_status = 0;
		}
	
		if(work_dianyang_c_w_start_status == 1)
		{
			work_dianyang_c_w_start_status = 0;
			work_dianyang_c_w_start_finish = 1;
		}
	
}


/////////////////////////////////////////////////
//////	�������������ʼ��     //////////////
//////	OUT 1~3   3·������� //////////////
////////////////////////////////////////////////
void Pulse_Out_Init()	
{
	DWORD tmp;
	
    //set GPB1 2 3 as TOUT
	// tmp = rGPBCON & (~(0x3<< 2)) & (~(0x3<< 4))& (~(0x3<< 6));
	// rGPBCON = tmp | (0x2<<2) | (0x2<<4) | (0x2<<6);		
	
	//set GPB1 2 3 as ��ͨ���Output
	tmp = rGPBCON & (~(0x3<< 2)) & (~(0x3<< 4))& (~(0x3<< 6));
	rGPBCON = tmp | (0x1<<2) | (0x1<<4) | (0x1<<6);	

	
	    
	
	// Timer1 Initialize	----HS Out1
	pISR_TIMER1 = (int)PulseOut_1_Process;	// Timer ISR for HS Out1
	rTCFG0 &= ~(0xff << 0); 
	rTCFG0 |= (110 << 0); 	// Dead zone=0, Prescaler0=10(0xff)
	rTCFG1 &= ~(0xf << 4); 
	rTCFG1 |= (0x0 << 4); 	// Interrupt, Mux0=1/2
	//rTCNTB1 = 30;    //30.27273	// Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}	100K
	//rTCMPB1 = 15;
	
	 
	
	
	PulseOut_1_Stop();
	

	
	
	// Timer2 Initialize	----HS Out2
	pISR_TIMER2 = (int)PulseOut_2_Process;	// Timer ISR for HS Out2
	rTCFG0 &= ~(0xff << 8); 
	rTCFG0 |= (110 << 8); 	// Dead zone=0, Prescaler0=110(0xff)   ע��ע��ע�⣡�������ܱ� Timer 2 3 4 ����
	rTCFG1 &= ~(0xf << 8); 
	rTCFG1 |= (0x0 << 8); 	// Interrupt, Mux0=1/2
	
	
 	
	PulseOut_2_Stop(); 
	
	
	
	
	

	// Timer3 Initialize	----HS Out3
 	pISR_TIMER3 = (int)PulseOut_3_Process;	// Timer ISR for HS Out3
	rTCFG0 &= ~(0xff << 8); 
	rTCFG0 |= (110 << 8); 	// Dead zone=0, Prescaler0=110(0xff)   ע��ע��ע�⣡�������ܱ� Timer 2 3 4 ����
	rTCFG1 &= ~(0xf << 12); 
 	rTCFG1 |= (0x0 << 12); 	// Interrupt, Mux0=1/2
 	
 	PulseOut_3_Stop();		//�˴�����pulseout_3

}
 

//////////////////////////////////////////////////////////////////////
////	DA ����жϷ������///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void __irq DA0_10_Process(void) // ��ͨ��ʾ����ȷ��Ƶ��
{
	if(0xffff == (g_InteralMemory.Word[32] & 0xffff))
	{
		g_InteralMemory.Word[32] = 0;
		g_InteralMemory.Word[33]++;
	}
	else
		g_InteralMemory.Word[32]++;

	ClearPending((U32)BIT_TIMER3);
}


//////////////////////////////////////////////////////////////////////
////	DA �������(ֻ������һ��)//////////////////////////
////	voltage: �����ѹ(0.000~10.000V )  С�����3 λ//////////
//////////////////////////////////////////////////////////////////////
void DA0_10_Start(float voltage)
{
	DWORD tmp;

	if(voltage>10*1000)
		voltage = 10*1000;
	else if(voltage<0)
		voltage = 0;
	
	rTCNTB3= 300;	
	//rTCMPB3 = (rTCNTB3*g_InteralMemory.Word[30])/(10000*1.326);// ���10V , С�����3 λ10*1000
	rTCMPB3 = (rTCNTB3*voltage)/(10*1000*1.326);// ���10V , С�����3 λ10*1000
	if(rTCMPB3 == rTCNTB3)
		rTCMPB3 -= 1;
	
	tmp = rGPBCON & (~(0x3<< 6));
	rGPBCON = tmp | (0x2<<6);	//set GPB3 as TOUT	

	rINTMSK1 &=~(BIT_TIMER3);
	tmp = rTCON & (~(0xf << 16));	// dead zone Disable
	rTCON = tmp | (2 << 16);		/* update TCVNTB3, stop					*/
	rTCON = tmp | (9 << 16);		/* interval mode,  start				*/	
}

//////////////////////////////////////////////////////////////////////
////	DA ���ֹͣ//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void DA0_10_Stop(void)
{
	DWORD tmp;

	rTCNTB3= 300;	
	rINTMSK1 |= BIT_TIMER3;
	rTCON &= ~(1 << 16);			/* Timer3, stop							*/

	tmp = rGPBCON & (~(0x3<< 6));
	rGPBCON = tmp | (1<<6);	//set GPB3 as OUT	
	rGPBDAT &= ~(1<<3);
}


//////////////////////////////////////////////////////////////////////
////	DA �����ʼ��///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void DA0_10_Init(void)
{
	DWORD tmp;

	tmp = rGPBCON & (~(0x3<< 6));
	rGPBCON = tmp | (0x2<<6);	//set GPB3 as TOUT	

	// Timer3 Initialize	----DA
	pISR_TIMER3 = (int)DA0_10_Process;	// Timer ISR for DA
	rTCFG0 &= ~(0xff << 8); 
	rTCFG0 |= (110 << 8); 	// Dead zone=0, Prescaler0=110(0xff)   Timer 2 3 4 ����
	rTCFG1 &= ~(0xf << 12); 
	rTCFG1 |= (0x0 << 12); 	// Interrupt, Mux0=1/2
	rTCNTB3 = 300;    			// Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}	1K
	rTCMPB3 = 150;
	DA0_10_Stop();
}



/////////////////////////////////////////////////
//////	��ͨ�����ʼ��     //////////////
//////	IN7~IN15 ��9 ·		//////////////
////////////////////////////////////////////////
void X_In_Init(void)	
{
	DWORD tmp;

	tmp = rGPFCON & (~(0x3<< 14));
	rGPFCON = tmp |(0x0<<14);			//set GPF7 as input
	//////////////
	tmp = rGPGCON & (~(0x3<< 0)) & (~(0x3<< 2)) & (~(0x3<< 4)) & (~(0x3<< 6)) & (~(0x3<< 8))
					 & (~(0x3<< 10)) & (~(0x3<< 12)) & (~(0x3<< 14));
	
	rGPGCON = tmp | (0x0<<0) | (0x0<<2) | (0x0<<4) | (0x0<<6) | (0x0<<8) | (0x0<<10) | (0x0<<12) | (0x0<<14);		//set GPF5 7 as input	
	//////////	
}



/////////////////////////////////////////////////
//////	��ͨ�����ʼ��     //////////////
//////	Y2~Y15 ��14 ·	 //////////////
////////////////////////////////////////////////
void Y_Out_Init(void)		
{
	DWORD tmp;

	tmp = rGPECON & (~(0x3<< 0)) & (~(0x3<< 2)) & (~(0x3<< 4)) & (~(0x3<< 6)) & (~(0x3<< 8)) & (~(0x3<< 10)) & (~(0x3<< 12)) & (~(0x3<< 14));
	
	
	rGPECON = tmp | (0x1<<0) | (0x1<<2) | (0x1<<4) | (0x1<<6) | (0x1<<8) | (0x1<<10) | (0x1<<12) | (0x1<<14);	//set GPB4 5 9 10 as output	
	// �������������
	rGPEDAT |= (1<<0);	// OUT4		
	rGPEDAT |= (1<<1);	// OUT5		
	rGPEDAT |= (1<<2);	// OUT6		
	rGPEDAT |= (1<<3);	// OUT7		
	rGPEDAT |= (1<<4);	// OUT8		
	rGPEDAT |= (1<<5);	// OUT9		
	rGPEDAT |= (1<<6);	// OUT10	
	rGPEDAT |= (1<<7);	// OUT11	


}





////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
////	����   IN7-IN15//////////////////////
////	����: �˿�״̬
////////////////////////////////////////////////////////////////
unsigned char Get_X_Value(unsigned char X_num)
{
	unsigned char X_value=0xff;

	switch(X_num)
	{	
		case 1:
			X_value = (rGPFDAT >> 1) & 0x1;	// X1
			break;
			
		case 2:
			X_value = (rGPFDAT >> 2) & 0x1;	// X2
			break;
			
		case 3:
			X_value = (rGPFDAT >> 3) & 0x1;	// X3
			break;
			
		case 4:
			X_value = (rGPFDAT >> 4) & 0x1;	// X4
			break;
				
		case 5:
			X_value = (rGPFDAT >> 5) & 0x1;	// X5
			break;
				
		case 6:
			X_value = (rGPFDAT >> 6) & 0x1;	// X6
			break;

		case 7:
			X_value = (rGPFDAT >> 7) & 0x1;	// X7
			break;
		case 8:
			X_value = (rGPGDAT >> 0) & 0x1;	// X8	
			break;
		case 9:
			X_value = (rGPGDAT >> 1) & 0x1;	// X9
			break;
		case 10:
			X_value = (rGPGDAT >> 2) & 0x1;	// X10
			break;
		case 11:
			X_value = (rGPGDAT >> 3) & 0x1;	// X11
			break;
		case 12:
			X_value = (rGPGDAT >> 4) & 0x1;	// X12
			break;
		case 13:
			X_value = (rGPGDAT >> 5) & 0x1;	// X13
			break;
		case 14:
			X_value = (rGPGDAT >> 6) & 0x1;	// X14
			break;
		case 15:
			X_value = (rGPGDAT >> 7) & 0x1;	// X15
			break;
	}

	return X_value;// xuzhiqin tixing xiugai
}


////////////////////////////////////////////////////////////////
////	���   Y4~Y15    /////////////////////////
////	Y_num:4~15		Value:   0��1   //////  ����y1 �� y2 
////////////////////////////////////////////////////////////////
void Set_Y_Value(unsigned char Y_num, unsigned char Value)
{
	Value = Value;// xuzhiqin tixing xiugai
	
	switch(Y_num)
	{
	case 1:
		(Value) ? (rGPBDAT |= (1<<1)) : (rGPBDAT &= ~(1<<1));	// Y1	�����������ģʽ
		break;
	case 2:
		(Value) ? (rGPBDAT |= (1<<2)) : (rGPBDAT &= ~(1<<2));	// Y2	�����������ģʽ
		break;
	case 3:
		(Value) ? (rGPBDAT |= (1<<3)) : (rGPBDAT &= ~(1<<3));	// Y3	�����������ģʽ
		break;
	case 4:
		(Value) ? (rGPEDAT |= (1<<0)) : (rGPEDAT &= ~(1<<0));	// Y4	
		break;
	case 5:
		(Value) ? (rGPEDAT |= (1<<1)) : (rGPEDAT &= ~(1<<1));	// Y5	
		break;
	case 6:
		(Value) ? (rGPEDAT |= (1<<2)) : (rGPEDAT &= ~(1<<2));	// Y6	
		break;
	case 7:
		(Value) ? (rGPEDAT |= (1<<3)) : (rGPEDAT &= ~(1<<3));	// Y7	
		break;
	case 8:
		(Value) ? (rGPEDAT |= (1<<4)) : (rGPEDAT &= ~(1<<4));	// Y8	
		break;
	case 9:
		(Value) ? (rGPEDAT |= (1<<5)) : (rGPEDAT &= ~(1<<5));	// Y9	
		break;
	case 10:
		(Value) ? (rGPEDAT |= (1<<6)) : (rGPEDAT &= ~(1<<6));	// Y10	
		break;
	case 11:
		(Value) ? (rGPEDAT |= (1<<7)) : (rGPEDAT &= ~(1<<7));	// Y11	
		break;
	}
}





