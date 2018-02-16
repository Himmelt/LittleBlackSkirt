#include "STC15F2K60S2.H"

#define FOSC 11059200L          //系统频率
#define BAUD 115200             //串口波特率
#define S2_S0 0x01              //P_SW2.0

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3


typedef unsigned char BYTE;
typedef unsigned int WORD;


sbit POWER_SWITCH = P2^4;
sbit SERIAL_RXD = P1^0;
sbit SERIAL_TXD = P1^1;

bit busy = 0;

void SendData(BYTE dat);
void SendString(char *s);

void main(){

    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;

    P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)

    S2CON = 0x50;               //8位可变波特率
    //S2CON = 0xda;               //9位可变波特率,校验位初始为1
    //S2CON = 0xd2;               //9位可变波特率,校验位初始为0

    T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2
    IE2 = 0x01;                 //使能串口2中断
    EA = 1;

    SendString("STC15F2K60S2\r\nUart2 Test !\r\n");
    while(1);
}



/*----------------------------
UART2 中断服务程序
-----------------------------*/
void Uart2() interrupt 8 using 1
{
    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //清除S2RI位
        P0 = S2BUF;             //P0显示串口数据
        P2 = (S2CON & S2RB8);   //P2.2显示校验位
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //清除S2TI位
        busy = 0;               //清忙标志
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(BYTE dat)
{
    while (busy);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    busy = 1;
    S2BUF = ACC;                //写数据到UART2数据寄存器
}

/// 发送字符串
void SendString(char *s) {
	  ///检测字符串结束标志
    while (*s) {
        SendData(*s++);         //发送当前字符
    }
}
