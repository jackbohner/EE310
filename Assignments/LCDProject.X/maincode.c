#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// PIC18F47K42 Configuration Bits
#pragma config FEXTOSC = OFF
#pragma config RSTOSC = HFINTOSC_1MHZ

#pragma config CLKOUTEN = OFF
#pragma config PR1WAY = ON
#pragma config CSWEN = ON
#pragma config FCMEN = OFF

#pragma config MCLRE = EXTMCLR
#pragma config PWRTS = PWRT_OFF
#pragma config MVECEN = OFF
#pragma config IVT1WAY = ON
#pragma config LPBOREN = OFF
#pragma config BOREN = SBORDIS

#pragma config BORV = VBOR_2P45
#pragma config ZCD = OFF
#pragma config PPS1WAY = ON
#pragma config STVREN = ON
#pragma config DEBUG = OFF
#pragma config XINST = OFF

#pragma config WDTCPS = WDTCPS_31
#pragma config WDTE = OFF

#pragma config WDTCWS = WDTCWS_7
#pragma config WDTCCS = SC

#pragma config BBSIZE = BBSIZE_512
#pragma config BBEN = OFF
#pragma config SAFEN = OFF
#pragma config WRTAPP = OFF

#pragma config WRTB = OFF
#pragma config WRTC = OFF
#pragma config WRTD = OFF
#pragma config WRTSAF = OFF
#pragma config LVP = ON

#pragma config CP = OFF

#define _XTAL_FREQ 1000000UL

#define RS LATDbits.LATD0
#define EN LATDbits.LATD1
#define LCD_DATA LATB

#define Vref 5.0

int digital;
float voltage;
char data[16];

void ADC_Init(void);
void LCD_Init(void);
void float_to_string(float val, char *buf);
void LCD_Command(unsigned char cmd);
void LCD_Char(unsigned char dat);
void LCD_String(const char *msg);
void LCD_String_xy(unsigned char row, unsigned char pos, const char *msg);
void LCD_Clear(void);
void LCD_Print_Status(unsigned int x_adc, unsigned int y_adc, unsigned int z_adc);
unsigned int ADC_Read(unsigned char channel);

void main(void)
{
    ANSELB = 0x00;
    ANSELD = 0x00;

    TRISB = 0x00;
    TRISD = 0x00;

    LATB = 0x00;
    LATD = 0x00;

    // RD2 LED/output
    ANSELDbits.ANSELD2 = 0;
    TRISDbits.TRISD2 = 0;
    LATDbits.LATD2 = 0;

    // RC3 button input
    ANSELCbits.ANSELC3 = 0;
    TRISCbits.TRISC3 = 1;
    
    PORTC;

    // Interrupt-on-change for RC3
    IOCCPbits.IOCCP3 = 1;   // rising edge
    IOCCNbits.IOCCN3 = 1;   // falling edge too, for testing

    IOCCFbits.IOCCF3 = 0;
    PIR0bits.IOCIF = 0;

    PIE0bits.IOCIE = 1;

    INTCON0bits.IPEN = 0;   // no priority mode
    INTCON0bits.GIE = 1;    // global interrupts enabled

    ADC_Init();
    __delay_ms(500);
    LCD_Init();

    unsigned int x_adc, y_adc, z_adc;

    while(1)
    {
        x_adc = ADC_Read(0x01);
        y_adc = ADC_Read(0x02);
        z_adc = ADC_Read(0x03);

        LCD_Print_Status(x_adc, y_adc, z_adc);

        __delay_ms(150);
    }
}

void __interrupt() ISR(void)
{
    if(PIR0bits.IOCIF)
    {
        if(IOCCFbits.IOCCF3)
        {
            IOCCFbits.IOCCF3 = 0;

            for(int i = 0; i < 20; i++)
            {
                LATDbits.LATD2 = 1;
                __delay_ms(250);
                LATDbits.LATD2 = 0;
                __delay_ms(250);
            }
        }

        PIR0bits.IOCIF = 0;
    }
}

void LCD_Print_Status(unsigned int x_adc, unsigned int y_adc, unsigned int z_adc)
{
    static unsigned int last_x = 0;
    static unsigned int last_y = 0;
    static unsigned int last_z = 0;

    int dx = abs((int)x_adc - (int)last_x);
    int dy = abs((int)y_adc - (int)last_y);
    int dz = abs((int)z_adc - (int)last_z);

    last_x = x_adc;
    last_y = y_adc;
    last_z = z_adc;

    LCD_Clear();

    if(dx > 100 || dy > 100 || dz > 100)
    {
        LCD_String_xy(1, 0, "SHAKING");
    }
    else if(x_adc > 1460)
    {
        LCD_String_xy(1, 0, "TILTING LEFT");
    }
    else if(x_adc < 1360)
    {
        LCD_String_xy(1, 0, "TILTING RIGHT");
    }
    else
    {
        LCD_String_xy(1, 0, "FLAT");
    }
}

unsigned int ADC_Read(unsigned char channel)
{
    ADPCH = channel;
    __delay_ms(2);

    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);

    return ((unsigned int)ADRESH << 8) | ADRESL;
}

void LCD_Init(void)
{
    __delay_ms(500);

    RS = 0;
    EN = 0;
    LCD_DATA = 0x00;

    __delay_ms(50);

    LCD_Command(0x30);
    __delay_ms(10);

    LCD_Command(0x30);
    __delay_ms(10);

    LCD_Command(0x30);
    __delay_ms(10);

    LCD_Command(0x38);
    __delay_ms(10);

    LCD_Command(0x08);
    __delay_ms(5);

    LCD_Command(0x01);
    __delay_ms(10);

    LCD_Command(0x06);
    __delay_ms(5);

    LCD_Command(0x0C);
    __delay_ms(5);
}

void LCD_Clear(void)
{
    LCD_Command(0x01);
    __delay_ms(5);
}

void LCD_Command(unsigned char cmd)
{
    LCD_DATA = cmd;

    RS = 0;
    __delay_ms(1);

    EN = 1;
    __delay_ms(1);
    EN = 0;

    __delay_ms(3);
}

void LCD_Char(unsigned char dat)
{
    LCD_DATA = dat;

    RS = 1;
    __delay_ms(1);

    EN = 1;
    __delay_ms(1);
    EN = 0;

    __delay_ms(2);
}

void LCD_String(const char *msg)
{
    while(*msg != '\0')
    {
        LCD_Char(*msg);
        msg++;
    }
}

void LCD_String_xy(unsigned char row, unsigned char pos, const char *msg)
{
    unsigned char location;

    if(row == 1)
    {
        location = 0x80 + pos;
    }
    else
    {
        location = 0xC0 + pos;
    }

    LCD_Command(location);
    LCD_String(msg);
}

void ADC_Init(void)
{
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;

    ANSELAbits.ANSELA1 = 1;
    ANSELAbits.ANSELA2 = 1;
    ANSELAbits.ANSELA3 = 1;

    ADCON0bits.FM = 1;
    ADCON0bits.CS = 1;

    ADCLK = 0x00;
    ADPREL = 0x00;
    ADPREH = 0x00;
    ADACQL = 0x00;
    ADACQH = 0x00;

    ADCON0bits.ON = 1;
}