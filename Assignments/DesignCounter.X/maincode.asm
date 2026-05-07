#include <xc.inc>
#include "AsmHeader.inc"

        PROCESSOR 18F47K42

        PSECT resetVec,class=CODE,reloc=2
        ORG 0x00
resetVec:
        GOTO START

        PSECT code

COUNT   EQU 0x20
SW      EQU 0x21
D1      EQU 0x22
D2      EQU 0x23
D3      EQU 0x24
DB1     EQU 0x25
DB2     EQU 0x26

SEG0    EQU 0x30

START:
        BANKSEL ANSELA	;set as digital
        CLRF ANSELA
        BANKSEL ANSELD	;set as digital
        CLRF ANSELD

        BANKSEL WPUA
        CLRF WPUA

        BANKSEL ODCONA
        CLRF ODCONA
        BANKSEL ODCOND
        CLRF ODCOND

        BANKSEL LATA
        CLRF LATA
        BANKSEL LATD
        CLRF LATD

        ; RA0 = Button A input
        ; RA1 = Button B input
        BANKSEL TRISA	;PORTA input
        MOVLW 0x03
        MOVWF TRISA

        ; PORTD = 7-segment output
        BANKSEL TRISD	;PORTD output
        CLRF TRISD

        CLRF COUNT
        CALL INIT_TABLE
        CALL DISPLAY

MAIN:
        CALL READ_SWITCHES
        
	MOVLW 0x00  ; 00 = no button, hold last value
        CPFSEQ SW   ;go to reset unless no buttons were pressed
        GOTO CHECK_RESET
        GOTO MAIN

CHECK_RESET:
        ; 11 = both buttons, reset to 0
        MOVLW 0x03
        CPFSEQ SW   ;go to check a unless both buttons pressed
        GOTO CHECK_A
	
	;reset display
        CLRF COUNT
        CALL DISPLAY
        CALL STEP_DELAY
        GOTO MAIN

CHECK_A:
        ; 01 = A only, count up
        MOVLW 0x01
        CPFSEQ SW   ;go to check b unless only button a was pressed
        GOTO CHECK_B

        INCF COUNT, F
        MOVLW 0x0F
        ANDWF COUNT, F	;wrap count
        CALL DISPLAY	;display number
        CALL STEP_DELAY
        GOTO MAIN

CHECK_B:
        ; 10 = B only, count down
        MOVLW 0x02
        CPFSEQ SW   ;go to main unless only button b was pressed
        GOTO MAIN

        DECF COUNT, F
        MOVLW 0x0F
        ANDWF COUNT, F	;wrap count
        CALL DISPLAY	;display number
        CALL STEP_DELAY
        GOTO MAIN


; READ_SWITCHES with debounce
; active-HIGH:
; RA0 = A
; RA1 = B
READ_SWITCHES:
        BANKSEL PORTA
        MOVF PORTA, W
        ANDLW 0x03  ;keep only RA1 and RA0
        MOVWF SW    ;store 00,01,10,or 11

        CALL DEBOUNCE_DELAY

        BANKSEL PORTA
        MOVF PORTA, W
        ANDLW 0x03

        CPFSEQ SW   ;skip unless both buttons pressed
        GOTO READ_SWITCHES

        RETURN


; DISPLAY using indirect addressing
DISPLAY:
        MOVLW LOW SEG0
        MOVWF FSR0L ;set pointer FSR0 to beginning of 7 segment table
        CLRF FSR0H

        MOVF COUNT, W
        MOVF PLUSW0, W	;read table value at (beginning of table) + count

        BANKSEL LATD
        MOVWF LATD  ;output read value to 7 segment
        RETURN


; Initialize table for 7 segment
INIT_TABLE:
        MOVLW LOW SEG0
        MOVWF FSR0L
        CLRF FSR0H

        MOVLW 0x3F
        MOVWF POSTINC0
        MOVLW 0x06
        MOVWF POSTINC0
        MOVLW 0x5B
        MOVWF POSTINC0
        MOVLW 0x4F
        MOVWF POSTINC0
        MOVLW 0x66
        MOVWF POSTINC0
        MOVLW 0x6D
        MOVWF POSTINC0
        MOVLW 0x7D
        MOVWF POSTINC0
        MOVLW 0x07
        MOVWF POSTINC0
        MOVLW 0x7F
        MOVWF POSTINC0
        MOVLW 0x6F
        MOVWF POSTINC0
        MOVLW 0x77
        MOVWF POSTINC0
        MOVLW 0x7C
        MOVWF POSTINC0
        MOVLW 0x39
        MOVWF POSTINC0
        MOVLW 0x5E
        MOVWF POSTINC0
        MOVLW 0x79
        MOVWF POSTINC0
        MOVLW 0x71
        MOVWF POSTINC0

        RETURN


;delay function
STEP_DELAY:
        MOVLW 0x40
        MOVWF D1

SD1:
        MOVLW 0xFF
        MOVWF D2

SD2:
        MOVLW 0xFF
        MOVWF D3

SD3:
        DECFSZ D3, F
        GOTO SD3
        DECFSZ D2, F
        GOTO SD2
        DECFSZ D1, F
        GOTO SD1

        RETURN


; short debounce delay

DEBOUNCE_DELAY:
        MOVLW 0x40
        MOVWF DB1

DB_LOOP1:
        MOVLW 0xFF
        MOVWF DB2

DB_LOOP2:
        DECFSZ DB2, F
        GOTO DB_LOOP2

        DECFSZ DB1, F
        GOTO DB_LOOP1

        RETURN

        END