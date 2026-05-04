/*
;---------------------------------------------------------------
; Title: Assignment: Analog to Digital Converters
;---------------------------------------------------------------
; Program Details:
; This program compares a measured temperature from a temperature sensor input to an input
; reference temperature, then turns on a cooling device or a heating device depending on if
; the measured temperature is higher or lower than the reference temperature.
; Inputs: temperature sensor, keypad
;
; Outputs:
;   - PORTD 2 : Cooling system
;   - PORTD 1 : Heating system
;
; Date:
;   May 1, 2028
;
; Compiler:
;   pic-as v3.10
;
; Author:
;   Jack Bohner
;
; Versions:
;   V1.0 : Original version
;---------------------------------------------------------------
*/
#include <xc.inc>
#include "myheader.inc"
;----------------
; PROGRAM INPUTS
;----------------
;The DEFINE directive is used to create macros or symbolic names for values.
;It is more flexible and can be used to define complex expressions or sequences of instructions.
;It is processed by the preprocessor before the assembly begins.

#define  measuredTempInput     -5 ; this is the input value
#define  refTempInput         15 ; this is the input value

;---------------------
; Definitions
;---------------------
#define SWITCH    LATD,2  
#define LED0      PORTD,0
#define LED1      PORTD,1

    
 
;---------------------
; Program Constants
;---------------------
; The EQU (Equals) directive is used to assign a constant value to a symbolic name or label.
; It is simpler and is typically used for straightforward assignments.
;It directly substitutes the defined value into the code during the assembly process.
    
REG10   equ     10h   ;in HEX
REG11   equ     11h
REG01   equ     1h
contReg    equ    0x22
    
    PSECT absdata,abs,ovrld
   
    ORG    0x20    ;start program at 0x20 memory location
   
    CLRF    TRISD
    CLRF    PORTD
    MOVLW   measuredTempInput
    MOVWF   0x21
    MOVLW   refTempInput
    MOVWF   0x20
; Convert refTemp at 0x20 to decimal digits in 0x60-0x62
        MOVF    0x20, 0
        MOVWF   0x23        ; temp copy

        CLRF    0x60        ; clear ones
        CLRF    0x61        ; clear tens
        CLRF    0x62        ; clear hundreds

REFERENCE_HUND:
        MOVLW   100
        SUBWF   0x23, 0
        BTFSS   STATUS, 0   ;check if ref is more than 100
        GOTO    REFERENCE_TENS    ;continues to tens if ref is less than 100
        MOVWF   0x23
        INCF    0x62, F    ;increase hundreds count
        GOTO    REFERENCE_HUND

REFERENCE_TENS:
        MOVLW   10
        SUBWF   0x23, 0
        BTFSS   STATUS, 0   ;check if ref is more than 10
        GOTO    REFERENCE_ONES    ;continues to ones if ref is less than 10
        MOVWF   0x23
        INCF    0x61, F    ;increase tens count
        GOTO    REFERENCE_TENS

REFERENCE_ONES:
        MOVF    0x23, 0
        MOVWF   0x60        ; store ones digit


; Convert measuredTemp at 0x21 to decimal digits in 0x70-0x72
        MOVF    0x21, 0
        MOVWF   0x23        ; temp copy

        BTFSS   0x23, 7     ; check if negative
        GOTO    MEASURED_POS
        COMF    0x23, F
        INCF    0x23, F     ; take absolute value if negative
MEASURED_POS:
        CLRF    0x70        ; clear ones
        CLRF    0x71        ; clear tens
        CLRF    0x72        ; clear hundreds
MEASURED_HUND:
        MOVLW   100
        SUBWF   0x23, 0
        BTFSS   STATUS, 0   ;check if measure is more than 100
        GOTO    MEASURED_TENS    ;continues if measure is less than 100
        MOVWF   0x23
        INCF    0x72, F    ;increment hundreds count
        GOTO    MEASURED_HUND
MEASURED_TENS:
        MOVLW   10
        SUBWF   0x23, 0
        BTFSS   STATUS, 0   ;check if measure is more than 10
        GOTO    MEASURED_ONES    ;continues if measure is less than 10
        MOVWF   0x23
        INCF    0x71, F    ;incremenet tens count
        GOTO    MEASURED_TENS
MEASURED_ONES:
        MOVF    0x23, 0
        MOVWF   0x70        ; store ones digit
    
LOOP:
    MOVF    0x21, 0 ;move measured temp to WREG
    SUBWF   0x20, 0 ;subtract measured temp from ref temp
    BTFSC   STATUS, 2    ;check if ref temp and measured temp were equal
    GOTO    LED_OFF
    BTFSS   STATUS, 0    ;check if ref temp was less than measured temp
    GOTO    LED_HOT
    GOTO    LED_COOL

LED_HOT:
        MOVLW   0x02
        MOVWF   0x22    ;contReg = 2
	MOVLW	0x02
	MOVWF	PORTD    ;turn on hot LED
        GOTO    LOOP

LED_COOL:
        MOVLW   0x01
        MOVWF   0x22    ;contReg = 1
	MOVLW	0x04
	MOVWF	PORTD    ;turn on cool LED
        GOTO    LOOP

LED_OFF:
        CLRF    0x22    ;contReg = 0
	MOVLW	0x00
	MOVWF	PORTD    ;turn off LEDs
        GOTO    LOOP

    
    END





