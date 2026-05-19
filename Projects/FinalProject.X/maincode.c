/*
;---------------------------------------------------------------
; Title: Final_Project_Aim_Trainer
;---------------------------------------------------------------
; Program Details:
;   This program uses two servos to control the vertical and horizontal angle of a
;   laser pointer. The servos are controlled by the analog output of a user-controlled
;   joystick input. There are three photoresistors that act as targets for the laser
;   to hit, and an LCD displays a message when a target gets hit and the total number of
;   targets hit. A buzzer beeps a number of times corresponding to the number of photoresistors
;   hit. There is a reset button that resets the system to allow the user to try hitting the targets again.
;
; Inputs:
;   - RA0: Joystick x-input
;   - RA1: Joystick y-input
;   - RD4: Photoresistor 1
;   - RD5: Photoresistor 2
;   - RD6: Photoresistor 3
;   - RA3: Reset button
;
; Outputs:
;   - RC3: servo1
;   - RC2: servo2
;   - PORTB, RD0, RD1: LCD
;
; Setup:
;   - PIC18F47K42 mounted on breadboard
;   - 4 direction joystick as input to control servos
;   - Servo1 that controls laser vertical angle
;   - Servo2 that controls laser horizontal angle
;   - small laser mounted on servo1
;   - LCD as output that displays
;   - Button to reset the system
;   - Three photoresistor inputs spaced apart as targets for the laser to hit
;   - 5V battery module to power servos
;
; Date:
;   May 18, 2026
;
; File Dependencies / Libraries:
;   - xc.h
;   - mcc_generated_files/system/system.h
;   - header.h
;   - functions.h
;   - stdio.h
;   - stdlib.h
;   - time.h
;   - math.h
;
; Compiler:
;   XC8 v3.10
;
; Authors:
;   Jack Bohner and Brandon McCulloch
;
; Versions:
;   V1.0
;---------------------------------------------------------------
*/


#include "mcc_generated_files/system/system.h"
#include <xc.h>
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

unsigned long PWM2_INITIALIZE_DUTY_VALUE = 46;
unsigned long PWM1_INITIALIZE_DUTY_VALUE = 46;

int duty_value2 = 46;
int duty_value1 = 46;

int xError;
int yError;
int xSpeed;
int ySpeed;

int hit_count = 0;
int hit_1 = 0;
int hit_2 = 0;
int hit_3 = 0;

uint16_t xValue;
uint16_t yValue;
uint8_t swValue;
uint8_t resetButton;

void main(void)
{
    OSCSTATbits.HFOR = 1;
    OSCFRQ = 0x02;

    // Reconfigure LCD pins AFTER MCC setup
    ANSELB = 0x00;
    ANSELD = 0x00;

    TRISB = 0x00;
    TRISDbits.TRISD0 = 0;   // LCD RS
    TRISDbits.TRISD1 = 0;   // LCD EN

    LATB = 0x00;

    // Reconfigure ADC pins AFTER MCC setup
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 1;   // RESET BUTTON input

    ANSELAbits.ANSELA0 = 1;
    ANSELAbits.ANSELA1 = 1;
    ANSELAbits.ANSELA2 = 0;
    ANSELAbits.ANSELA3 = 0; // RA3 digital input

    // Reconfigure PWM pins AFTER MCC setup
    ANSELC = 0x00;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;

    TMR2_Initialize();
    TMR2_StartTimer();

    PWM_Output_RC2_RC3_Enable();
    PWM2_Initialize();
    PWM1_Initialize();

    PWM2_LoadDutyValue(PWM2_INITIALIZE_DUTY_VALUE);
    PWM1_LoadDutyValue(PWM1_INITIALIZE_DUTY_VALUE);

    __delay_ms(500);

    LCD_Init();
    __delay_ms(100);
    LCD_String_xy(1, 0, "Total Hits: 0   ");

    ADC_Initialize();

    systemInit();

    // Startup reset indicator
    Buzzer = 1;
    __delay_ms(100);
    Buzzer = 0;
    __delay_ms(1000);

    char buffer[17];

    while(1)
    {
        xValue = ADC_ChannelSelectAndConvert(ADC_CHANNEL_ANA0);
        yValue = ADC_ChannelSelectAndConvert(ADC_CHANNEL_ANA1);
        swValue = PORTDbits.RD3;
        resetButton = PORTAbits.RA3;

        // ---------- RESET BUTTON ----------
        if(resetButton == 1)
        {
            hit_count = 0;
            hit_1 = 0;
            hit_2 = 0;
            hit_3 = 0;

            LCD_String_xy(1, 0, "Total Hits: 0   ");
            LCD_String_xy(2, 0, "COUNTS RESET    ");

            Buzzer = 1;
            __delay_ms(100);
            Buzzer = 0;

            __delay_ms(300);   // debounce delay
        }

        // ---------- X controls Servo 2 ----------
        if(xValue < 50)                  // LEFT FAST
        {
            if(duty_value2 < DUTYHIGH)
            {
                duty_value2 += FAST_SPEED;

                if(duty_value2 > DUTYHIGH)
                    duty_value2 = DUTYHIGH;

                PWM2_LoadDutyValue(duty_value2);
            }
        }
        else if(xValue < 1950)           // LEFT SLOW
        {
            if(duty_value2 < DUTYHIGH)
            {
                duty_value2 += SLOW_SPEED;

                if(duty_value2 > DUTYHIGH)
                    duty_value2 = DUTYHIGH;

                PWM2_LoadDutyValue(duty_value2);
            }
        }
        else if(xValue > 4000)           // RIGHT FAST
        {
            if(duty_value2 > DUTYLOW)
            {
                duty_value2 -= FAST_SPEED;

                if(duty_value2 < DUTYLOW)
                    duty_value2 = DUTYLOW;

                PWM2_LoadDutyValue(duty_value2);
            }
        }
        else if(xValue > 2200)           // RIGHT SLOW
        {
            if(duty_value2 > DUTYLOW)
            {
                duty_value2 -= SLOW_SPEED;

                if(duty_value2 < DUTYLOW)
                    duty_value2 = DUTYLOW;

                PWM2_LoadDutyValue(duty_value2);
            }
        }

        // ---------- Y controls Servo 1 ----------
        if(yValue < 50)                  // UP FAST
        {
            if(duty_value1 < DUTYHIGH)
            {
                duty_value1 += FAST_SPEED;

                if(duty_value1 > DUTYHIGH)
                    duty_value1 = DUTYHIGH;

                PWM1_LoadDutyValue(duty_value1);
            }
        }
        else if(yValue < 2000)           // UP SLOW
        {
            if(duty_value1 < DUTYHIGH)
            {
                duty_value1 += SLOW_SPEED;

                if(duty_value1 > DUTYHIGH)
                    duty_value1 = DUTYHIGH;

                PWM1_LoadDutyValue(duty_value1);
            }
        }
        else if(yValue > 4000)           // DOWN FAST
        {
            if(duty_value1 > DUTYLOW)
            {
                duty_value1 -= FAST_SPEED;

                if(duty_value1 < DUTYLOW)
                    duty_value1 = DUTYLOW;

                PWM1_LoadDutyValue(duty_value1);
            }
        }
        else if(yValue > 2200)           // DOWN SLOW
        {
            if(duty_value1 > DUTYLOW)
            {
                duty_value1 -= SLOW_SPEED;

                if(duty_value1 < DUTYLOW)
                    duty_value1 = DUTYLOW;

                PWM1_LoadDutyValue(duty_value1);
            }
        }

        __delay_ms(150);

        if(PR1 == 1 && hit_1 == 0)
        {
            hit_count++;
            hit_1 = 1;
            hit_Tone(hit_count);

            sprintf(buffer, "Total Hits: %d  ", hit_count);
            LCD_String_xy(1, 0, buffer);
            LCD_String_xy(2, 0, "TARGET 1 HIT!   ");

            __delay_ms(100);
        }
        else if(PR2 == 1 && hit_2 == 0)
        {
            hit_count++;
            hit_2 = 1;
            hit_Tone(hit_count);

            sprintf(buffer, "Total Hits: %d  ", hit_count);
            LCD_String_xy(1, 0, buffer);
            LCD_String_xy(2, 0, "TARGET 2 HIT!   ");

            __delay_ms(130);
        }
        else if(PR3 == 1 && hit_3 == 0)
        {
            hit_count++;
            hit_3 = 1;
            hit_Tone(hit_count);

            sprintf(buffer, "Total Hits: %d  ", hit_count);
            LCD_String_xy(1, 0, buffer);
            LCD_String_xy(2, 0, "TARGET 3 HIT!   ");

            __delay_ms(130);
        }
    }
}