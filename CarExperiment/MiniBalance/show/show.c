#include "show.h"

unsigned char i;          //��������
unsigned char Send_Count; //������Ҫ���͵����ݸ���
float Vol;

float sum1 = 0;
float sum2 = 0;
float mean = 0;
float differ = 0;
float yawn = 0;
/**************************************************************************
�������ܣ�OLED��ʾ
��ڲ�������
����  ֵ����
**************************************************************************/
void oled_show(void)
{
	//=============��һ����ʾС��ģʽ=======================//	
	
//	if(Way_Angle==1)	OLED_ShowString(0,0,"DMP");
//	else if(Way_Angle==2)	OLED_ShowString(0,0,"Kalman");
//	else if(Way_Angle==3)	OLED_ShowString(0,0,"Hubu");
//	if(Bi_zhang==2)	OLED_ShowString(60,0,"GEN SUI");           
//	else if(Bi_zhang==1)	OLED_ShowString(60,0,"BIZHANG");
//	else             OLED_ShowString(60,0,"PU TONG");
	//=============�ڶ�����ʾ�¶Ⱥ;���===============//	
//	OLED_ShowNumber(0,10,Temperature/10,2,12);
//	OLED_ShowNumber(23,10,Temperature%10,1,12);
//	OLED_ShowString(13,10,".");
//	OLED_ShowString(35,10,"`C");
//	OLED_ShowNumber(70,10,(u16)Distance,5,12);
//	OLED_ShowString(105,10,"mm");
	//=============��������ʾ������1=======================//	
	/*OLED_ShowString(00,20,"EncoLEFT");
	if( Encoder_Left<0)		OLED_ShowString(80,20,"-"),
		  OLED_ShowNumber(95,20,-Encoder_Left,4,12);
	else  OLED_ShowString(80,20,"+"),
		  OLED_ShowNumber(95,20, Encoder_Left,4,12);
	//=============��������ʾ������2=======================//		
		  OLED_ShowString(00,30,"EncoRIGHT");
	if(Encoder_Right<0)		  OLED_ShowString(80,30,"-"),
		  OLED_ShowNumber(95,30,-Encoder_Right,4,12);
	else  OLED_ShowString(80,30,"+"),
		  OLED_ShowNumber(95,30,Encoder_Right,4,12);	*/
	//=============��������ʾ������1=======================//	
	OLED_ShowString(00,20,"EncoLEFT");
	if( Encoder_Left<0)		OLED_ShowString(80,20,"-"),
		  OLED_ShowNumber(95,20,-Encoder_Left,4,12);
	else  OLED_ShowString(80,20,"+"),
		  OLED_ShowNumber(95,20, Encoder_Left,4,12);
	//=============��������ʾ������2=======================//		
		  OLED_ShowString(00,30,"EncoRIGHT");
	if(Encoder_Right<0)		  OLED_ShowString(80,30,"-"),
		  OLED_ShowNumber(95,30,-Encoder_Right,4,12);
	else  OLED_ShowString(80,30,"+"),
		  OLED_ShowNumber(95,30,Encoder_Right,4,12);	
	//=============��������ʾ��ѹ=======================//
	OLED_ShowString(00,40,"Volta");
	OLED_ShowString(58,40,".");
	OLED_ShowString(80,40,"V");
	OLED_ShowNumber(45,40,Voltage/100,2,12);
	OLED_ShowNumber(68,40,Voltage%100,2,12);
	if(Voltage%100<10) 	OLED_ShowNumber(62,40,0,2,12);
	//=============��������ʾ�Ƕ�=======================//
	//OLED_ShowString(0,50,"Angle");
	//if(Angle_Balance<0)		OLED_ShowNumber(45,50,Angle_Balance+360,3,12);
	//else					        OLED_ShowNumber(45,50,Angle_Balance,3,12);
	
	//if(Flag_Hover==1)	OLED_ShowString(80,50,"XT-Y");           
	//else							OLED_ShowString(80,50,"XT-N");
	
	differ = Gyro_Turn - mean;
	
	sum1 += Gyro_Turn;
	if(differ > -30 && differ < 30)
	{
		sum2 += Gyro_Turn;
		mean = mean * 0.95 + Gyro_Turn * 0.05;
	}
	else
	{
		sum2 += mean;
	}
	yawn = (sum1 - sum2) / 500;
	
	while(yawn > 360) yawn -= 360;
	while(yawn < 0) {yawn += 360;}
	
	Yawn = yawn;
	
	if(sum2 > 500000)
	{
		sum1 -= 500000;
		sum2 = 0;
	}
	
	if(Gyro_Turn >= 0) OLED_ShowString(0,50,"Turn +");
	else OLED_ShowString(0,50,"Turn -");
	if(Gyro_Turn >= 0) OLED_ShowNumber(55,50,Gyro_Turn,3,12);
	else OLED_ShowNumber(55,50,-Gyro_Turn,3,12);
	
	OLED_ShowNumber(95,50,Yawn,4,12);


	//=============ˢ��=======================//
	OLED_Refresh_Gram();	
}
/**************************************************************************
�������ܣ���APP��������
��ڲ�������
����  ֵ����
**************************************************************************/
void APP_Show(void)
{    
	static u8 flag;
	int app_2,app_3,app_4;
	app_4=(Voltage-1110)*2/3;		if(app_4<0)app_4=0;if(app_4>100)app_4=100;   //�Ե�ѹ���ݽ��д���
	app_3=Encoder_Right*1.1; if(app_3<0)app_3=-app_3;			                   //�Ա��������ݾ������ݴ�������ͼ�λ�
	app_2=Encoder_Left*1.1;  if(app_2<0)app_2=-app_2;
	flag=!flag;
	if(PID_Send==1)//����PID����
	{
		printf("{C%d:%d:%d:%d:%d:%d:%d:%d:%d}$",(int)(Balance_Kp*100),(int)(Balance_Kd*100),(int)(Velocity_Kp*100),(int)(Velocity_Ki*100),0,0,0,0,0);//��ӡ��APP����	
		PID_Send=0;	
	}	
	else if(flag==0)// 
	printf("{A%d:%d:%d:%d}$",(u8)app_2,(u8)app_3,app_4,(int)Angle_Balance); //��ӡ��APP����
	else
	printf("{B%d:%d:%d:%d}$",(int)Angle_Balance,Distance,Encoder_Left,Encoder_Right);//��ӡ��APP���� ��ʾ����
}
/**************************************************************************
�������ܣ�����ʾ��������λ���������� �ر���ʾ��
��ڲ�������
����  ֵ����
**************************************************************************/
void DataScope(void)
{   
	Vol=(float)Voltage/100;
	DataScope_Get_Channel_Data( Angle_Balance, 1 );       //��ʾ�Ƕ� ��λ���ȣ��㣩
	DataScope_Get_Channel_Data( Distance/10, 2 );         //��ʾ�����������ľ��� ��λ��CM 
	DataScope_Get_Channel_Data( Vol, 3 );                 //��ʾ��ص�ѹ ��λ��V
	//		DataScope_Get_Channel_Data( 0 , 4 );   
	//		DataScope_Get_Channel_Data(0, 5 ); //����Ҫ��ʾ�������滻0������
	//		DataScope_Get_Channel_Data(0 , 6 );//����Ҫ��ʾ�������滻0������
	//		DataScope_Get_Channel_Data(0, 7 );
	//		DataScope_Get_Channel_Data( 0, 8 ); 
	//		DataScope_Get_Channel_Data(0, 9 );  
	//		DataScope_Get_Channel_Data( 0 , 10);
	Send_Count = DataScope_Data_Generate(3);
	for( i = 0 ; i < Send_Count; i++) 
	{
		while((USART1->SR&0X40)==0);  
		USART1->DR = DataScope_OutPut_Buffer[i]; 
	}
}