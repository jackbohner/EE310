/*
* The purpose of this program is to generate a PWM with different pulse width 
* The output of the PWM will be on RB2
* myLED is connected to RB0 and it toggles very slowly
* In order to change the PULSE period and width you need to do the following: 

 * PWM Period=
 * [T2PR+1]*4*Tosc*PreScale

 * Pulse Width=
 * Tosc*Prescale*CCPR2

 * Duty Cycle Ratio %=
 * CCPR2 / [4*(T2PR+1)]

 * The presale value for the timer is defined in T2CON register. 
 * The ACTUAL value of CCP2 MUST be varied by changing
 * PWM2_INITIALIZE_DUTY_VALUE set to the equivalent decimal value for CCPR2

 * Author: Farid Farahmand
 */




#include <xc.h> // must have this
#include "header.h" // must have this
#include "config.h" // must have this -  XC8_ConfigFile.h
//#include "C:\Program Files\Microchip\xc8\v2.40\pic\include\proc\pic18f46k42"


#define _XTAL_FREQ 4000000      // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4     // system clock is 1 usec
#define BUTTONleft PORTDbits.RD0
#define BUTTONright PORTDbits.RD1

#define myLED  PORTBbits.RB0
unsigned long PWM2_INITIALIZE_DUTY_VALUE = 47;
int duty_value = 47;

uint16_t checkdutyCycle;
char preScale;
_Bool pwmStatus;

void main (void) {
    OSCSTATbits.HFOR =1; // enable  HFINTOSC Oscillator (see clock schematic))
    OSCFRQ=0x02; // 00=1 MHZ, 02=4MHZ internal - see page 106 of data sheet
    
    ANSELB = 0b00000000;    
    TRISB= 0b00000000;//sets PORTB as all outputs 
    PORTB= 0b00000000;//turns off PORTB outputs so that the LED is initially off
    TMR2_Initialize();
    TMR2_StartTimer();
    ANSELD = 0x00;
    TRISD = 0b00000011;
    
    PWM_Output_D8_Enable();
    PWM2_Initialize();
    PWM2_LoadDutyValue(PWM2_INITIALIZE_DUTY_VALUE ); // initialize CCPR2H/L
   // PWM_Output_D8_Disable();
   // TMR2_StopTimer();  

    // Duty Cycle in percentage 
    checkdutyCycle =(uint16_t)((100UL*PWM2_INITIALIZE_DUTY_VALUE)/(4*(T2PR+1)));
    // binary value of Register T2CON.PRESCALE
    preScale = ((T2CON >> 4) & (0x0F)); 
    
    while (1) {
        pwmStatus = PWM2_OutputStatusGet();
        PORTBbits.RB2 = pwmStatus;
        if (BUTTONright){
            if (duty_value > 16){
                duty_value--;
            }
            PWM2_LoadDutyValue(duty_value);
            __delay_ms(70);
        }
        if (BUTTONleft){
            if (duty_value < 70){
                duty_value++;
            }
            PWM2_LoadDutyValue(duty_value);
            __delay_ms(70);
        }
    }
}