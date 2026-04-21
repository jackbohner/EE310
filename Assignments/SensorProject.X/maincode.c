/*
;---------------------------------------------------------------
; Title: Assignment_7_Interfacing_With_Sensors
;---------------------------------------------------------------
; Program Details:
;   This program takes an input of two photoresistors. After entering a code
;   on the first photoresistor by covering it a number of times, a button is pressed to
;   move to entering a code on the next photoresistor and the status LED lights up to indicate. 
;   the button has been pressed. After the next code is entered, the same button is pressed 
;   again to store the values entered on both resistors and check it against the secret code.
;
;   If the secret code matches the numbers entered on the photoresistors, the relay circuit is
;   activated, and turns on a motor for 1 second. If the secret code doesn't match the entered
;   numbers, a buzzer turns on for a short period to indicate an incorrect answer. The stored numbers
;   corresponding to each photoresistor are reset, and the first photoresistor is ready to read values
;
;   While the photoresistors are reading inputs from the user, a seven segment display displays
;   the value currently stored for the active photoresistor and resets when the button is pressed
;   to switch photoresistors.
;
;   A seperate button is used as an emergency switch. When pressed, a distinct melody is played on the
;   buzzer
;
; Inputs:
;   - RC2 : Photoresistor 1, first digit input
;   - RC3 : Photoresistor 2, second digit input
;   - RC4 : Pushbutton used to store the entered digits and switch between photoresistors
;   - RB0 : Interrupt Button
;
; Outputs:
;   - PORTD : 7-segment display output
;   - RA1   : Status LED
;   - RA3   : Buzzer output
;   - RA4   : Relay driver
;
; Setup:
;   - PIC18F47K42 mounted on breadboard
;   - Two photoresistors used as digital inputs
;   - One pushbutton connected to external interrupt
;   - One pushbutton used to switch between reading each photoresistor and to confirm code entry
;   - One 7-segment display connected to PORTD
;   - One relay to drive a motor circuit
;   - One buzzer output
;
; Date:
;   April 18, 2028
;
; File Dependencies / Libraries:
;   - xc.h
;   - config.h
;   - define.h
;   - functions.h
;   - XC8_ConfigFile.h
;   - Project header/source files as required
;
; Compiler:
;   XC8 v2.40
;
; Author:
;   Jack Bohner
;
; Versions:
;   V1.0 : Original version
;---------------------------------------------------------------
*/
#include <xc.h>
#include "Myheader.h"
#include "define.h"
#include "functions.h"

void main (void)
{
    initializePorts();
    int sdigit1 = 1;
    int sdigit2 = 1;
    int toggle = 0;
    int scheck1 = 0;
    int scheck2 = 0;


    while(1){
        if (emergency_flag){
            scheck1 = 0;
            scheck2 = 0;
            toggle = 0;
            emergency_flag = 0;
        }
        photoresistorCheck1(toggle, &scheck1);
        if (toggle == 0){
            update_seven_segment(scheck1);  
        }
        photoresistorCheck2(toggle, &scheck2);
        buttonPress(&toggle, &scheck1, &scheck2, sdigit1, sdigit2);
        if (toggle == 1){
            update_seven_segment(scheck2);
        }
    }
}