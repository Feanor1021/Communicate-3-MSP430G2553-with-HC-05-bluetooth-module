#include "io430.h"
#include "intrinsics.h"

void Uart_Init(void);//9600 baud rate
void adc_init(void);//init. of the adc10

//Debug functions
void TX(const char *s);
void putc(const unsigned c);

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  if (CALBC1_1MHZ == 0xFF)
  {
    while(1);//wait till the processor is fully calibrated
  }
  DCOCTL = 0;
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  
  adc_init();//calibration of the adc10 is done
  Uart_Init();//calibration of the uart module is done
  
  __bis_SR_register(GIE+LPM4_bits);//general interrupt enable and lmp4 bits are set
  
  while(1);//BT will sleep till '1' is reached its RX channel.
}
void Uart_Init(void)
{  
  P1SEL=BIT1+BIT2;//RX-TX
  P1SEL2=BIT1+BIT2;//RX-TX
  
  UCA0CTL1|=UCSSEL_2;//SMCLK
  UCA0BR0=104;
  UCA0BR1=0;
  UCA0MCTL=UCBRF_0+UCBRS_1;
  UCA0CTL1&=~UCSWRST;
  IE2|=UCA0RXIE;//RX interrupts is set
}

void adc_init()
{
  ADC10CTL0&=~ENC;// Disable Conversation - ADC10CTL0 REGISTER CANNOT BE MODYFIED WHILE EMC IS ENABLED. 
  ADC10CTL0|=SREF_1|ADC10SHT_3|REFON|ADC10ON|ADC10IE;// necessary settings have been made
  ADC10CTL1|=INCH_4|ADC10DIV_3|ADC10SSEL_0;
  ADC10AE0=BIT4;// BIT is made available for 4 analog inputs
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void isr(void)
{
  if(UCA0RXBUF=='1')//if it recives '1' on its RX channel then start the ADC10
    ADC10CTL0|=ENC+ADC10SC;
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC_ISR(void)
{
  //if analog value is over 600 then send a to master else send b
  if(ADC10MEM>600){
    UCA0TXBUF='a';
  }
  else{
    UCA0TXBUF='b';
  }
}
 
//Debug functions
void putc(const unsigned c)
{
  while (!(IFG2&UCA0TXIFG));
    UCA0TXBUF = c;
}
void TX(const char *s)
{
  while(*s)
    putc(*s++);
}
