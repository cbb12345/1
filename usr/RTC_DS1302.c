#include "RTC_DS1302.h"

/*----------------- 引脚 -------------------------------------------------------------------------------------*/
#define RTC_SCLK_PIN			GPIO_Pin_7
#define RTC_SCLK_GPIO			GPIOA
#define RTC_IO_PIN				GPIO_Pin_6
#define RTC_IO_GPIO				GPIOA

#define RTC_RST_PIN				GPIO_Pin_5
#define RTC_RST_GPIO			GPIOA

/*----------------- RCC -------------------------------------------------------------------------------------*/
#define RTC_APB2_RCC			RCC_APB2Periph_GPIOA


/*------------------------------------------------------------------------------------------------------*/
#define RTC_SCLK_PIN_H			GPIO_SetBits(RTC_SCLK_GPIO,RTC_SCLK_PIN)
#define RTC_SCLK_PIN_L			GPIO_ResetBits(RTC_SCLK_GPIO,RTC_SCLK_PIN)

#define RTC_IO_PIN_H			GPIO_SetBits(RTC_IO_GPIO,RTC_IO_PIN)
#define RTC_IO_PIN_L			GPIO_ResetBits(RTC_IO_GPIO,RTC_IO_PIN)

#define RTC_RST_PIN_H			GPIO_SetBits(RTC_RST_GPIO,RTC_RST_PIN)
#define RTC_RST_PIN_L			GPIO_ResetBits(RTC_RST_GPIO,RTC_RST_PIN)

#define CLK_H			RTC_SCLK_PIN_H
#define CLK_L			RTC_SCLK_PIN_L
#define IO_H			RTC_IO_PIN_H
#define IO_L			RTC_IO_PIN_L
#define RST_H			RTC_RST_PIN_H
#define RST_L			RTC_RST_PIN_L

#define read_IOBit		((u8)((RTC_IO_GPIO->IDR&RTC_IO_PIN) != 0))
#define SET_DATA_IN		RTC_IO_GPIO->CRL = (RTC_IO_GPIO->CRL & (~(0xf<<pinnum))) | (8<<pinnum);RTC_IO_GPIO->BSRR |= RTC_IO_PIN
#define SET_DATA_OUT	RTC_IO_GPIO->CRL = (RTC_IO_GPIO->CRL & (~(0xf<<pinnum))) | (3<<pinnum)

/*----------------- 延时 -----------------------------------------------------------------------*/
#define DELAY_TMR										TIM7
#define DELAY_TMR_APB1_RCC					RCC_APB1Periph_TIM7


static u8 pinnum = 0;

/*------------------------------------------------------------------------------------------------------*/
void DS1302_Delay_Init(void)
{
	RCC_APB1PeriphClockCmd(DELAY_TMR_APB1_RCC, ENABLE);
	DELAY_TMR->CR1 |= (u16)0x08;
}

static void DelayUS(u16 us)
{
	DELAY_TMR->ARR = us;
	DELAY_TMR->PSC = 71;
	DELAY_TMR->SR = 0;
	DELAY_TMR->CNT = 0;
	DELAY_TMR->CR1 |= 1;

	while ( DELAY_TMR->SR == 0 )
	{	}
	DELAY_TMR->SR = 0;
}

__INLINE static void delayns(u16 ns)
{
	u32 cnt;
	cnt = (ns/14)-3;
	while (cnt--)
	{}
}

static void rtc_gpio_config(void)
{
	GPIO_InitTypeDef		g;
	
	g.GPIO_Speed = GPIO_Speed_50MHz;

	g.GPIO_Pin = RTC_SCLK_PIN;
	g.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( RTC_SCLK_GPIO , &g );

	g.GPIO_Pin = RTC_IO_PIN;
	g.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( RTC_IO_GPIO , &g );
	IO_L;

	g.GPIO_Pin = RTC_RST_PIN;
	g.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( RTC_RST_GPIO , &g );
	RST_L;
}
static void rtc_rcc_config(void)
{
#ifdef RTC_AHB_RCC
	RCC_AHBPeriphClockCmd(RTC_AHB_RCC, ENABLE);
#endif// RTC_AHB_RCC

#ifdef RTC_APB2_RCC
	RCC_APB2PeriphClockCmd( RTC_APB2_RCC, ENABLE );
#endif // RTC_APB2_RCC

#ifdef RTC_APB1_RCC
	RCC_APB1PeriphClockCmd( RTC_APB1_RCC, ENABLE );
#endif // RTC_APB1_RCC

}

/*------------------------------------------------------------------------------------------------------*/
__INLINE static void writeByte(u8 w_addr, u8 data)
{
	u8 i;
	u16 ad;

	ad = (data<<8) | w_addr;
	CLK_L;
	RST_H;
	DelayUS(4);
	for (i=0; i<16; ++i)
	{
		CLK_L;
		DelayUS(1);
		if ( (ad&(1<<i)) == 0 )
			IO_L;
		else
			IO_H;
		CLK_H;
		delayns(300);
		
	}

	RST_L;
	DelayUS(4);
}
__INLINE static u8 readByte(u8 r_addr)
{
	u8 data,i;

	CLK_L;
	RST_H;//CE使能端，高电平使能芯片
	DelayUS(4);
	for (i=0; i<8; ++i)
	{
		CLK_L;
		DelayUS(1);
		if ( (r_addr&(1<<i)) == 0 )
			IO_L;
		else
			IO_H;
		CLK_H;
		delayns(300);
		
	}

	SET_DATA_IN;	//上拉输入
	
	data = 0;
	for (i=0; i<8; ++i)
	{
		CLK_H;
		CLK_L;
		data |= ( read_IOBit << i );
	}

	RST_L;
	SET_DATA_OUT; //推挽 输出
	return data;
}
#if 0
//返回1表示时钟振荡器停止，此时处于低功耗状态，返回0表示时钟振荡器在运行
static u8 get_state(void)
{
	return (u8)((readByte(0x81)&0x80) != 0);
}
//写1表示表示时钟振荡器停止，进入低功耗状态，写0表示时钟振荡器启动
static void set_state(u8 state)
{
	u8 sec;
	sec = (readByte(0x81)&0x7f) | (state<<7);
	writeByte(0x8e, 0x00);
	writeByte(0x80,sec);
	writeByte(0x8e, 0x80);
}
//1=12小时制, 0=24小时制
static u8 get_24hours(void)
{
	return (u8)((readByte(0x85)&0x80) != 0);
}
#endif
//1=12小时制, 0=24小时制
static void set_24hours(u8 state)
{
	u8 tmp;
	tmp = (readByte(0x85)&0x7f) | (state<<7);
	writeByte(0x8e, 0x00);
	writeByte(0x84,tmp);
	writeByte(0x8e, 0x80);
}

u8 BCD2HEX(u8 bcd_data)	//BCD->HEX  
{ 
	unsigned char temp; 
	temp=(bcd_data/16*10 + bcd_data%16); 
	return temp; 
} 

u8 HEX2BCD(u8 hex_data)	//HEX->BCD   
{ 
	unsigned char temp; 
	temp=(hex_data/10*16 + hex_data%10); 
	return temp; 
} 

void getTime(u8 *recbuf)//HEX 年月日时分秒，返回给上位机的也是Hex码
{
	u8 i, addr = 0x8D;
	
	for (i=0; i<6; ++i)
	{
		recbuf[i] = readByte(addr);
		addr -= 2;
		if ( addr == 0x8B ) addr -= 2;//将星期去掉
	}

	recbuf[1] &= 0x1f; 	//月
	recbuf[2] &= 0x3f; 	//日
	recbuf[3] &= 0x3f; 	//时
	recbuf[4] &= 0x7f;	//分
	recbuf[5] &= 0x7f;	//秒
	
	for(i=0; i<6; i++)//BCD -> HEX
	{
		recbuf[i] = BCD2HEX(recbuf[i]);
	}
}

void setTime(u8 time[6])//HEX 年月日时分秒上位机传过来的是Hex码不是BCD码，因此需要进行转换
{
 	u8 reg[7],i,addr=0x8C;
	u8 clock[6];
	
	for(i=0; i<6; i++)//HEX -> BCD
	{
		clock[i] = HEX2BCD(time[i]);
	}

 	getTime(reg);//获取时间，获取到的是Hex码
 
	for(i=0; i<6; i++)//HEX -> BCD
	{
		reg[i] = HEX2BCD(reg[i]);
	}
	
 	reg[0] = (clock[0]); 												//年
 	reg[1] = (0x01); 														//周
 	reg[2] = (clock[1]&0x1f); 									//月
 	reg[3] = (clock[2]&0x3f); 									//日
 	reg[4] = (reg[3]&0x80) | (clock[3]&0x3f);  	//时
 	reg[5] = (clock[4]&0x7f); 									//分
 	reg[6] = (reg[5]&0x80) | (clock[5]&0x7f);  	//秒
 
 	writeByte(0x8e, 0x00);  //关闭写保护
 
	for (i=0; i<7; ++i)
	{
		writeByte(addr,reg[i]);
		addr -= 2;
	}
 
 	writeByte(0x8e, 0x80);  //写保护
}

void RTC_init(void)
{
	u8 i;
	
	DS1302_Delay_Init();
	
	for (i=0;i<8;++i)
	{
		if ( (RTC_IO_PIN&(1<<i)) == 0 )
			pinnum++;
		else
			break;
	}

	pinnum <<= 2;
	rtc_rcc_config();
	rtc_gpio_config();
	
	set_24hours(0); //设置为24小时制
}


