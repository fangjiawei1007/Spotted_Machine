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
#include "..\inc\wdt.h"	//WDT���� by ����ΰ 2017/4/1


#include "..\inc\app.h"
#include "..\inc\appBase.h"

#include "includes.h"



//extern INTERALMEMORY g_InteralMemory;

//extern GLOBALVAR g_GlobalVar;

//extern INT16U PreScrNum;


extern unsigned int InPulseCount[7];		//��¼6·�����������������

extern unsigned int OutPulse_PreSet[4];	// 3·���������  ����Ԥ��ֵ
extern volatile unsigned int OutPulse_Counter[4];	// 3·���������  ��ǰ���������

extern INTERALMEMORY g_InteralMemory;




////�Ӵ˿�ʼ����û�����//////////////////////////////////

//unsigned char model_work;
	unsigned int move_allow = 0;
	
	unsigned int start_mode = 0;
	unsigned int power_flag = 0;//�����ϵ�ʱ��ѡ��ģ��
	
 	unsigned int x_diandong_flag = 0;
	unsigned int y_diandong_flag = 0;
	unsigned int w_diandong_flag = 0;
	
	unsigned int x_dingchang_flag = 0;
	unsigned int y_dingchang_flag = 0;
	unsigned int w_dingchang_flag = 0;
	
	unsigned int weitiao_flag = 0;
	unsigned int mode_flag = 0;
	unsigned int xitong_flag = 0;
	
	unsigned int gongzuo_flag = 0;
	unsigned int clean_flag = 0;
	unsigned int quyang_flag = 0;
	unsigned int zhangan_flag = 0;
	unsigned int pingshan_flag = 0;
	unsigned int tuye_flag = 0;
	
	
	unsigned int status_record1 = 0;//�����ж���������
	unsigned int status_record2 = 0;//�����ж���������
	
	unsigned int null = 0;
	static unsigned int condition_flag = 0;//condition_flag�����жϵ㶯ͬʱͣ����������,condition_flag == 0��ʱ����ܹ���������
///////////////////////////////////////////////////////////////
//////	��ʼ��     //////////////////////////////////////////
//////	ÿ���ϵ�App_Init ִֻ��һ��	 //////////////////
//////////////////////////////////////////////////////////////
void App_Init(void)
{
	int i;
	
	X_In_Init();
	Y_Out_Init();
	Pulse_In_Init();
	Pulse_Out_Init();
	parameter_init();
	xitong_position_init();
	wdt_feed_dog();

	//������������
	start_b = 0;
	start_once = 0;
	
	
	//test:�ϵ�֮������������Ϊ0 �����Ҫ����
	if(power_zero == 1)
	{
		tmp_dianyang_danyuan_lieshu = 1;
		tmp_dianyang_danyuan_hangshu = 1;
		
		tmp_dianyang_jiance_lieshu = 1;
		tmp_dianyang_jiance_hangshu = 1;
		
		tmp_dianyang_dianzhen_memory = 1;
			
		tmp_dianyang_dianzhen_no = dianzhen_bianhao;
		tmp_dianyang_yangpin_no = yangpin_bianhao;
		
		Y1_dianyang_x_back_remember = 0;
		Y1_pulse_remember_back_add = 0;
		
		spotted_c_remember = 0;
		tmp_dianyang_times_remember = 1;
		tmp_dianyang_times = 0;
		
		tmp_dianyang_c_x_s_hang_remember = 0;
		
		// Y1_pulse_remember = 0;
		// Y1_pulse_remember_add = 0;
		
		// Y2_pulse_remember = 0;
		// Y2_pulse_remember_add = 0;
		
		// Y3_pulse_remember = 0;
		// Y3_pulse_remember_add = 0;
		
		// Y1_dianyang_x_back_remember = 0;
		
		// x_biaoding_pulse = 1;
		// x_biaoding_length = 1;
		
		// y_biaoding_pulse = 1;
		// y_biaoding_length = 1;
		
		// w_biaoding_pulse = 1;
		// w_biaoding_length = 1;
		
		// monitor_x_biaoding = 0;
		// monitor_y_biaoding = 0;
		// monitor_w_biaoding = 0;
	}
	
	monitor_yangpin_no = 0;
	//	DA0_10_Init();

	// ���������ʼ��
	for(i=0; i<7; i++)
	{
		InPulseCount[i] = 0;
	}
	//parameter_input();
	
}


///////////////////////////////////////////////////////////////
//////	Ӧ��������     //////////////////////////////////////////
//////	ϵͳÿ��ѭ�������һ��App_Operation	 //////////////////
//////////////////////////////////////////////////////////////
void App_Operation(void)
{

//////********************************************////



//////////////////////////////////////////////////////////////
/////////////////////�Ӵ˿�ʼ����û�����/////////////////////
//////////////////////////////////////////////////////////////

	
//ι��
	wdt_feed_dog();//WDT���� by����ΰ
	
/*
g_InteralMemory.Bit[]
g_InteralMemory.KeepBit[]
g_InteralMemory.Word[]
g_InteralMemory.KeepWord[]
*/	
	if(dry == 1)
	{
		Y1_pulse_remember = 0;
		Y1_pulse_remember_add = 0;
		
		Y2_pulse_remember = 0;
		Y2_pulse_remember_add = 0;
		
		Y3_pulse_remember = 0;
		Y3_pulse_remember_add = 0;
		
		Y1_pulse_remember_back_add = 0;
	}
	pulse_cal();
	biaoding();
	//*********************���**********************************//
	monitor_x_biaoding = (Y1_PULSE_TOTAL/10);//Y1_pulse_remember_add
	monitor_y_biaoding = (Y2_PULSE_TOTAL/10);
	monitor_w_biaoding = (Y3_PULSE_TOTAL/10);
	
	monitor_x = ((x_biaoding_pulse*Y1_PULSE_TOTAL*10.0)/x_biaoding_length*100000);// /10 Y1_pulse_remember_add,���һ�����10��umת0.1um�ľ���ת�����ֵ
	monitor_y = ((((Y2_PULSE_TOTAL/10)*y_biaoding_length)/(y_biaoding_pulse)));// /10Y2_BIAODING_PULSE_TOTAL
	monitor_w = ((w_biaoding_pulse*Y1_PULSE_TOTAL*10.0)/w_biaoding_length*100000);// /10
	
	/* if(Y3_pulse_remember == 5000)
	{
		Y3_pulse_remember = 0;
	} */
	
	monitor_jiance_hang = tmp_dianyang_jiance_hangshu;
	monitor_jiance_lie = tmp_dianyang_jiance_lieshu;
	
	monitor_danyuan_hang = tmp_dianyang_danyuan_hangshu;
	monitor_danyuan_lie = tmp_dianyang_danyuan_lieshu;
	
	monitor_dianzhen_no = dianzhen_bianhao;
	//monitor_yangpin_no = tmp_dianyang_dianzhen_memory;	// tmp_dianyang_yangpin_no
	
	
	c_s_distance = dianzhen_hangjianju;
	c_m_distance = (jiance_hangjianju +jiance_gaodu);
	c_l_distance = (jiance_hangshu * c_m_distance + danyuan_hangjianju);
	//**********************��������****************************//
	
	button_huchi(&start_b,&start_once,&start_mode,null);
	button_huchi(&power_previous,&power_zero,&power_flag,null);//�ϵ�ѡ��ģʽ
	
	button_huchi(&diandong_x_p,&diandong_x_n,&x_diandong_flag,condition_flag);//�˴�condition_flag û����ָ�����ͣ����Ե���һ��ʼ�㶯ʧ��
	button_huchi(&diandong_y_p,&diandong_y_n,&y_diandong_flag,condition_flag);//���������ʱ����Խ��н�һ�����ԣ������Ĳα���������
	button_huchi(&diandong_w_p,&diandong_w_n,&w_diandong_flag,condition_flag);//�����Ŀ����͸��ƣ�ֻ���ڵ�ַ��������ͬʱ���ܸı������ֵ�ı仯
	
	button_huchi(&x_dingchang_pb,&x_dingchang_nb,&x_dingchang_flag,null);
	button_huchi(&y_dingchang_pb,&y_dingchang_nb,&y_dingchang_flag,null);
	button_huchi(&w_dingchang_pb,&w_dingchang_nb,&w_dingchang_flag,null);
	
	button_huchi(&weitiao_p,&weitiao_n,&weitiao_flag,null);//΢������
	
	button_3_huchi(&xitong_x,&xitong_y,&xitong_w,&status_record1,&status_record2);//������������
	/* button_huchi(&xitong_x,&xitong_y,&xitong_flag,null);
	button_huchi(&xitong_y,&xitong_w,&xitong_flag,null);
	button_huchi(&xitong_x,&xitong_w,&xitong_flag,null);
	 */
	button_huchi(&gongzuo_b,&gongzuo_c,&gongzuo_flag,null);
	button_huchi(&clean_b,&clean_c,&clean_flag,null);
	button_huchi(&quyang_b,&quyang_c,&quyang_flag,null);
	button_huchi(&zhangan_b,&zhangan_c,&zhangan_flag,null);
	button_huchi(&pingshan_b,&pingshan_c,&pingshan_flag,null);
	button_huchi(&tuye_b,&tuye_c,&tuye_flag,null);
	
	button_huchi(&single_mode,&multi_mode,&mode_flag,null);//�������ģʽѡ��
	
	
	//�㶯
	/* fn_jog(Y_1,x_diandong_n,x_diandong_speed_t,condition_flag);//Y_1 ��ֵ5 ���dir
	fn_jog(Y_1,x_diandong_n,x_diandong_speed_t,condition_flag);
	
	fn_jog(Y_2,y_diandong_p,y_diandong_speed_t,condition_flag);
	fn_jog(Y_2,y_diandong_n,y_diandong_speed_t,condition_flag);
	
	fn_jog(Y_3,w_diandong_p,w_diandong_speed_t,condition_flag);
	fn_jog(Y_3,w_diandong_n,w_diandong_speed_t,condition_flag); */
	
	//TEST
	//PulseOut_1_Start(2000,-1);
	
	//ȷ�ϰ�ť����
/* 	
	if(gongzuo_c == 1 || clean_c == 1 || quyang_c == 1 || zhangan_c == 1 || pingshan_c == 1 || tuye_c == 1)
	{
		Beep(1);
	} */
	//*****�ֶ�����*****//
	//*****�㶯*****//
	fn_jog_test();
	
	//*****����******//
	fn_dingchang();
	
	
	//*****����******//
	fn_get_zero();//���㺯�����⣺�жϺ����е� Y1_pulse_rememberֵ�޷�����
	
	//************************************//
	
	
	//****ϵͳλ������*****//
	//********΢��*********//
	fn_weitiao();
	
	//****************//
	
	fn_pos_init();
	
	//fn_position();//���ֳ�����bug��ֻ�ܵ�һ�����������û�а취�ƶ�
	fn_gongzuo_position();
	fn_gongzuo_confirm();
	
	
	fn_clean_position();
	fn_clean_confirm();
	
	
	fn_quyang_position();
	fn_quyang_confirm();
	
	
	fn_zhangan_position();
	fn_zhangan_confirm();
	
	
	fn_pingshan_position();
	fn_pingshan_confirm();
	
	
	fn_tuye_position();
	fn_tuye_confirm();
	
	//ϵͳλ�ð�������������������3��flag
	
	
	//ֻ������λ����Ϣ��ȷ����֮�󣬲��ܹ����е���
	move_allow = (gongzuo_c && clean_c && quyang_c && zhangan_c && pingshan_c && tuye_c);	

	//����һ����̬�Ʊ����������㴦��	
	if(start_b == 1 && move_allow == 1 && power_zero == 1 )//&& start_b_status == 0
	{
		start_b_status = 1;
		
		//Beep(1);
		//�ϵ����
		work_get_zero();
		
		
		//�����ϵ�1st������ϴ����
		if(work_zero_x_finish == 1 && work_zero_y_finish == 1 && work_zero_w_finish == 1 && work_clean_start_status == 0)
		{
			work_clean();
		}
		
		if(work_clean_x_finish == 1 && work_clean_y_finish == 1 && work_cleaning_finish == 1 && work_clean_back_finish == 1)
		{
			work_clean_x_finish = 0;
			work_clean_y_finish = 0;
			work_cleaning_finish = 0;
			work_clean_back_finish = 0;
			
			work_clean_start_status = 1;	//�����������֮�󽫴���Ϊ1
			
			quyang_allow = 1;
		}
		
		
		//clean������ɺ󣬽���ȡ������
		if(quyang_allow == 1 && quyang_status == 0 && work_quyang_start_status == 0)//ȡ������
		{
			work_quyang();
		}
		
		if(work_quyang_x_finish == 1 && work_quyang_y_finish == 1 && work_quyanging_finish == 1 && work_quyang_back_finish == 1)
		{
			quyang_allow = 0;
			
			work_quyang_x_finish = 0;
			work_quyang_y_finish = 0;
			work_quyanging_finish = 0;
			work_quyang_back_finish = 0;
			
			work_quyang_start_status = 1;	//�����������֮�󽫴���Ϊ0,����ѭ��
			
			pre_dianyang_allow = 1;
			
		}
		
		//ȡ��������ɺ����Ԥ��������
	 	if(pre_dianyang_allow == 1 && pre_dianyang_status == 0 && work_pre_dianyang_start_status == 0)
		{
			work_pre_dianyang();
		}
		
		if(work_pre_dianyang_x_finish == 1 && work_pre_dianyang_y_finish == 1 && work_pre_dianyanging_finish == 1 
		   && work_pre_dianyang_w_finish == 1 && work_pre_dianyang_back_finish == 1)//
		{
			pre_dianyang_allow = 0;
			
			work_pre_dianyang_x_finish = 0;
			work_pre_dianyang_y_finish = 0;
			
			work_pre_dianyanging_finish = 0;
			
			work_pre_dianyang_w_finish = 0;
			work_pre_dianyang_back_finish = 0;
			
			work_pre_dianyang_start_status = 1;	//�����������֮�󽫴���Ϊ0,����ѭ��
			
			zhangan_allow = 1;
		} 
		
		//Ԥ����������ɺ����պ�ɹ���
	 	
 		if(zhangan_allow == 1 && zhangan_status == 0 && work_zhangan_start_status == 0)
		{
			work_zhangan();
			
			
		}
		
		 if(work_zhangan_x_finish == 1 && work_zhangan_y_finish == 1 && work_zhanganing_finish == 1 
		   && work_zhangan_w_finish == 1 && work_zhangan_back_finish == 1 && work_zhangan_x_back_finish == 1)//
		{
			zhangan_allow = 0;
			
			work_zhangan_x_finish = 0;
			work_zhangan_y_finish = 0;
			
			work_zhanganing_finish = 0;
			
			work_zhangan_w_finish = 0;
			work_zhangan_back_finish = 0;
			
			work_zhangan_start_status = 1;	//�����������֮�󽫴���Ϊ0,����ѭ��
			
			dianyang_allow = 1;
		}   
		
		if(dianyang_allow == 1 && dianyang_status == 0 && work_dianyang_start_status == 0)
		{
			work_dianyang();
			//Beep(1);
		}
		
		
		
	}
	
	
	else if(start_b == 0 && start_b_status == 1)
	{
 		
		start_b_status = 0;
		
		if (work_zero_x_status == 1 || work_zero_y_status == 1 || work_zero_w_status == 1)
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
		/* 	work_zero_x_status = 0;
			work_zero_y_status = 0;
			work_zero_w_status = 0; */
			
		}
		
		
		if (work_clean_x_status == 1 || work_clean_y_status == 1 || work_clean_back_status == 1)
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
			work_clean_x_status = 0;
			work_clean_y_status = 0;
			work_clean_back_status = 0;	

		} 
		
		
		if (work_quyang_x_status == 1 || work_quyang_y_status == 1 || work_quyang_back_status == 1)
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
			work_quyang_x_status = 0;
			work_quyang_y_status = 0;
			work_quyang_back_status = 0;	

		} 
		
		if (work_pre_dianyang_x_status == 1 || work_pre_dianyang_y_status == 1 || work_pre_dianyang_back_status == 1 
		   || work_pre_dianyang_w_status == 1)
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
			work_pre_dianyang_x_status = 0;
			work_pre_dianyang_y_status = 0;
			work_pre_dianyang_w_status = 0;
			
			work_pre_dianyang_back_status = 0;	

		}
		
		if(work_zhangan_x_status == 1 || work_zhangan_w_status == 1 || work_zhangan_y_status == 1  
		|| work_zhangan_back_status == 1 || work_zhangan_w_back_status == 1 || work_zhangan_x_back_status == 1)//|| work_zhanganing_status == 1
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
			work_zhangan_x_status = 0;
			work_zhangan_w_status = 0;
			work_zhangan_y_status = 0;
			
			work_zhanganing_status = 0;
			work_zhangan_back_status = 0;
			work_zhangan_w_back_status = 0;
			work_zhangan_x_back_status = 0;
		
		}
		
		if(work_dianyang_w_l_lie_status == 1 || work_dianyang_w_s_status == 1 || work_dianyang_x_s_status == 1
		  || work_dianyang_w_s_lie_status == 1 || work_dianyang_x_s_hang_status == 1 || work_dianyang_w_m_lie_status == 1 
		  || work_dianyang_y_down_status == 1 || work_dianyanging_s_status == 1 || work_dianyang_x_m_status == 1 
		  || work_dianyang_x_l_status == 1 || work_dianyang_y_up_status == 1 || work_dianyang_x_back_status == 1 
		  || work_dianyang_x_get_zero_status == 1 || work_dianyang_w_get_zero_status == 1)
		  {
				PulseOut_1_Stop();
				PulseOut_2_Stop();
				PulseOut_3_Stop();
			  
				work_dianyang_w_l_lie_status = 0;
				work_dianyang_w_s_status = 0;
				work_dianyang_x_s_status = 0;
				work_dianyang_w_s_lie_status = 0;
				work_dianyang_x_s_hang_status = 0;
				work_dianyang_w_m_lie_status = 0;

				work_dianyang_y_down_status = 0;
				work_dianyanging_s_status = 0;
				work_dianyang_x_m_status = 0;
				work_dianyang_x_l_status= 0;

				work_dianyang_y_up_status = 0;

				work_dianyang_x_back_status = 0;
				
				work_dianyang_x_get_zero_status = 0;
				work_dianyang_w_get_zero_status = 0;
				
		  }
		
	}
	
	//����һ����̬�Ʊ������������㴦��		
	if(start_b == 1 && move_allow == 1 && power_previous == 1 && start_b_status == 0)
		{
			start_b_status = 1;
			
			
		}



	//����������̬�Ʊ����������㴦��
	if(start_once == 1 && move_allow == 1 && power_zero == 1 )//&& start_b_status == 0
	{
		start_once_status = 1;
		
		//�ϵ����
		work_get_zero();
		
		
		//�����ϵ�1st������ϴ����
		if(work_zero_x_finish == 1 && work_zero_y_finish == 1 && work_zero_w_finish == 1 && work_clean_start_status == 0)
		{
			work_clean();
		}
		
		if(work_clean_x_finish == 1 && work_clean_y_finish == 1 && work_cleaning_finish == 1 && work_clean_back_finish == 1)
		{
			work_clean_x_finish = 0;
			work_clean_y_finish = 0;
			work_cleaning_finish = 0;
			work_clean_back_finish = 0;
			
			work_clean_start_status = 1;	//�����������֮�󽫴���Ϊ1
			
			quyang_allow = 1;
		}
		
		
		//clean������ɺ󣬽���ȡ������
		if(quyang_allow == 1 && quyang_status == 0 && work_quyang_start_status == 0)//ȡ������
		{
			work_quyang();
		}
		
		if(work_quyang_x_finish == 1 && work_quyang_y_finish == 1 && work_quyanging_finish == 1 && work_quyang_back_finish == 1)
		{
			quyang_allow = 0;
			
			work_quyang_x_finish = 0;
			work_quyang_y_finish = 0;
			work_quyanging_finish = 0;
			work_quyang_back_finish = 0;
			
			work_quyang_start_status = 1;	//�����������֮�󽫴���Ϊ0,����ѭ��
			
			pre_dianyang_allow = 1;
			
		}
		
		//ȡ��������ɺ����Ԥ��������
	 	if(pre_dianyang_allow == 1 && pre_dianyang_status == 0 && work_pre_dianyang_start_status == 0)
		{
			work_pre_dianyang();
		}
		
		if(work_pre_dianyang_x_finish == 1 && work_pre_dianyang_y_finish == 1 && work_pre_dianyanging_finish == 1 
		   && work_pre_dianyang_w_finish == 1 && work_pre_dianyang_back_finish == 1)//
		{
			pre_dianyang_allow = 0;
			
			work_pre_dianyang_x_finish = 0;
			work_pre_dianyang_y_finish = 0;
			
			work_pre_dianyanging_finish = 0;
			
			work_pre_dianyang_w_finish = 0;
			work_pre_dianyang_back_finish = 0;
			
			work_pre_dianyang_start_status = 1;	//�����������֮�󽫴���Ϊ0,����ѭ��
			
			zhangan_allow = 1;
		} 
		
		//Ԥ����������ɺ����պ�ɹ���
	 	
 		if(zhangan_allow == 1 && zhangan_status == 0 && work_zhangan_start_status == 0)
		{
			work_zhangan();
			
			
		}
		
		 if(work_zhangan_x_finish == 1 && work_zhangan_y_finish == 1 && work_zhanganing_finish == 1 
		   && work_zhangan_w_finish == 1 && work_zhangan_back_finish == 1 && work_zhangan_x_back_finish == 1)//
		{
			zhangan_allow = 0;
			
			work_zhangan_x_finish = 0;
			work_zhangan_y_finish = 0;
			
			work_zhanganing_finish = 0;
			
			work_zhangan_w_finish = 0;
			work_zhangan_back_finish = 0;
			
			work_zhangan_start_status = 1;	//�����������֮�󽫴���Ϊ0,����ѭ��
			
			dianyang_allow = 1;
		}   
		
		
		if(dianyang_allow == 1 && dianyang_c_status == 0 )//&& work_dianyang_c_start_status == 0
		{
			
			work_dianyang_continuous();
			
		}
		
		
		
	}
	
	
	else if(start_once == 0 && start_once_status == 1)
	{
 		
		start_once_status = 0;
		
		if (work_zero_x_status == 1 || work_zero_y_status == 1 || work_zero_w_status == 1)
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
		/* 	work_zero_x_status = 0;
			work_zero_y_status = 0;
			work_zero_w_status = 0; */
			
		}
		
		
		if (work_clean_x_status == 1 || work_clean_y_status == 1 || work_clean_back_status == 1)
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
			work_clean_x_status = 0;
			work_clean_y_status = 0;
			work_clean_back_status = 0;	

		} 
		
		
		//ȡ��λ�ð�ͣ
		if (work_quyang_x_status == 1 || work_quyang_y_status == 1 || work_quyang_back_status == 1)
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
			work_quyang_x_status = 0;
			work_quyang_y_status = 0;
			work_quyang_back_status = 0;	

		} 
		
		
		//Ԥ����λ�ð�ͣ
		if (work_pre_dianyang_x_status == 1 || work_pre_dianyang_y_status == 1 || work_pre_dianyang_back_status == 1 
		   || work_pre_dianyang_w_status == 1)
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
			work_pre_dianyang_x_status = 0;
			work_pre_dianyang_y_status = 0;
			work_pre_dianyang_w_status = 0;
			
			work_pre_dianyang_back_status = 0;	

		}
		
		
		//պ��λ�ð�ͣ
		if(work_zhangan_x_status == 1 || work_zhangan_w_status == 1 || work_zhangan_y_status == 1  
		|| work_zhangan_back_status == 1 || work_zhangan_w_back_status == 1 || work_zhangan_x_back_status == 1)//|| work_zhanganing_status == 1
		{
			PulseOut_1_Stop();
			PulseOut_2_Stop();
			PulseOut_3_Stop();
			
			work_zhangan_x_status = 0;
			work_zhangan_w_status = 0;
			work_zhangan_y_status = 0;
			
			work_zhanganing_status = 0;
			work_zhangan_back_status = 0;
			work_zhangan_w_back_status = 0;
			work_zhangan_x_back_status = 0;
		
		}
		
		
		//����λ�ð�ͣ
	/* 	if(work_dianyang_w_l_lie_status == 1 || work_dianyang_w_s_status == 1 || work_dianyang_x_s_status == 1
		  || work_dianyang_w_s_lie_status == 1 || work_dianyang_x_s_hang_status == 1 || work_dianyang_w_m_lie_status == 1 
		  || work_dianyang_y_down_status == 1 || work_dianyanging_s_status == 1 || work_dianyang_x_m_status == 1 
		  || work_dianyang_x_l_status == 1 || work_dianyang_y_up_status == 1 || work_dianyang_x_back_status == 1 
		  || work_dianyang_x_get_zero_status == 1 || work_dianyang_w_get_zero_status == 1)
		  {
				PulseOut_1_Stop();
				PulseOut_2_Stop();
				PulseOut_3_Stop();
			  
				work_dianyang_w_l_lie_status = 0;
				work_dianyang_w_s_status = 0;
				work_dianyang_x_s_status = 0;
				work_dianyang_w_s_lie_status = 0;
				work_dianyang_x_s_hang_status = 0;
				work_dianyang_w_m_lie_status = 0;

				work_dianyang_y_down_status = 0;
				work_dianyanging_s_status = 0;
				work_dianyang_x_m_status = 0;
				work_dianyang_x_l_status= 0;

				work_dianyang_y_up_status = 0;

				work_dianyang_x_back_status = 0;
				
				work_dianyang_x_get_zero_status = 0;
				work_dianyang_w_get_zero_status = 0;
				
		  } */
		
	}		
		
	//wdt_test by ����ΰ
		/* if(start_once == 1)
		{
			while(1)
			{
				
			}
			
		} */
}





