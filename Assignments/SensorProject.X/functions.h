#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Myheader.h"
#include "define.h"

unsigned char emergency_flag = 0;

void photoresistorCheck1(int toggle, int *scheck1){
if (light_switch1 == 1 && toggle == 0){
            *scheck1 = *scheck1 + 1;
            while(light_switch1 == 1){
                __delay_ms(10);
            }
        }
        else{
            
        }
}

void photoresistorCheck2(int toggle, int *scheck2){
            if (light_switch2 == 1 && toggle == 1){
            *scheck2 = *scheck2 + 1;
            while(light_switch2 == 1){
                __delay_ms(10);
            }
        }
        else{
        }
}

void buttonPress(int *toggle, int *scheck1, int *scheck2, int sdigit1, int sdigit2){
        if (button_store == 1){
            
            if (*scheck1 == sdigit1 && *scheck2 == sdigit2 && *toggle == 1){
                relay = 1;
                __delay_ms(1000);
                relay = 0;
                *scheck1 = 0;
                *scheck2 = 0;
                *toggle = 0;
            }
            else if(*toggle == 1){
                *scheck1 = 0;
                *scheck2 = 0;
                *toggle = 0;
                led_test = 1;
                __delay_ms(200);
                led_test = 0;
                buzzer = 1;
                __delay_ms(800);
                buzzer = 0;
            }
            else {
                led_test = 1;
                __delay_ms(200);
                led_test = 0;
                *toggle = 1;
            }
            while(button_store == 1){
                __delay_ms(10);
            }
        }
        else{
        }
}
void initializePorts(){
    ANSELB = 0x00;
    ANSELA = 0x00;
    ANSELD = 0x00;
    ANSELC = 0x00;
    TRISA = 0b00000000;
    PORTA = 0b00000000;
    TRISB = 0b00000001;
    TRISC = 0b00011100;
    TRISD = 0b00000000;
    PORTD = 0b00000000;
    INTCON0bits.INT0EDG = 1;   // rising edge

    PIR1bits.INT0IF = 0;       // clear flag
    PIE1bits.INT0IE = 1;       // enable INT0 interrupt

    INTCON0bits.GIE = 1;       // global interrupt enable
}

void update_seven_segment(int display_number){
    int seven_segment_array[] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};
    PORTD = seven_segment_array[display_number];
}

void play_melody(){
    for (int i = 0; i < 4; i++){
        buzzer = 1;
        __delay_ms(100);
        buzzer = 0;
        __delay_ms(100);
    }
}

void __interrupt(irq(IRQ_INT0), base(8)) INT0_ISR(void)
{
    emergency_flag = 1;
    play_melody();
    PIR1bits.INT0IF = 0;    // clear interrupt
}
