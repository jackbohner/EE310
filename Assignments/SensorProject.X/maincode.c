#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
        if (melody_flag){
            play_melody();
            melody_flag = 0;
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