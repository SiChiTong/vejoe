#include "APP.h"

#include "Components.h"
	
	void showSpeedValue(int speedLeft,int speedRight)
	{
		int showDirectNumber = 1;
		char showDirectChar = '+';
		OLED_ShowString(00,20,"LEFT");
		if( speedLeft<0)
		{
			showDirectNumber = -1;
			showDirectChar = '-';
		}
		OLED_ShowChar(60,20,showDirectChar,12,1);
		OLED_ShowNumber(75,20,showDirectNumber*speedLeft,6,12);
		//右轮速度显示
		OLED_ShowString(00,30,"RIGHT");
		showDirectNumber = 1;
		showDirectChar = '+';
		if(speedRight<0)		  
		{
			showDirectNumber = -1;
			showDirectChar = '-';
		}
		OLED_ShowChar(60,30,showDirectChar,12,1);
		OLED_ShowNumber(75,30,showDirectNumber * speedRight,6,12);
		//显示屏刷新
		OLED_Refresh_Gram();
	}
	
	void ShowVolCurValue(int encodeIdx, u8 voltage,u8 current)
	{
		u8 showLocation = 40;		
		u8* strEncoder = "Lvc";
		if( encodeIdx > 0)
		{
			strEncoder = "Rvc";
			showLocation = 50;		
		}
		OLED_ShowString(00,showLocation,strEncoder);
		OLED_ShowNumber(50,showLocation,voltage,4,12);
		OLED_ShowNumber(80,showLocation,current,4,12);
		
		//显示屏刷新
		OLED_Refresh_Gram();
	}
