
#include <xc.h>

#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


#include <stdio.h>
#include <stdlib.h>
#define _XTAL_FREQ 8000000

#define NOBT 0  
#define BTMODE1 1     
#define BTMODE2 2     
#define BTMODE3 3     

#define TIMER0VALUE 6 // count from 6 to 256 = 250 count x 8 us = 2000 us per interrupt.
#define TIMER1HVALUE 255
#define TIMER1LVALUE 50


#define MODE1GREENTIME1 20 //Tgian den xanh Huong 1 che do binh thuong
#define MODE1GREENTIME2 20 //Tgian den xanh Huong 2 che do binh thuong

#define MODE2GREENTIME1 30 //Tgian den xanh Huong 1 che do cao diem
#define MODE2GREENTIME2 30 //Tgian den xanh Huong 2 che do cao diem


#define YELLOWTIME 3 //Tgian den vang o cac chu ki


#define Green2A PORTBbits.RB1
#define Red2B PORTBbits.RB2
#define Yellow2B PORTBbits.RB5
#define Green2B PORTBbits.RB4 

#define ON 0
#define OFF 1


#define GREEN1 0
#define YELLOW1 1
#define GREEN2 2
#define YELLOW2 3

char LEDCode[12]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};

int Timer0Count=0;
int Timer1Count=0;

char CurrentLED=0;
unsigned char LEDData[8];

unsigned char Phase1GreenTime=MODE1GREENTIME1;
unsigned char Phase2GreenTime=MODE1GREENTIME2;


unsigned char TimeCount1=0; // Line 1
unsigned char TimeCount2=0; // Line 2


unsigned char CurrentState;

unsigned char NightMode=OFF; // Che do ban dem
unsigned char NightLight=OFF; // Den ban dem


void SetLightPhase1Green() 
{
    PORTA=0xFB;
    PORTE=0xFC;
    
    Green2A=OFF;
    Red2B=ON;
    Yellow2B=OFF;
    Green2B=OFF;

    
}

void SetLightPhase1Yellow() 
{
    PORTA=0xDD;
    PORTE=0xFD;
    
    Green2A=OFF;
    Red2B=ON;
    Yellow2B=OFF;
    Green2B=OFF;
}

void SetLightPhase2Green() 
{
   
    PORTA=0xF6;
    PORTE=0xFF;
    
    Green2A=ON;
    Red2B=OFF;
    Yellow2B=OFF;
    Green2B=ON;
    
}

void SetLightPhase2Yellow() 
{
  
    
    PORTA=0xF6;
    PORTE=0xF3;
    
    Green2A=OFF;
    Red2B=OFF;
    Yellow2B=ON;
    Green2B=OFF;
    
}




unsigned char CheckButton() // Ham kiem tra nut nhan
{
  if(PORTBbits.RB0==0)
  {
    __delay_ms(250);
    return BTMODE1;

  }
 
  if(PORTBbits.RB6==0)
  {
    __delay_ms(250);
    return BTMODE2;

  }
  if(PORTBbits.RB7==0)
  {
    __delay_ms(250);
    return BTMODE3;

  }

   return NOBT;


}

void ScanLED()
{
    CurrentLED++;
    if(CurrentLED==8) CurrentLED=0;
    PORTC=0xFF; // Off ALL LED
    PORTD=LEDCode[LEDData[CurrentLED]];
    if(CurrentLED==0) RC0=0; 
    else if(CurrentLED==1) RC1=0; 
    else if(CurrentLED==2) RC2=0; 
    else if(CurrentLED==3) RC3=0; 
    else if(CurrentLED==4) RC4=0; 
    else if(CurrentLED==5) RC5=0; 
    else if(CurrentLED==6) RC6=0; 
    else if(CurrentLED==7) RC7=0;
   
}

void HienThi()
{
    LEDData[0]=TimeCount1/10;
    LEDData[1]=TimeCount1%10;
    
    LEDData[2]=TimeCount1/10;
    LEDData[3]=TimeCount1%10;
    
    LEDData[4]=TimeCount2/10;
    LEDData[5]=TimeCount2%10;
    
    LEDData[6]=TimeCount2/10;
    LEDData[7]=TimeCount2%10;
    
    
}

void SetTimePhase1Green()
{
    TimeCount1=Phase1GreenTime;
    TimeCount2=Phase1GreenTime+YELLOWTIME;
   
}

void SetTimePhase1Yellow()
{
    TimeCount1=YELLOWTIME;
    TimeCount2=YELLOWTIME;

}

void SetTimePhase2Green()
{
    TimeCount1=Phase2GreenTime+YELLOWTIME;
    TimeCount2=Phase2GreenTime;

}

void SetTimePhase2Yellow()
{
    TimeCount1=YELLOWTIME;
    TimeCount2=YELLOWTIME;
   
}


//Blink led vang o che do ban dem

void OnYellow()
{
    
    PORTA=0xDD;
    PORTE=0xFB;
    
    Green2A=OFF;
    Red2B=OFF;
    Yellow2B=ON;
    Green2B=OFF;
}

void OffYellow()
{
    
    PORTA=0xFF;
    PORTE=0xFF;
    
    Green2A=OFF;
    Red2B=OFF;
    Yellow2B=OFF;
    Green2B=OFF;
}



void InitTimer0() // time count second
{
    OPTION_REGbits.T0CS=0; // internal clock for timer 0
    OPTION_REGbits.PSA = 0; // prescal for timer 0
    OPTION_REGbits.PS2=0;  // prescal 1:16 Clock = FOSC /4 and / prescal/ 8Mhz/4/16 = 0.125Mhz -> 8 us per timer count
    OPTION_REGbits.PS1=1;
    OPTION_REGbits.PS0=1;
    TMR0=TIMER0VALUE;// count from 6 to 255 = 250 count x 8 us = 2000 us = 2ms per interrupt.
    INTCONbits.TMR0IF=0; // clear flag
    INTCONbits.TMR0IE=1; // enable timer0 interrupt
    INTCONbits.PEIE=1; // enable preha interrupt
    INTCONbits.GIE=1; // enable global interrupt
}

void InitTimer1() //timer scan 7SEG LED
{
    T1CONbits.TMR1CS=0;
    T1CONbits.T1CKPS1=1; //prescal 1:8
    T1CONbits.T1CKPS0=1;
    PIE1bits.TMR1IE=1; //enable Timer1 interrupt
    INTCONbits.PEIE=1; // enable preha interrupt
    INTCONbits.GIE=1; // enable global interrupt
    PIR1bits.TMR1IF=0; // clear flag
    TMR1L=TIMER1LVALUE; // preload value
    TMR1H=TIMER1HVALUE; // preload value
    T1CONbits.TMR1ON=1;// start timer 1
}

void __interrupt() ISR()
{ 

    if(INTCONbits.TMR0IF==1) //if interrupt timer 0
    {
        INTCONbits.TMR0IF=0;// clear flag
        TMR0=TIMER0VALUE; // preload value
        Timer0Count++;
         
        if(Timer0Count>=500) // 500x2=1000ms = 1s for time count
        {
            Timer0Count=0;
            // 1 second action
            
            if(NightMode==OFF)
            {
                        TimeCount1--;
                        TimeCount2--;

                        if(CurrentState==GREEN1&&TimeCount1==0)
                        {
                            SetTimePhase1Yellow();
                            CurrentState=YELLOW1;
                            SetLightPhase1Yellow();
                        }
                        if(CurrentState==YELLOW1&&TimeCount1==0)
                        {
                            SetTimePhase2Green();
                            CurrentState=GREEN2;
                            SetLightPhase2Green();
                        }
                        if(CurrentState==GREEN2&&TimeCount2==0)
                        {
                            SetTimePhase2Yellow();
                            CurrentState=YELLOW2;
                            SetLightPhase2Yellow();
                        }

                         if(CurrentState==YELLOW2&&TimeCount2==0)
                        {
                            SetTimePhase1Green();
                            CurrentState=GREEN1;
                            SetLightPhase1Green();
                        }

                        HienThi();
            }
           
            else
           {
               if(NightLight==OFF) 
                {
                    NightLight=ON;
                    OnYellow();
                    LEDData[0]=10;
                    LEDData[1]=10;
                    LEDData[2]=10;
                    LEDData[3]=10;
                    LEDData[4]=10;
                    LEDData[5]=10;
                    LEDData[6]=10;
                    LEDData[7]=10;
                }
                else
                {
                    NightLight=OFF;
                    OffYellow();
                    LEDData[0]=11;
                    LEDData[1]=11;
                    LEDData[2]=11;
                    LEDData[3]=11;
                    LEDData[4]=11;
                    LEDData[5]=11;
                    LEDData[6]=11;
                    LEDData[7]=11;
                }
                 
            }
        }    
        
    }
    
    
    if(PIR1bits.TMR1IF==1) //if interrupt timer 1
    {
        PIR1bits.TMR1IF=0;// clear flag
        TMR1L=TIMER1LVALUE; // preload value
        TMR1H=TIMER1HVALUE; // preload value
        Timer1Count++;
         
        if(Timer1Count>=5) //
        {
            Timer1Count=0;
            ScanLED(); 
        }
        
        
    }
}




void main(void) {

    unsigned char Button=NOBT;
    OPTION_REGbits.nRBPU=0;// enable pull up resistor
    ADCON1 = 0x0F;// disable analog on port A
    CMCON = 7; // disable comparator
    TRISD=0x00;
    TRISC=0x00;
    TRISB=0xC1;
    TRISA=0x00;
    TRISE=0x00;
    
    InitTimer1();
    
    // Test LED
    LEDData[0]=8;
    LEDData[1]=8;
    LEDData[2]=8;
    LEDData[3]=8;
    LEDData[4]=8;
    LEDData[5]=8;
    LEDData[6]=8;
    LEDData[7]=8;
    PORTA=0x00;
    PORTE=0x00;
    PORTB=0x00;
    
    __delay_ms(2000);
    SetLightPhase1Green();
    SetTimePhase1Green();
    HienThi();
    CurrentState=GREEN1;
    InitTimer0();
    while(1)
    {
        Button=CheckButton();
        if(Button==BTMODE1)
        {   NightMode=OFF;
            INTCONbits.TMR0IE=0; // disable timer0 interrupt
            Phase1GreenTime=MODE1GREENTIME1;
            Phase2GreenTime=MODE1GREENTIME2;
            SetLightPhase1Green();
            SetTimePhase1Green();
            HienThi();
            CurrentState=GREEN1;
            LEDData[0]=10; // display --
            LEDData[1]=10;
            LEDData[2]=10;
            LEDData[3]=10;
            LEDData[4]=10;
            LEDData[5]=10;
            LEDData[6]=10;
            LEDData[7]=10;
            __delay_ms(1000);
            Timer0Count;
            INTCONbits.TMR0IE=1; // enable timer0 interrupt
        }
        else if(Button==BTMODE2)
        {
            NightMode=OFF;
            INTCONbits.TMR0IE=0; // disable timer0 interrupt
            Phase1GreenTime=MODE2GREENTIME1;
            Phase2GreenTime=MODE2GREENTIME2;
            SetLightPhase1Green();
            SetTimePhase1Green();
            HienThi();
            CurrentState=GREEN1;
            LEDData[0]=10; // display --
            LEDData[1]=10;
            LEDData[2]=10;
            LEDData[3]=10;
            LEDData[4]=10;
            LEDData[5]=10;
            LEDData[6]=10;
            LEDData[7]=10;
            __delay_ms(1000);
            Timer0Count;
            INTCONbits.TMR0IE=1; // enable timer0 interrupt
        }
        
        else if(Button==BTMODE3)
        {
            NightMode=ON;
        }
     }
    
    return;
}
