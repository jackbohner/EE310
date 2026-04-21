#include "Myheader.h"
#include <xc.h>

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4


#define light_switch1 PORTCbits.RC2
#define light_switch2 PORTCbits.RC3
#define button_store PORTCbits.RC4
#define led_test PORTAbits.RA1
#define buzzer PORTAbits.RA3
#define relay PORTAbits.RA4