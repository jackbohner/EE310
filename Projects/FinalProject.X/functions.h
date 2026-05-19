
#define _XTAL_FREQ 4000000      // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4     // system clock is 1 usec
#define RS LATDbits.LATD0
#define EN LATDbits.LATD1
#define LCD_DATA LATB
#define DUTYLOW      16
#define DUTYCENTER   46
#define DUTYHIGH     76
#define PR1 PORTDbits.RD4
#define PR2 PORTDbits.RD5
#define PR3 PORTDbits.RD6

#define Buzzer LATAbits.LATA2

#define SLOW_SPEED   1
#define FAST_SPEED   3

void LCD_Init(void);
void LCD_Command(unsigned char cmd);
void LCD_Char(unsigned char dat);
void LCD_String(const char *msg);
void LCD_String_xy(unsigned char row, unsigned char pos, const char *msg);
void LCD_Clear(void);
void hit_Tone(int);

void hit_Tone(int hit_count) {
            int count = 0;
            while (count < hit_count) {
                Buzzer = 1;
                __delay_ms(200);
                Buzzer = 0;
                __delay_ms(200);
                count++;
            }
        }

void systemInit(void) {
            ANSELD = 0x00;
            ANSELAbits.ANSELA2 = 0; //Make Buzzer Digital
            TRISDbits.TRISD4 = 1; // Photo Resistor 1
            TRISDbits.TRISD5 = 1; // Photo Resistor 2
            TRISDbits.TRISD6 = 1; // Photo Resistor 3
            TRISAbits.TRISA2 = 0; // Buzzer

            WPUDbits.WPUD4 = 1;
            WPUDbits.WPUD5 = 1;
            WPUDbits.WPUD6 = 1;

            PORTAbits.RA2 = 0;


        }

float convert_to_degrees(int dutyvalue){
    float temp = dutyvalue - 16;
    return temp * 3.0;
}
void delay_fast(void){
    __delay_ms(100);
}
void delay_slow(void){
    __delay_ms(200);
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
void TMR2_Initialize(void)
{
    // Set TMR2 to the options selected in the User Interface

    // T2CS FOSC/4; 
    T2CLKCON = 0x01;

    // T2PSYNC Not Synchronized; T2MODE Software control; T2CKPOL Rising Edge; T2CKSYNC Not Synchronized; 
    T2HLT = 0x00;

    // T2RSEL T2CKIPPS pin; 
    T2RST = 0x00;

    // PR2 156; 
    T2PR = 0x9C;

    // TMR2 0; 
    T2TMR = 0x00;

    // Clearing IF flag.
    PIR4bits.TMR2IF = 0;

    // T2CKPS 1:128; T2OUTPS 1:1; TMR2ON on; 
    T2CONbits.ON = 1;
    T2CONbits.CKPS = 0b111; //1:128 prescale for period > 20ms
    T2CONbits.OUTPS = 0x00;
}

//void TMR2_ModeSet(TMR2_HLT_MODE mode)
//{
//   // Configure different types HLT mode
//    T2HLTbits.MODE = mode;
//
//
//void TMR2_ExtResetSourceSet(TMR2_HLT_EXT_RESET_SOURCE reset)
//{
//    //Configure different types of HLT external reset source
//    T2RSTbits.RSEL = reset;
//}

void TMR2_Start(void)
{
    // Start the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 1;
}

void TMR2_StartTimer(void)
{
    TMR2_Start();
}

void TMR2_Stop(void)
{
    // Stop the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 0;
}

void TMR2_StopTimer(void)
{
    TMR2_Stop();
}

uint8_t TMR2_Counter8BitGet(void)
{
    uint8_t readVal;

    readVal = TMR2;

    return readVal;
}

uint8_t TMR2_ReadTimer(void)
{
    return TMR2_Counter8BitGet();
}

void TMR2_Counter8BitSet(uint8_t timerVal)
{
    // Write to the Timer2 register
    TMR2 = timerVal;
}

void TMR2_WriteTimer(uint8_t timerVal)
{
    TMR2_Counter8BitSet(timerVal);
}

void TMR2_Period8BitSet(uint8_t periodVal)
{
   PR2 = periodVal;
}

void TMR2_LoadPeriodRegister(uint8_t periodVal)
{
   TMR2_Period8BitSet(periodVal);
}

//bool TMR2_HasOverflowOccured(void)
//{
//    // check if  overflow has occurred by checking the TMRIF bit
//    bool status = PIR4bits.TMR2IF;
//    if(status)
//    {
//        // Clearing IF flag.
//        PIR4bits.TMR2IF = 0;
//    }
//    return status;
//}



///////////// END OF TIMER 


void PWM_Output_RC2_RC3_Enable(void)
{
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0;

    RC2PPS = 0x0A;   // CCP2 -> RC2
    RC3PPS = 0x09;   // CCP1 -> RC3, example only

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 1;

    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;
}



void PWM1_Initialize(void)
{
    CCP1CON = 0x8C;          // PWM enabled, same style as CCP2
    CCPR1H = 0x00;
    CCPR1L = 0x0F;

    CCPTMRS0bits.C1TSEL = 0x1;   // PWM1 uses Timer2
}

void PWM2_Initialize(void)
{
    CCP2CON = 0x8C;
    CCPR2H = 0x00;
    CCPR2L = 0x0F;

    CCPTMRS0bits.C2TSEL = 0x1;   // PWM2 uses Timer2
}
void PWM1_LoadDutyValue(uint16_t dutyValue)
{
    dutyValue &= 0x03FF;

    if(CCP1CONbits.FMT)
    {
        dutyValue <<= 6;
        CCPR1H = dutyValue >> 8;
        CCPR1L = dutyValue;
    }
    else
    {
        CCPR1H = dutyValue >> 8;
        CCPR1L = dutyValue;
    }
}

void PWM2_LoadDutyValue(uint16_t dutyValue)
{
    dutyValue &= 0x03FF;

    if(CCP2CONbits.FMT)
    {
        dutyValue <<= 6;
        CCPR2H = dutyValue >> 8;
        CCPR2L = dutyValue;
    }
    else
    {
        CCPR2H = dutyValue >> 8;
        CCPR2L = dutyValue;
    }
}

 _Bool PWM2_OutputStatusGet(void)
{
    // Returns the output status
    return(CCP2CONbits.OUT);
}
 
