/*
* The program is a sample code. 
* It needs run with NuMaker NuWicam board.
*/

/* ----------------------- System includes --------------------------------*/
#include "mbed.h"
#include "rtos.h"
/*----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
// Sharing buffer index
enum {
  eData_MBInCounter,
  eData_MBOutCounter,
  eData_MBError,  
  eData_DI,
  eData_DO,
  eData_RGB,
  eData_MBResistorVar,
  eData_TemperatureSensor,
  eData_Cnt
} E_DATA_TYPE;

#define REG_INPUT_START 1
#define REG_INPUT_NREGS eData_Cnt
#define SLAVE_ID 0x01
/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];

DigitalOut led1(LED1);  // For temperature worker.
DigitalOut led2(LED2);  // For Modbus worker.
DigitalOut led3(LED3);  // For Holder CB

AnalogIn   LM35(A0);

#define DEF_LED_NUM 6
#if defined(TARGET_NUMAKER_PFM_NUC472)
DigitalOut LED[DEF_LED_NUM] = { PF_9, PF_10, PC_10, PC_11, PA_10, PA_9 } ;
#elif defined(TARGET_NUMAKER_PFM_M453)
DigitalOut LED[DEF_LED_NUM] = { PC_6, PC_7, PC_11, PC_12, PC_13, PC_14 } ;
#elif defined(TARGET_NUMAKER_PFM_M487)
DigitalOut LED[DEF_LED_NUM] = { PH_9, PH_8, PB_9, PF_11, PG_4, PC_11 } ;
#endif

void light_leds()
{
    int i=0;
    USHORT usOutValue = ~usRegInputBuf[eData_DO];
    for ( i=0; i<DEF_LED_NUM ; i++)
        LED[i].write( (usOutValue&(0x1<<i)) >> i  );
}

void get_temp(void)
{
    float tempC, a[10], avg;
    int i;
    #define DEF_ADC_READTIMES   10
    avg=0;
    for(i=0;i<DEF_ADC_READTIMES;i++)
    {
        a[i] = LM35.read();
        ThisThread::sleep_for(1s);
    }
    
    for ( i=0; i<DEF_ADC_READTIMES; i++ )
        avg += a[i];
    
    avg /= DEF_ADC_READTIMES;
    tempC=(avg*3.685503686*100);
    usRegInputBuf[eData_TemperatureSensor] = (USHORT)tempC;
    //printf("[%s %d] %f %d\r\n", __func__, __LINE__, avg, usRegInputBuf[eData_TemperatureSensor]  );
}

void worker_get_temperature()
{
    // Poll temperature sensor per 1 second.
    while (true) {
        get_temp();
        led1 = !led1;
        ThisThread::sleep_for(1000ms);
    }
}

#if 0
void worker_uart()
{   
    // For UART-SERIAL Tx/Rx Service.
    while (true)
        xMBPortSerialPolling();
}
#endif

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    eMBErrorCode    eStatus;
    

    Thread temperature_thread;
    temperature_thread.start(callback(worker_get_temperature));
    
#if 0
    Thread uart_thread;
    uart_thread.start(callback(worker_uart));
#endif

    // Initialise some registers
    for (int i=0; i<REG_INPUT_NREGS; i++)
         usRegInputBuf[i] = 0x0;

    light_leds(); // Control LEDs
            
    /* Enable the Modbus Protocol Stack. */
    if ( (eStatus = eMBInit( MB_RTU, SLAVE_ID, 0, 115200, MB_PAR_NONE )) !=  MB_ENOERR )
        goto FAIL_MB;
    else if ( (eStatus = eMBEnable(  ) ) != MB_ENOERR )
        goto FAIL_MB_1;
    else {
        for( ;; )
        {
            xMBPortSerialPolling();
            if ( eMBPoll( ) != MB_ENOERR ) break;
        }       
    }    
    
FAIL_MB_1:
    eMBClose();    
    
FAIL_MB:
    for( ;; )
    {
        led2 = !led2;
        ThisThread::sleep_for(200ms);
    }
}













eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    
    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
     
    usRegInputBuf[eData_MBInCounter]++;
    usRegInputBuf[eData_MBOutCounter]++;
        
    if (eMode == MB_REG_READ)
    {
        if( ( usAddress >= REG_INPUT_START )
            && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
        {
            iRegIndex = ( int )( usAddress - usRegInputStart );
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ =
                    ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ =
                    ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
        }
    }

    if (eMode == MB_REG_WRITE)
    {
        if( ( usAddress >= REG_INPUT_START )
            && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
        {
            iRegIndex = ( int )( usAddress - usRegInputStart );
            while( usNRegs > 0 )
            {
                usRegInputBuf[iRegIndex] =  ((unsigned int) *pucRegBuffer << 8) | ((unsigned int) *(pucRegBuffer+1));
                pucRegBuffer+=2;
                iRegIndex++;
                usNRegs--;
            }
            light_leds(); // Control LEDs
        }
    }

    led3=!led3;
        
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
