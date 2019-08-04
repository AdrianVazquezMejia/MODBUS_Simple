
#include <xc.h> // include processor files - each processor file is guarded.  


typedef union
{
    uint16_t Val;
    struct
    {
        uint8_t LB;
        uint8_t HB;
    } byte;

} INT_VAL;
