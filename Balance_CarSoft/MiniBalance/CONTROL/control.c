/*

*/

#include "control.h"	
#include "filter.h"	
#include "Check_OverLoad.h"


int lastEncoderLeft=0,lastEncoderRight=0,deltaEncoderLeft=0,deltaEncoderRight=0;

int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
u8 Flag_Target;
u32 Flash_R_Count;
int Voltage_Temp,Voltage_Count,Voltage_All;

void updateEcoderDelta(void)
{
	Encoder_Left=Read_Encoder(2);                                       //===��ȡ��������ֵ
	Encoder_Right=Read_Encoder(4);                                      //===��ȡ��������ֵ
	
	deltaEncoderLeft = Encoder_Left - lastEncoderLeft;
	deltaEncoderRight = Encoder_Right - lastEncoderRight;
	
	if(abs(deltaEncoderLeft) > ENCODER_TIM_PERIOD_HALF)
	{//���������ֵ�ı仯�������˷�ת
		deltaEncoderLeft = ((Encoder_Left>lastEncoderLeft?lastEncoderLeft:Encoder_Left) + (ENCODER_TIM_PERIOD - (Encoder_Left>lastEncoderLeft?Encoder_Left:lastEncoderLeft)));
	}
	if(abs(deltaEncoderRight) > ENCODER_TIM_PERIOD_HALF)
	{//���������ֵ�ı仯�������˷�ת
		deltaEncoderRight = ((Encoder_Right>lastEncoderRight?lastEncoderRight:Encoder_Right) + (ENCODER_TIM_PERIOD - (Encoder_Right>lastEncoderRight?Encoder_Right:lastEncoderRight)));
	}
	
	lastEncoderLeft = Encoder_Left;
	lastEncoderRight = Encoder_Right;
}

/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ�����ʱ��ͬ��				 
**************************************************************************/
int EXTI2_IRQHandler(void) 
{    
	if(INT==0)		
	{   
		EXTI_ClearITPendingBit(EXTI_Line2);  //����жϱ�־λ                                  
		Flag_Target=!Flag_Target;
		if(delay_flag==1)
		{
			if(++delay_50==10)delay_50=0, delay_flag=0;                     //���������ṩ50ms�ľ�׼��ʱ
		}
		if(Flag_Target==1)                                                  //5ms��ȡһ�������Ǻͼ��ٶȼƵ�ֵ�����ߵĲ���Ƶ�ʿ��Ը��ƿ������˲��ͻ����˲���Ч��
		{
			Get_Angle(Way_Angle);                                               //===������̬	
//			Get_MC6();                                                          //===��ȡ��ģң����������					
			if(++Flash_R_Count==150&&Angle_Balance>30)
			Flash_Read();             //=====��ȡFlash��PID����		
			Voltage_Temp=Get_battery_volt();		                                //=====��ȡ��ص�ѹ		
			Voltage_Count++;                                                    //=====ƽ��ֵ������
			Voltage_All+=Voltage_Temp;                                          //=====��β����ۻ�
			if(Voltage_Count==100) Voltage=Voltage_All/100,Voltage_All=0,Voltage_Count=0;//=====��ƽ��ֵ		
			return 0;	                                               
		}                                                                   //10ms����һ�Σ�Ϊ�˱�֤M�����ٵ�ʱ���׼�����ȶ�ȡ����������
		updateEcoderDelta();
		Get_Angle(Way_Angle);                                               //===������̬	
		Read_Distane();                                                     //===��ȡ��������������ֵ
		if(Bi_zhang==0)Led_Flash(100);                                      //===LED��˸;����ģʽ 1s�ı�һ��ָʾ�Ƶ�״̬	
		else           Led_Flash(0);                                        //===LED��˸;������ģʽ ָʾ�Ƴ���	
//		Key();                                                              //===ɨ�谴��״̬ ����˫�����Ըı�С������״̬
		Balance_Pwm =balance(Angle_Balance,Gyro_Balance);                   //===ƽ��PID����	
		Velocity_Pwm=velocity(deltaEncoderLeft,deltaEncoderRight);                  //===�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ��
		Turn_Pwm    =turn(deltaEncoderLeft,Encoder_Right,Gyro_Turn);            //===ת��PID����     
		Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                            //===�������ֵ������PWM
		Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                            //===�������ֵ������PWM
		Xianfu_Pwm();                                                       //===PWM�޷�
		if(Pick_Up(Acceleration_Z,Angle_Balance,Encoder_Left,deltaEncoderRight))//===����Ƿ�С��������
		Flag_Stop=1;	                                                      //===���������͹رյ��
		if(Put_Down(Angle_Balance,deltaEncoderLeft,deltaEncoderRight))              //===����Ƿ�С��������
		Flag_Stop=0;	                                                      //===��������¾��������
		//
		if(Turn_Off(Angle_Balance,Voltage)==0)                              //===����������쳣
		Set_Pwm(Moto1,Moto2);                                               //===��ֵ��PWM�Ĵ���  

	}       	
	return 0;	  
} 

/**************************************************************************
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
**************************************************************************/
float static_kp = 0.5, static_ki = 0.5;
int balance(float Angle,float Gyro)
{  
	float Bias;
//	static int sum_offset = 0;
	int balance;
	
	Bias = Angle - Zhongzhi;   
	//
//	if(Angle > 270)
//	Bias = Zhongzhi - (360 - Angle); 	
//	else if(Angle < 90)	
//	Bias = Angle - Zhongzhi;                     		//===���ƽ��ĽǶ���ֵ �ͻ�е���
//	
//	sum_offset += Bias;
//	
//	if(sum_offset > 1000)sum_offset = 1000;
//	else if(sum_offset < -1000)sum_offset = -1000;
	
	balance = Balance_Kp * Bias + Gyro * Balance_Kd;
//	balance = Balance_Kp * Bias + static_ki* sum_offset + Gyro * Balance_Kd;   	//===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	return balance;
}
/*

*/

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ�����Target_Velocity�����磬�ĳ�60�ͱȽ�����
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
	static float Velocity,Encoder_Least,Encoder,Movement;
	static float Encoder_Integral,Target_Velocity;
	//=============ң��ǰ�����˲���=======================// 
	if(Flag_sudu==10)  Target_Velocity=80;				//ת��ʱ���ø���ģʽ
	else if(Bi_zhang!=0&&Flag_sudu==1)  Target_Velocity=55;                 //����������ģʽ,�Զ��������ģʽ
	else            Target_Velocity=110;   
	
	if(1==Flag_Qian)    	Movement=-Target_Velocity/Flag_sudu;	         //===ǰ����־λ��1 
	else if(1==Flag_Hou)	Movement=Target_Velocity/Flag_sudu;         //===���˱�־λ��1
	else  Movement=0;	
	
	if(Bi_zhang==1&&Flag_Left!=1&&Flag_Right!=1)        //�������ģʽ
	{
		if(Distance<500)  Movement=-Target_Velocity/Flag_sudu;
	}	
	else if(Bi_zhang==2&&Flag_Left!=1&&Flag_Right!=1)        //�������ģʽ
	{
		if(Distance>100&&Distance<300)  Movement=Target_Velocity/Flag_sudu;
	}
	//=============�ٶ�PI������=======================//	
	Encoder_Least = (encoder_left+encoder_right)-0;                    //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩 
	Encoder *= 0.8;		                                                //===һ�׵�ͨ�˲���       
	Encoder += Encoder_Least*0.2;	                                    //===һ�׵�ͨ�˲���    
	Encoder_Integral +=Encoder;                                       //===���ֳ�λ�� ����ʱ�䣺10ms
	Encoder_Integral=Encoder_Integral-Movement;                       //===����ң�������ݣ�����ǰ������
	if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===�����޷�
	if(Encoder_Integral<-10000)	Encoder_Integral=-10000;              //===�����޷�	
	Velocity=Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;        //===�ٶȿ���	

	if(Flag_Hover==1)Zhongzhi=-Encoder/10-Encoder_Integral/300;

	if(Turn_Off(Angle_Balance,Voltage)==1||Flag_Stop==1)   Encoder_Integral=0;      //===����رպ��������
	return Velocity;
}

/**************************************************************************
�������ܣ�ת�����  �޸�ת���ٶȣ����޸�Turn_Amplitude����
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//ת�����
{
	static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
	float Turn_Amplitude=100/Flag_sudu,Kp=52,Kd=0;     
	//=============ң��������ת����=======================//
	if(1==Flag_Left||1==Flag_Right)                      //��һ������Ҫ�Ǹ�����תǰ���ٶȵ����ٶȵ���ʼ�ٶȣ�����С������Ӧ��
	{
		if(++Turn_Count==1)
		Encoder_temp=myabs(encoder_left+encoder_right);
		Turn_Convert=50/Encoder_temp;
		if(Turn_Convert<0.6)Turn_Convert=0.6;
		if(Turn_Convert>3)Turn_Convert=3;
	}	
	else
	{
		Turn_Convert=0.9;
		Turn_Count=0;
		Encoder_temp=0;
	}			
	if(1==Flag_Left)	  Turn_Target-=Turn_Convert;
	else if(1==Flag_Right)Turn_Target+=Turn_Convert; 
	else Turn_Target=0;

	if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===ת���ٶ��޷�
	if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
	if(Flag_Qian==1||Flag_Hou==1)  Kd=0.5;        
	else Kd=0;   //ת���ʱ��ȡ�������ǵľ��� �е�ģ��PID��˼��
	//=============ת��PD������=======================//
	Turn=-Turn_Target*Kp-gyro*Kd;                 //===���Z�������ǽ���PD����
	return Turn;
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
	if(moto1>0)	AIN2=1, AIN1=0;
	else AIN2=0, AIN1=1;
	PWMA=myabs(moto1);
	if(moto2>0)	BIN1=1,	BIN2=0;
	else        BIN1=0,	BIN2=1;
	PWMB=myabs(moto2);
}
/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	int Amplitude=6900;    //===PWM������7200 ������6900
	if(Flag_Qian==1)  Moto1+=DIFFERENCE;  //DIFFERENCE��һ������ƽ��С������ͻ�е��װ�����һ��������ֱ���������������С�����и��õ�һ���ԡ�
	if(Flag_Hou==1)   Moto2-=DIFFERENCE;
	if(Moto1<-Amplitude) Moto1=-Amplitude;	
	if(Moto1>Amplitude)  Moto1=Amplitude;	
	if(Moto2<-Amplitude) Moto2=-Amplitude;	
	if(Moto2>Amplitude)  Moto2=Amplitude;
}
/**************************************************************************
�������ܣ������޸�С������״̬ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void)
{	
	u8 tmp,tmp2;
	tmp=click_N_Double(50); 
	if(tmp==1)Flag_Stop=!Flag_Stop;//��������С������ͣ
	else if(tmp==2)Flag_Show=!Flag_Show;//˫������С������ʾ״̬
	tmp2=Long_Press(); 
	if(tmp2==1) Bi_zhang=!Bi_zhang;		//��������С���Ƿ���볬��������ģʽ 
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������Ǻ͵�ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
uint8_t g_lift_motor_error = 0, g_right_motor_error = 0;
u8 Turn_Off(float angle, int voltage)
{
	u8 temp;
	//
	Output_FeedbackSig(0, &g_lift_motor_error, 0, 0, 0);
	Output_FeedbackSig(1, &g_right_motor_error, 0, 0, 0);

	if(angle < (-40+Zhongzhi) || angle > (40+Zhongzhi) || 1 == Flag_Stop || voltage < 1110 ||
		g_lift_motor_error >= 1 || g_right_motor_error >= 1)	//��ص�ѹ����11.1V�رյ��
	{	                                                 		//===��Ǵ���40�ȹرյ��
		temp=1;                                            		//===Flag_Stop��1�رյ��
		AIN1=0;                                            
		AIN2=0;
		BIN1=0;
		BIN2=0;
	}
	else
	temp=0;
	return temp;			
}
	
/**************************************************************************
�������ܣ���ȡ�Ƕ� �����㷨�������ǵĵ�У�����ǳ����� 
��ڲ�������ȡ�Ƕȵ��㷨 1��DMP  2�������� 3�������˲�
����  ֵ����
**************************************************************************/
void Get_Angle(u8 way)
{ 
	float Accel_Y,Accel_Angle,Accel_Z,Gyro_X,Gyro_Z;
	Temperature=Read_Temperature();      //===��ȡMPU6050�����¶ȴ��������ݣ����Ʊ�ʾ�����¶ȡ�
	if(way==1)                           //===DMP�Ķ�ȡ�����ݲɼ��ж϶�ȡ���ϸ���ѭʱ��Ҫ��
	{	
		Read_DMP();                      //===��ȡ���ٶȡ����ٶȡ����
		Angle_Balance=-Roll;             //===����ƽ�����
		Gyro_Balance=-gyro[0];            //===����ƽ����ٶ�
		Gyro_Turn=gyro[2];               //===����ת����ٶ�
		Acceleration_Z=accel[2];         //===����Z����ٶȼ�
	}			
	else
	{
		Gyro_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_XOUT_L);    //��ȡY��������
		Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //��ȡZ��������
		Accel_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_YOUT_L); //��ȡX����ٶȼ�
		Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //��ȡZ����ٶȼ�
		if(Gyro_X>32768)  Gyro_X-=65536;                       //��������ת��  Ҳ��ͨ��shortǿ������ת��
		if(Gyro_Z>32768)  Gyro_Z-=65536;                       //��������ת��
		if(Accel_Y>32768) Accel_Y-=65536;                      //��������ת��
		if(Accel_Z>32768) Accel_Z-=65536;                      //��������ת��
		Gyro_Balance=Gyro_X;                                   //����ƽ����ٶ�
		Accel_Angle=atan2(Accel_Y,Accel_Z)*180/PI;             //�������	
		Gyro_X=Gyro_X/16.4;                                    //����������ת��	
		if(Way_Angle==2)Kalman_Filter(Accel_Angle,Gyro_X);	   //�������˲�	
		else if(Way_Angle==3)Yijielvbo(Accel_Angle,Gyro_X);    //�����˲�
		Angle_Balance=angle;                                   //����ƽ�����
		Gyro_Turn=Gyro_Z;                                      //����ת����ٶ�
		Acceleration_Z=Accel_Z;                                //===����Z����ٶȼ�	
	}
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	int temp;
	if(a<0)  temp=-a;  
	else temp=a;
	return temp;
}
/**************************************************************************
�������ܣ����С���Ƿ�����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int Pick_Up(float Acceleration,float Angle,int encoder_left,int encoder_right)
{ 		   
	static u16 flag,count0,count1,count2;
	if(flag==0)                                                                   //��һ��
	{
		if(myabs(encoder_left)+myabs(encoder_right)<30)                         //����1��С���ӽ���ֹ
		count0++;
		else 
		count0=0;		
		if(count0>10)				
		flag=1,count0=0; 
	} 
	if(flag==1)                                                                  //����ڶ���
	{
		if(++count1>200)       count1=0,flag=0;                                 //��ʱ���ٵȴ�2000ms
		if(Acceleration>26000&&(Angle>(-20+Zhongzhi))&&(Angle<(20+Zhongzhi)))   //����2��С������0�ȸ���������
		flag=2; 
	} 
	if(flag==2)                                                                  //������
	{
		if(++count2>100)       count2=0,flag=0;                                   //��ʱ���ٵȴ�1000ms
		if(myabs(encoder_left+encoder_right)>135)                                 //����3��С������̥��Ϊ�������ﵽ����ת��   
		{
			flag=0;                                                                                     
			return 1;                                                               //��⵽С��������
		}
	}
	return 0;
}
/**************************************************************************
�������ܣ����С���Ƿ񱻷���
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int Put_Down(float Angle,int encoder_left,int encoder_right)
{ 		   
	static u16 flag,count;	 
	if(Flag_Stop==0)                           //��ֹ���      
	return 0;	                 
	if(flag==0)                                               
	{
		if(Angle>(-10+Zhongzhi)&&Angle<(10+Zhongzhi)&&encoder_left==0&&encoder_right==0)         //����1��С������0�ȸ�����
		flag=1; 
	} 
	if(flag==1)                                               
	{
		if(++count>50)                                          //��ʱ���ٵȴ� 500ms
		{
			count=0;flag=0;
		}
		if(encoder_left<-3&&encoder_right<-3&&encoder_left>-60&&encoder_right>-60)                //����2��С������̥��δ�ϵ��ʱ����Ϊת��  
		{
			flag=0;
			flag=0;
			return 1;                                             //��⵽С��������
		}
	}
	return 0;
}


/**************************************************************************
�������ܣ��ɼ�ң�������ź�
��ڲ�������
����  ֵ����
**************************************************************************/
void  Get_MC6(void)
{ 
	if(Flag_Left==0&&Flag_Right==0)
	{	
		if((Remoter_Ch1>1650&&Remoter_Ch1<2100)||(Remoter_Ch1>21650&&Remoter_Ch1<22100))	Flag_Qian=1,Flag_Hou=0,Flag_sudu=1;//////////////ǰ
		else if((Remoter_Ch1<1350&&Remoter_Ch1>900) ||(Remoter_Ch1<21350&&Remoter_Ch1>20900))	Flag_Qian=0,Flag_Hou=1,Flag_sudu=1;//////////////��
		else if ((Remoter_Ch1>1350&&Remoter_Ch1<1650) ||(Remoter_Ch1>21350&&Remoter_Ch1<21650))	Flag_Qian=0,Flag_Hou=0;//////////////ͣ
	}
	if(Flag_Qian==0&&Flag_Hou==0)
	{	
		if((Remoter_Ch2>1650&&Remoter_Ch2<2100)||(Remoter_Ch2>21650&&Remoter_Ch2<22100))Flag_Left=1,Flag_Right=0,Flag_sudu=1;//////////////��
		else if((Remoter_Ch2<1350&&Remoter_Ch2>900) ||(Remoter_Ch2<21350&&Remoter_Ch2>20900))Flag_Left=0,Flag_Right=1,Flag_sudu=1;//////////////��
		else if ((Remoter_Ch2>1350&&Remoter_Ch2<1650) ||(Remoter_Ch2>21350&&Remoter_Ch2<21650))Flag_Left=0,Flag_Right=0;//////////////ͣ
	}
}	
