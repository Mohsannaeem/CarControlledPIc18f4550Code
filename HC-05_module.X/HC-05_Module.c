

/* Interface HC-05 Bluetooth module with PIC18F4550
 * http://www.electronicwings.com
 */

#include <math.h>
#include <stdio.h>
#include <pic18f4550.h>
#include "Configuration_Header_File.h"
#include "USART_Header_File.h"
#include <stdbool.h>

#define MINTHR              8000
#define RESOLUTION          488

#define InternalOsc_8MHz    8000000
#define InternalOsc_4MHz    4000000
#define InternalOsc_2MHz    2000000
#define InternalOsc_1MHz    1000000
#define InternalOsc_500KHz  500000
#define InternalOsc_250KHz  250000
#define InternalOsc_125KHz  125000
#define InternalOsc_31KHz   31000

#define Timer2Prescale_1    1
#define Timer2Prescale_4    4
#define Timer2Prescale_16   16
void PWM_Init()             /* Initialize PWM */
{
    TRISCbits.TRISC1 = 0;   /* Set CCP2 pin as output for PWM out */
    CCP2CON = 0x0C;         /* Set PWM mode */
}

int setPeriodTo(unsigned long FPWM)/* Set period */
{
    int clockSelectBits, TimerPrescaleBits;
    int TimerPrescaleValue;
    float period;
    unsigned long FOSC, _resolution = RESOLUTION;

    if (FPWM < MINTHR)    {TimerPrescaleBits = 2; TimerPrescaleValue = Timer2Prescale_16;}
    else                  {TimerPrescaleBits = 0; TimerPrescaleValue = Timer2Prescale_1;}

    if (FPWM > _resolution)               {clockSelectBits = 7; FOSC = InternalOsc_8MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 6; FOSC = InternalOsc_4MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 5; FOSC = InternalOsc_2MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 4; FOSC = InternalOsc_1MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 3; FOSC = InternalOsc_500KHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 2; FOSC = InternalOsc_250KHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 1; FOSC = InternalOsc_125KHz;}
    else                                  {clockSelectBits = 0; FOSC = InternalOsc_31KHz;}

    period = ((float)FOSC / (4.0 * (float)TimerPrescaleValue * (float)FPWM)) - 1.0;
    period = round(period);

    OSCCON = ((clockSelectBits & 0x07) << 4) | 0x02;
    PR2 = (int)period;
    T2CON = TimerPrescaleBits;
    TMR2 = 0;
    T2CONbits.TMR2ON = 1;  /* Turn ON Timer2 */
    return (int)period;
}

void SetDutyCycleTo(float Duty_cycle, int Period)/* Set Duty cycle for given period */
{
    int PWM10BitValue;
    
    PWM10BitValue = 4.0 * ((float)Period + 1.0) * (Duty_cycle/100.0);
    CCPR2L = (PWM10BitValue >> 2);
    CCP2CON = ((PWM10BitValue & 0x03) << 4) | 0x0C;
}

  bool GetBitValue( int n, int bitPosition ) {
   return ((n >> bitPosition) & 1);
}
void main()
{
    OSCCON=0x72;              /* use internal oscillator frequency
                               which is set to * MHz */
    int out;
    char data_in;
    float Duty_Scale;
    int Period;
     TRISB = 0x00;           /* PORTD as output */
    
    TRISD = 0x00;           /* PORTD as output */
    LATD5 = 0;              /* Initial Direction */
    LATD6 = 1;                /*set PORT as output port */
    PWM_Init();  
    Period = setPeriodTo(10000);/* 10KHz PWM frequency */
    USART_Init(9600);
    /* initialize USART operation with 9600 baud rate */ 
    while(1)
    {    data_in=USART_ReceiveChar();
          //LATD=data_in;
        
         LATB=out;
         /*LATD1=GetBitValue(data_in,1);
         LATD2=GetBitValue(data_in,2);
         LATD3=GetBitValue(data_in,3);
         LATD4=GetBitValue(data_in,4);
         LATD5=GetBitValue(data_in,5);
         LATD6=GetBitValue(data_in,6);
         LATD7=GetBitValue(data_in,7);
       */
         if(GetBitValue(data_in,5)){
           Duty_Scale = ((15000.0)/2.55); /* Scale Duty Cycle */
           SetDutyCycleTo(Duty_Scale, Period);
         }
         else{
           Duty_Scale = ((8000.0)/2.55); /* Scale Duty Cycle */
           SetDutyCycleTo(Duty_Scale, Period);
           
        }
          if((data_in=='1')|(data_in=='A'))
        {   LATD0 = 0;              /* Initial Direction */
            LATD1 = 1;
            LATD2 = 0;              /* Initial Direction */
            LATD3 = 1;                   /* turn ON LED */
            USART_SendString("FORWARD"); /* send LED ON status to terminal */
        }
        else if((data_in=='2')|(data_in=='B'))
                
        {
               LATD0 = 1;              /* Initial Direction */
               LATD1 = 0;  
               LATD2 = 1;              /* Initial Direction */
               LATD3 = 0;/* turn OFF LED */
            USART_SendString("BACKWARD");/* send LED ON status to terminal */
        }
        else if((data_in=='3')|(data_in=='C'))
        {   LATD0 = 0;              /* Initial Direction */
            LATD1 = 1;
            LATD2 = 1;              /* Initial Direction */
            LATD3 = 0;                   /* turn ON LED */
            USART_SendString("LEFT"); /* send LED ON status to terminal */
        }else if((data_in=='4')|(data_in=='D'))
        {   LATD0 = 1;              /* Initial Direction */
            LATD1 = 0;
            LATD2 = 0;              /* Initial Direction */
            LATD3 = 1;                   /* turn ON LED */
            USART_SendString("RIGHT"); /* send LED ON status to terminal */
        }
        else if(data_in=='7')
        {   LATD6 = 0;              /* Initial Direction */
            LATD5 = 1;
            USART_SendString("5"); /* send LED ON status to terminal */
        }
        else if(data_in=='8')
        {   LATD6 = 1;              /* Initial Direction */
            LATD5 = 0;
        }
        MSdelay(100);
    
    }
    
}
