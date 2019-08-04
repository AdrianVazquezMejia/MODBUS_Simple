

/**
  Section: Included Files
 */

#include "uart2.h"
#include "CRC.h"
#include "pin_manager.h"
/**
  Section: Data Type Definitions
 */


#define SlaveID   1

INT_VAL dirIn;
INT_VAL NoIn;
INT_VAL Crc;

ModbusEstados estadoRx = SlaveAddress;
uint8_t buffRx[100], buffTx[100], n;
uint8_t *pint;
uint8_t contTx;
INT_VAL InputRegister[10];
INT_VAL HoldingRegister[10];
INT_VAL DataCoils;
INT_VAL DataInputs;
INT_VAL CoilAddress;
INT_VAL Data;

/**
  Section: Driver Interface
 */


void UART2_Initialize(void) {
    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; RXINV disabled; UEN TX_RX; 
    U2MODE = (0x8008 & ~(1 << 15)); // disabling UARTEN bit   
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U2STA = 0x0000;
    // BaudRate = 9600; Frequency = 16000000 Hz; BRG 416; 
    U2BRG = 0x01A0;

    IFS1bits.U2RXIF = false; // Clear the Recieve Interrupt Flag
    IEC1bits.U2RXIE = 1; // 1 Enable Recieve Interrupts

    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
    U2MODEbits.UARTEN = 1; // enabling UART ON bit
    U2STAbits.UTXEN = 1;

    IFS1bits.U2TXIF = false; // Clear the Transmit Interrupt Flag 
    IEC1bits.U2TXIE = 1; // 1 Enable Transmit Interrupts

}

/**
    Maintains the driver's transmitter state machine and implements its ISR
 */

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void) {

    pint++;
    if (--contTx > 0) U2TXREG = *pint;

    IFS1bits.U2TXIF = false;
}

INT_VAL RetrieveCoilsStatus(INT_VAL dirIn, INT_VAL NoIn) {
    INT_VAL feedback, puerto;
    //puerto.Val=0xFFFF;
    feedback.byte.LB = dirIn.byte.LB;
    puerto.Val = PORTB;
    puerto.byte.LB = puerto.byte.LB << 4;
    feedback.byte.LB = puerto.byte.LB + feedback.byte.LB;
    puerto.Val = PORTB;
    puerto.byte.LB = puerto.byte.LB >> 4;
    feedback.byte.HB = puerto.byte.LB;
    //feedback.Val=0xFFFA;
    return feedback;

}

INT_VAL RetrieveInputsStatus(INT_VAL dirIn, INT_VAL NoIn) {
    INT_VAL feedback, puerto;
    //puerto.Val=0xFFFF;
    feedback.byte.LB = dirIn.byte.LB;
    puerto.Val = PORTB;
    puerto.byte.HB = puerto.byte.HB << 4;
    feedback.byte.LB = puerto.byte.HB + feedback.byte.LB;
    puerto.Val = PORTB;
    puerto.byte.HB = puerto.byte.HB >> 4;
    feedback.byte.HB = 3;//puerto.byte.HB;
    //feedback.Val=0xFFFA;
    return feedback;

}

void WriteCoils(INT_VAL dirIn, INT_VAL DataCoils) {
    bool Coil = false;
    //PORTBbits.RB0 = true;
    if (DataCoils.Val == 0xFF00) Coil=true;
        switch (dirIn.Val) {
        case 0:
            PORTBbits.RB0 = Coil;
            break;

        case 1:
            PORTBbits.RB1 = Coil;
            break;

        case 2:
            PORTBbits.RB2 = Coil;
            break;

        case 3:
            PORTBbits.RB3 = Coil;
            break;

        case 4:
            PORTBbits.RB4 = Coil;
            break;

        case 5:
            PORTBbits.RB5 = Coil;
            break;

        case 6:
            PORTBbits.RB6 = Coil;
            break;

        case 7:
            PORTBbits.RB7 = Coil;
            break;
    }

        

}

void WriteRegister (void){
    HoldingRegister[dirIn.Val].Val=Data.Val;
}



void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) {

    uint8_t auxRx;
    TMR2 = 0x00;
    auxRx = U2RXREG;

    switch (estadoRx) {
        case SlaveAddress:
            LED1 = 0; // LED1  OFF 
            if (auxRx == SlaveID) {
                n = 0;
                buffRx[n++] = auxRx;
                estadoRx = Function;
            } else estadoRx = EsperaSicronismo;

            break;
            
          

        case Function:
            buffRx[n++] = auxRx;
            switch (auxRx) {
                case 1:
                    estadoRx = StartingAddress4HI;
                    break;

                case 2:
                    estadoRx = StartingAddress4HI;
                    break;

                case 3:
                    estadoRx = StartingAddress4HI;
                    break;

                case 4:
                    estadoRx = StartingAddress4HI;
                    break;

                case 5:
                    estadoRx = CoilAddressHi;
                    break;

                case 6: 
                       estadoRx = StartingAddress4HI;             
                    break;

                case 15:

                    break;

                case 16:

                    break;

                default:
                    estadoRx = EsperaSicronismo;
                    break;

            }
            break;
        case StartingAddress4HI:

            buffRx[n++] = auxRx;
            dirIn.byte.HB = auxRx;
            estadoRx = StartingAddress4LO;

            break;

        case StartingAddress4LO:

            buffRx[n++] = auxRx;
            dirIn.byte.LB = auxRx;
            if (buffRx[1] == 1) {
                estadoRx = NoCoilsHi;
            }
            if (buffRx[1] == 2) {
                estadoRx = NoInputsHi;
            }
            if ((buffRx[1] == 3) | (buffRx[1] == 4)) {
                estadoRx = NoRegisters4Hi;
            }
            if (buffRx[1] == 6) {
                estadoRx = RegisterDataHi;
            }
            
            



            break;

        case CoilAddressHi:
            buffRx[n++] = auxRx;
            dirIn.byte.HB = auxRx;
            estadoRx = CoilAddressLo;
            break;

        case CoilAddressLo:
            buffRx[n++] = auxRx;
            dirIn.byte.LB = auxRx;
            estadoRx = CoilDataHi;
            break;

        case CoilDataHi:
            buffRx[n++] = auxRx;
            DataCoils.byte.HB = auxRx;
            estadoRx = CoilDataLo;
            break;

        case CoilDataLo:
            buffRx[n++] = auxRx;
            DataCoils.byte.LB = auxRx;
            WriteCoils(dirIn, DataCoils);
            //nada();
            //PORTBbits.RB1=1;
            estadoRx = Crc4Hi;
            break;
        case NoRegisters4Hi:

            buffRx[n++] = auxRx;
            NoIn.byte.HB = auxRx;
            estadoRx = NoRegisters4Lo;

            break;

        case NoRegisters4Lo:

            buffRx[n++] = auxRx;
            NoIn.byte.LB = auxRx;
            estadoRx = Crc4Hi;

            break;

        case NoCoilsHi:

            buffRx[n++] = auxRx;
            NoIn.byte.HB = auxRx;
            estadoRx = NoCoilsLo;

            break;

        case NoCoilsLo:

            buffRx[n++] = auxRx;
            NoIn.byte.LB = auxRx;
            DataCoils = RetrieveCoilsStatus(dirIn, NoIn);
            estadoRx = Crc4Hi;

            break;

        case NoInputsHi:

            buffRx[n++] = auxRx;
            NoIn.byte.HB = auxRx;
            estadoRx = NoInputsLo;

            break;

        case NoInputsLo:

            buffRx[n++] = auxRx;
            NoIn.byte.LB = auxRx;
            DataInputs = RetrieveInputsStatus(dirIn, NoIn);
            estadoRx = Crc4Hi;

            break;
        
        case RegisterDataHi:
            buffRx[n++]=auxRx;
            estadoRx= RegisterDataLo;
            Data.byte.HB = auxRx;
            break;
            
            
        case RegisterDataLo:
            buffRx[n++]=auxRx;
            estadoRx=Crc4Hi;
            Data.byte.LB = auxRx;
            WriteRegister();
            
            
            
            
            break;
            
        case Crc4Hi:

            buffRx[n++] = auxRx;
            Crc.byte.HB = auxRx;
            estadoRx = Crc4Lo;
            
            break;

        case Crc4Lo:

            buffRx[n++] = auxRx;
            Crc.byte.LB = auxRx;
            estadoRx = EsperaSicronismo;
            //CRC
            if (CRC16(buffRx, n) == 0) {
                // datos buenos crear respuesta  

                LED0 = !LED0; // LED0 cambia cada vez que pasa
                if (buffRx[1] == 4) {
                    buffTx[0] = buffRx[0];
                    buffTx[1] = buffRx[1];
                    buffTx[2] = 4;
                    buffTx[3] = InputRegister[0].byte.HB;
                    buffTx[4] = InputRegister[0].byte.LB;
                    buffTx[5] = InputRegister[1].byte.HB;
                    buffTx[6] = InputRegister[1].byte.LB;
                    Crc.Val = CRC16(buffTx, 7);
                    buffTx[7] = Crc.byte.LB;
                    buffTx[8] = Crc.byte.HB;
                    contTx = 9;
                }
                // datos buenos crear respuesta  


                if (buffRx[1] == 3) {
                    PORTBbits.RB1=1;
                    buffTx[0] = buffRx[0];
                    buffTx[1] = buffRx[1];
                    buffTx[2] = 4;
                    buffTx[3] = HoldingRegister[0].byte.HB;
                    buffTx[4] = HoldingRegister[0].byte.LB;
                    buffTx[5] = HoldingRegister[1].byte.HB;
                    buffTx[6] = HoldingRegister[1].byte.LB;
                    Crc.Val = CRC16(buffTx, 7);
                    buffTx[7] = Crc.byte.LB;
                    buffTx[8] = Crc.byte.HB;
                    contTx = 9;
                }

                if (buffRx[1] == 1) {
                    buffTx[0] = buffRx[0];
                    buffTx[1] = buffRx[1];
                    buffTx[2] = 2;
                    buffTx[3] = DataCoils.byte.LB;
                    buffTx[4] = DataCoils.byte.HB;
                    Crc.Val = CRC16(buffTx, 5);
                    buffTx[5] = Crc.byte.LB;
                    buffTx[6] = Crc.byte.HB;
                    contTx = 7;
                }

                if (buffRx[1] == 2) {
                    buffTx[0] = buffRx[0];
                    buffTx[1] = buffRx[1];
                    buffTx[2] = 2;
                    buffTx[3] = DataInputs.byte.LB;
                    buffTx[4] = DataInputs.byte.HB;
                    Crc.Val = CRC16(buffTx, 5);
                    buffTx[5] = Crc.byte.LB;
                    buffTx[6] = Crc.byte.HB;
                    contTx = 7;
                }

               if ((buffRx[1] == 5)|(buffRx[1]==6)) {
                    int i;
                    for (i=0;i<9;i++)
                           buffTx[i] = buffRx[i];
                    contTx = 8;
                }

                // transmite respuesta

                pint = buffTx;
                U2TXREG = *pint;
            }

            break;

        case EsperaSicronismo:
            // este estado no hace nada espera Timer 2 lo saque de aqui

            break;

            //        case 100:
            //           
            //            
            //            
            //        break;        
            //                      


    }
    IFS1bits.U2RXIF = false;

}

void __attribute__((interrupt, no_auto_psv)) _U2ErrInterrupt(void) {
    if ((U2STAbits.OERR == 1)) {
        U2STAbits.OERR = 0;
    }

    IFS4bits.U2ERIF = false;
}

/**
  End of File
 */
