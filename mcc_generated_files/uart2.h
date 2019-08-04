
/**
 Section: Included Files
*/

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum
{
    /*  */
    SlaveAddress,
    Function,
    StartingAddress4HI, StartingAddress4LO, NoRegisters4Hi, 
    NoRegisters4Lo, Crc4Hi, Crc4Lo, NoCoilsHi, NoInputsHi , NoInputsLo ,
    NoCoilsLo, CoilAddressHi, CoilAddressLo, CoilDataHi, CoilDataLo,
            RegisterDataHi,RegisterDataLo,

    EsperaSicronismo

}ModbusEstados;


typedef union
{
    uint16_t Val;
    struct
    {
        uint8_t LB;
        uint8_t HB;
    } byte;

} INT_VAL;


void UART2_Initialize(void);
INT_VAL RetrieveCoilsStatus(INT_VAL dirIn,INT_VAL NoIn);
INT_VAL RetrieveInputStatus(INT_VAL dirIn,INT_VAL NoIn);
void WriteCoils(INT_VAL CoilAddress,INT_VAL DataCoils);
void WriteRegister (void);


