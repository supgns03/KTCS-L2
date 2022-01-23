/*
 * main.c
 *
 *
 *
 */


/* base include */
#include "sys_core.h"
#include "drv_het1.h"
#include "drv_het2.h"
#include "drv_adc.h"

/* bsp include */
#include "bsp_async.h"
#include "bsp_can.h"
#include "bsp_debug.h"
#include "bsp_display.h"
#include "bsp_eeprom.h"
#include "bsp_fee.h"
#include "bsp_i2c.h"
#include "bsp_log.h"
#include "bsp_power.h"
#include "bsp_selftest.h"
#include "bsp_switch.h"
#include "bsp_sync.h"
#include "bsp_temp.h"
#include "bsp_timer.h"
#include "bsp_tss.h"
#include "bsp_wdt.h"

#include "bsp_test_unit.h"
#include "bsp_test_sub.h"

#define TEST_UNIT       '1'
#define TEST_SUB        '2'
#define TEST_MECHA      '3'
#define TEST_RESET      0x18

void HTT_SystemInit(void){
    /*  HET1 Pin Setting */
    HET1SetInit();

    /*  HET2 Pin Setting */
    HET2SetInit();

    /* Timer Initialize Setting */
    HTT_InitTimer(rtiCOMPARE0, 100U);        /* 10us - TACHO SENSOR */
    HTT_InitTimer(rtiCOMPARE1, 10000U);      /* 1ms  - LED, SYSTEM  */
    HTT_InitTimer(rtiCOMPARE2, 1000U);       /* 1ms  - WDT          */
    HTT_InitTimer(rtiCOMPARE3, 10U);         /* 10us - SYNC TEST    */

    /* Watchdog Initialize Setting */
    HTT_SetWDI(WDI_ENABLE);

    /* ADC Initialize Setting */
    HTT_InitADC(adcREG1, adcGROUP1);     /* Speed Sensor State Check(ADC) Initialization  */
}

int main(int status) {

    /* Debug Initialize Setting */
    HTT_InitDebug();

    /* disable irq interrupt in Cortex R4 */
    _disable_interrupt_();

    /* Base System Initialize */
    HTT_SystemInit();

    /* Tacho initialization */
    HTT_InitTacho();

    /* Async Communication Initialization */
    HTT_InitAsync(sciREG, 115200);
    HTT_InitAsync(scilinREG, 115200);

    /* CAN Communication Initialization */
    HTT_InitCAN(canREG1, 500);       /* The CAN1 have to connect the jumper at HD1 in order to use the 1Mbit baudrate. */
    HTT_InitCAN(canREG2, 500);       /* The CAN2 have to connect the jumper at HD2 in order to use the 1Mbit baudrate. */
    HTT_InitCAN(canREG3, 500);       /* The CAN2 have to connect the jumper at HD2 in order to use the 1Mbit baudrate. */

    /* EEPROM Memory Initialization */
    HTT_InitEeprom();

    /* Temperature Initialize Setting */
    HTT_InitTemp();

    /* Flash EEPROM Emulation Initialization */
    HTT_InitFee();

    /* Log Initialization */
    HTT_InitLog();

    /* enable irq interrupt in Cortex R4 */
    _enable_interrupt_();

    uint8 menu_sel =0;
    sint32 main_ret = 0;

    while(1) {
        main_ret = HTT_SelfTest_Result();
        menu_sel = dp_Main_Title(main_ret);
        HTT_Printf("Input Key[%x] \r\n", menu_sel);

        switch(menu_sel) {
            case TEST_UNIT:
                /* Driver Unit Test */
                unit_test();                    /* Unit Test        */
                break;
            case TEST_SUB:
                /* Sub Rack Test */
                sub_test();                     /* Sub_Rack Test    */
                break;
            case TEST_MECHA:
                break;
            case TEST_RESET:
                systemREG1->SYSECR = (0x01 << 14U);
                systemREG1->SYSECR = (0x01 << 15U);
                break;
        }

    }

}
