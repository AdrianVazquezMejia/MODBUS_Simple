/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system intialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.95-b-SNAPSHOT
        Device            :  PIC24FJ64GA002
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36
        MPLAB 	          :  MPLAB X v5.10
*/


/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
//#include "uart2.h"

extern  unsigned int InputRegister[10];
extern  unsigned int HoldingRegister[10];
/*
                         Main application
 */
int main(void)
{
    unsigned int c;
    unsigned long t;
    
    // initialize the device
    SYSTEM_Initialize();
    InputRegister[0]=0;
    InputRegister[1]=0;
    HoldingRegister[0]=0x0A;
    HoldingRegister[1]=0xD;
    LATB=0x000F;
    LATBbits.LATB0=1;
    
    while (1)
    {
           
    InputRegister[0]=c++;
    InputRegister[1]=c++;
    for(t=0;t<1000000;t++);
    
    }

    return 1;
}
/**
 End of File
*/

