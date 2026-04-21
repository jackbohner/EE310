#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4

void main (void)
{
    ANSELB = 0;
    ANSELD = 0;
    TRISB = 0b00000000;
    PORTB = 0b00000000;
    TRISD = 0b00000011;
    
    while(1){
        if (PORTDbits.RD1 == 1){
            LATBbits.LATB1 = 1;
            while(PORTDbits.RD1 == 1){
                __delay_ms(10);
            }
            LATBbits.LATB1 = 0;
        }
    }
}

