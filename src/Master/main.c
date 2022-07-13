#include "io430.h"
#include <intrinsics.h>

#define EN3V BIT3 //EN pin of the BT
#define RESEt BIT4 //11. pin of the BT whic is used for reset BT
#define LED1   BIT0 //indicator led1
#define LED2   BIT6 //indicator led2

//you should write your slave adresses here.
#define SLAVE1BIND "AT+BIND=0021,11,01C939\r\n"
#define SLAVE2BIND "AT+BIND=0021,11,01CD07\r\n"
#define SLAVE1LINK "AT+LINK=0021,11,01C939\r\n"
#define SLAVE2LINK "AT+LINK=0021,11,01CD07\r\n"

//Functions for sending string to slaves
void putc(const unsigned c);
void TX(const char *s);

//Delay function 0.1ms * a. ex a = 5 == 0.5 = 500ms delay.
void    Delayy(int a); // you can switch all of the delays of the program. The delays are experimental lesser delays are may work proper too.

//BT Mode control functions.
void AT_Mode_Role(void); // AT mode
void Data_Mode_Role(void); // Data mode

//9600 ve 38400 baud rate init funcs.
void Uart_Init_AT_Mode(void); // 38400 br
void Uart_Init_Data_Mode(void); // 9600 br

//functions for binding the master to slaves
void Uart_At_Commands_Slave1(void);
void Uart_At_Commands_Slave2(void);

int i=0; //simple counter var.
 
int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  if (CALBC1_1MHZ == 0xFF)
  {
    while(1);//Wait till proccessor fully calibrated.
  }
  DCOCTL = 0;
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  
  P1OUT=0x00; //init of outputs
  P1DIR=LED1+LED2+EN3V+RESEt;
  
  P1SEL=BIT1+BIT2;//TX RX inits
  P1SEL2=BIT1+BIT2;//TX RX inits
  
  __bis_SR_register(GIE); //General interrupt enable
  
  while(1){
    AT_Mode_Role();//BT went into AT mode.
    Delayy(10);
    Uart_Init_AT_Mode();//38400 baudrate for uart is set which is required for AT commands.
    Delayy(10);
    Uart_At_Commands_Slave1();//Master connected to slave 1
    Delayy(10);
    Data_Mode_Role();//BT went into DATA mode
    Delayy(10);
    Uart_Init_Data_Mode();//9600 baudrate for uart is set which is required for DATA commands.
    Delayy(100);//Wait for bt connection
    i=0;
    //Connection established with slave 1 for 15 seconds
    while(i<150){
      TX("1");
      Delayy(1);
      i++;
    }
    Delayy(10);
    
    //Same process for slave 2
    AT_Mode_Role();
    Delayy(10);
    Uart_Init_AT_Mode();
    Delayy(10);
    Uart_At_Commands_Slave2();
    Delayy(10);
    Data_Mode_Role();
    Delayy(10);
    Uart_Init_Data_Mode();
    Delayy(100);
    i=0;
    while(i<150){
      TX("2");
      Delayy(1);
      i++;
    }
    Delayy(10);
  }
}

//interrupt for UCA0RXBUF control.
#pragma vector=USCIAB0RX_VECTOR
__interrupt void RX_ISR(void)
{
   if(UCA0RXBUF=='a')
  {
    P1OUT|=LED1;
  }
  else if(UCA0RXBUF=='b')
  {
    P1OUT&=~LED1;
  }
  else if(UCA0RXBUF=='c')
  {
    P1OUT|=LED2;
  }
  else if(UCA0RXBUF=='d')
  {
    P1OUT&=~LED2;
  }
}
void AT_Mode_Role(void)
{
  //It must be reset with 3v on the EN pin for it to switch to BT at mode.
  P1OUT |= EN3V;//3V is set
  Delayy(10);//wait 1 sec
  P1OUT|=RESEt;//Reset bluetooth
  Delayy(2);//wait 0.2 sec 
  P1OUT&=~RESEt;//close the reset
  Delayy(30);//wait 3 sec
}

void Data_Mode_Role(void)
{
  //To switch to BT data mode, it must be reset when there is 0v on the EN pin.
  P1OUT&=~EN3V;//3V is closed
  Delayy(2);
  P1OUT|=RESEt;//BT is reseted
  Delayy(2);
  P1OUT&=~RESEt;//reset is closed
  Delayy(15);// wait 1.5 sec
}

//Bind and link commands
void Uart_At_Commands_Slave1(void)
{
  TX(SLAVE1BIND);
  TX(SLAVE1LINK);
}
void Uart_At_Commands_Slave2(void)
{
  TX(SLAVE2BIND);
  TX(SLAVE2LINK);
}

//9600 baudrate
void Uart_Init_Data_Mode(void)
{ 
  UCA0CTL1|=UCSSEL_2;
  UCA0BR0=104;
  UCA0BR1=0;
  UCA0MCTL=UCBRF_0+UCBRS_1;
  UCA0CTL1&=~UCSWRST;
  IE2|=UCA0RXIE;
}
//38400 baudrate
void Uart_Init_AT_Mode(void)
{ 
  UCA0CTL1|=UCSSEL_2;
  UCA0BR0=26;
  UCA0BR1=0;
  UCA0MCTL=UCBRF_0+UCBRS_0;
  UCA0CTL1&=~UCSWRST;
  IE2|=UCA0RXIE;
}
//Delay function
void    Delayy(int a)
{
  while(a>0)// a * 0.1 sec delay
  {
    TACCR0  = 50000-1;
    TACTL=MC_1|ID_1|TASSEL_2|TACLR;
    while((!(TA0CTL&BIT0))&&(TA0CTL&(BIT6+BIT7)));
    TACTL=MC_0;
    a--;
  }
}
// String writing functions for sending data to slaves and debugging
void putc(const unsigned c)
{
  while (!(IFG2&UCA0TXIFG));//Wait till tx is free
    UCA0TXBUF = c;
}
void TX(const char *s)
{
  while(*s)
    putc(*s++);
}