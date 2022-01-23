/*
 * bsp_test_unit.c
 *
 *  Created on: 2021. 1. 24.
 *      Author: supgns03
 */
#include "bsp_test_unit.h"


uint32 BCDToDEC(uint8* bcd, uint8 flag) {
    uint32 dec_ret = 0;

    if(flag == 1) { /* little Endian */
        dec_ret = ((bcd[0] << 24) + (bcd[1] << 16) + (bcd[2] << 8) + (bcd[3] << 0));
    }
    else { /* Big Endian */
        dec_ret = ((bcd[3] << 24) + (bcd[2] << 16) + (bcd[1] << 8) + (bcd[0] << 0));
    }

    return dec_ret;
}

void DECToBCD(uint32 dec, uint8* bcd) {
    bcd[0] = (uint8)((dec >> 24) & 0xFF);
    bcd[1] = (uint8)((dec >> 16) & 0xFF);
    bcd[2] = (uint8)((dec >> 8) & 0xFF);
    bcd[3] = (uint8)((dec >> 0) & 0xFF);
}


/**
*   @fn void ht_temp_test(void)
*   @brief This is function to test Temperature Driver.
*   @param[in] N/A
*   @return N/A
*   @remarks Must include "ht_temp.h" file.
*/
void unit_temp_test(void) {
    /* Title Setting */
    uint8 TEMP_TEST_Title[30] = {"  Unit TEMP Test"};
    sint32 temp_test_ret = 0;
    uint8 key = 0;

    sint32 pointup = 0, pointdown = 0;

    float32 ftemp_value = 0.0;
    uint32 step=0;

    CLEAR_VT_SCREEN();
    GOTO_VT_XY(0, 0);

    while(1) {

        if(step==0){
            temp_test_ret = HTT_GetFloatTemp(&ftemp_value);
            dp_Temp_Title(TEMP_TEST_Title, pointup, pointdown, ftemp_value, temp_test_ret);
            HTT_Printf("   -------------------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(1000);
            if((key == 'q') || (key == 'Q')) {
                break;
            }
        }else if(step==1){
            HTT_Printf("   -------------------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(1000);
            if((key == 'q') || (key == 'Q')) {
                break;
            }
            step=0;
        }
    }
}

void unit_fee_test(void) {
#define buff_array  16       /* Block 1 ~ 16 */
#define buff_len    1024     /* 1 Block 1024 byte */

    sint32 fee_ret = 0;

    unsigned int BlockNumber;
    unsigned int BlockOffset, Length;

    uint8 wr_fee_buf[buff_len];
    uint8 rd_fee_buf[buff_len];

    uint32 i = 0, j = 0;

    uint8 fee_result[4];
    uint8 FEE_TEST_Title[35] = {" Unit Flash EEPROM Test "};
    uint8 FEE_PATTERN[4] = {
        {0x00},
        {0x55},
        {0xAA},
        {0xFF}
    };

    /* Title Setting */
    dp_Mem_Title(FEE_TEST_Title);

    for(i = 0 ; i < buff_array ; i++) /* buff_array 16 */
    {
        BlockNumber=i + 1;
        BlockOffset = 0;
        Length = sizeof(wr_fee_buf);

        for(j = 0 ; j < sizeof(FEE_PATTERN) ; j++) {
            /* Block Clear */
            fee_ret = HTT_EraseFee(BlockNumber);
            if(fee_ret == -1) {
                HTT_Printf("Data Erase Error[%d]\r\n", BlockNumber);
                return;
            }
            /* Buffer Clear */
            memset(wr_fee_buf, 0x00, Length);
            memset(rd_fee_buf, 0x00, Length);
            /* Flash EEPROM Read */
            fee_ret = HTT_ReadFee(BlockNumber,BlockOffset,rd_fee_buf,Length);
            if(fee_ret == 0) {
                /* Buffer Compare */
                if(memcmp(wr_fee_buf, rd_fee_buf, Length) != 0) {
                    fee_result[j] = 1;
                    //HTT_Printf("Block[%d] Erase Compare Error[%d].\r\n", BlockNumber);
                }
            }
            else {
                fee_result[j] = 1;
                //HTT_Printf("3 Step Erase Read Error[%d][%d]\r\n", BlockNumber, Length);
            }
            memset(wr_fee_buf, (FEE_PATTERN[j]), Length);
            memset(rd_fee_buf, 0x00, Length);
            fee_ret = HTT_WriteFee(BlockNumber, wr_fee_buf);
            if(fee_ret == 0) {
                fee_ret = HTT_ReadFee(BlockNumber,BlockOffset,rd_fee_buf,Length);
                if(fee_ret == 0) {
                    if(memcmp(wr_fee_buf, rd_fee_buf, Length) == 0) {
                        if(fee_result[j] != 1){
                            fee_result[j] = 0;
                        }
                        //HTT_Printf("Block[%d] Compare OK.[%d]\r\n", BlockNumber, read_temp[0]);
                    }
                    else {
                        fee_result[j] = 1;
                        //HTT_Printf("Block[%d] Compare Error.\r\n", BlockNumber);
                        return;
                    }
                }
                else {
                    fee_result[j] = 1;
                    //HTT_Printf("2 Block Read Error[%d][%d]\r\n", BlockNumber, Length);
                    return;
                }
            }
            else {
                fee_result[j] = 1;
                HTT_Printf("1 Block Write Error\r\n");
                return;
            }
        }
        dp_Mem_Result(BlockNumber, fee_result);
    }
}

void unit_sync_test(void) {
    uint8 OwnSYNC_TEST_Title[30] = {"  Unit Sync Test"};
    uint8 step = 0;
    sint32 sync_ret = 0;
    uint8 key = 0U;
    uint32 nok=0;
    uint32 nfail=0;

    CLEAR_VT_SCREEN();
    GOTO_VT_XY(0, 0);

    while(1) {

        if(step==0){
            sync_ret = bspGetOwnSyncResult();

            if(sync_ret==1)nok++;
            else if(sync_ret==-1)nfail++;

            dp_OwnSync_Title(OwnSYNC_TEST_Title, nok, nfail);
            HTT_Printf("   -------------------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(250);
            if((key == 'q') || (key == 'Q')) {
                break;
            }
        }
    }
}

/**
*   @fn void unit_async_test(void)
*   @brief This is function to test Async Driver.
*   @param[in] N/A
*   @return N/A
*   @remarks Must include "bsp_async.h" file.
*/
void unit_async_test(void) {
    uint8 rx_async_buf[256];
    uint8 tx_async_buf[256];

    sint32 rx_async_ret = 0;

    uint8 key = 0;
    uint32 i = 0;
    uint8 async_result[4];

    uint32 step=0;
    uint32 nok=0;
    uint32 nfail=0;
    uint32 ncnt=0;

    uint8 ASYNC_TEST_Title[30] = {"  Unit Async Test"};

    uint8 ASYNC_TEST_Ch_text[2][11] = {
        {"1CH -> 2CH"},
        {"2CH -> 1CH"},
    };

    uint8 ASYNC_PATTERN[4] = {
            {0x00},
            {0x55},
            {0xAA},
            {0xFF}
    };

    CLEAR_VT_SCREEN();
    GOTO_VT_XY(0, 0);

    while(1) {

        if(step==0){
            ncnt=0;
            /* Titile Setting & Display */
            dp_Comm_Title(ASYNC_TEST_Title);

            /* 1 Step 1Ch -> 2Ch */
            for(i = 0 ; i < sizeof(ASYNC_PATTERN) ; i++) {
                memset(tx_async_buf, ASYNC_PATTERN[i], sizeof(tx_async_buf));
                memset(rx_async_buf, 0x00, sizeof(rx_async_buf));

                HTT_WriteAsync(sciREG, tx_async_buf, sizeof(tx_async_buf));
                //delayMS(10);
                rx_async_ret = HTT_ReadAsync(scilinREG, rx_async_buf, ASYNC_WAIT_TIMEOUT);

                if(rx_async_ret > 0) {
                    if(memcmp(tx_async_buf, rx_async_buf, sizeof(tx_async_buf)) == 0) {
                        async_result[i] = 0;
                        ncnt++;
                    }
                    else {
                        async_result[i] = 1;
                    }
                }
                else {
                    async_result[i] = 1;
                }
            }
            /* 1 Step Result Display */
            dp_Comm_Result(ASYNC_TEST_Ch_text[0], async_result);

            /* 1 Step 2Ch -> 1Ch */
            for(i = 0 ; i < sizeof(ASYNC_PATTERN) ; i++) {
                memset(tx_async_buf, ASYNC_PATTERN[i], sizeof(tx_async_buf));
                memset(rx_async_buf, 0x00, sizeof(rx_async_buf));

                HTT_WriteAsync(scilinREG, tx_async_buf, sizeof(tx_async_buf));
                //delayMS(10);
                rx_async_ret = HTT_ReadAsync(sciREG, rx_async_buf, ASYNC_WAIT_TIMEOUT);

                if(rx_async_ret >0) {
                    if(memcmp(tx_async_buf, rx_async_buf, sizeof(tx_async_buf)) == 0) {
                        async_result[i] = 0;
                        ncnt++;
                    }
                    else {
                        async_result[i] = 1;
                    }
                }
                else {
                    async_result[i] = 1;
                }
            }

            if(ncnt == 8){
                nok++;
            }
            else{
                nfail++;
            }

            dp_Comm_Result(ASYNC_TEST_Ch_text[1], async_result);
            HTT_Printf("\r\n   -------------------------------------------------------\r\n");
            HTT_Printf("      Result  OK:%d      Fail:%d \r\n", nok, nfail);
            HTT_Printf("   -------------------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(250);
            if((key == 'q') || (key == 'Q')) {
                break;
            }
        }
    }
}

void unit_powermonitor_test(void){
    uint8 key;

    uint32 step=0;
    uint32 nok=0;
    uint32 nfail=0;
    uint32 i, cnt1;
    sint32 result[6];

    CLEAR_VT_SCREEN();
    GOTO_VT_XY(0, 0);

    while(1) {

        if(step==0){

            cnt1 = 0;

            for(i=1; i<6; i++){
                result[i] = HTT_PowerMonitor(i);
                if(result[i] == -1 || result[i] == 0){
                    result[i]=1;
                }
                else{
                    result[i]=0;
                    cnt1++;
                }
            }

            if(cnt1 == 5){
                nok++;
            }else{
                nfail++;
            }

            dp_PowerMonitor_Title(result[1],result[2],result[3],result[4],result[5]);
            HTT_Printf("   ------------------------------------------------\r\n");
            HTT_Printf("      Result  OK:%d      Fail:%d \r\n", nok, nfail);
            HTT_Printf("   ------------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(250);
            if((key == 'q') || (key == 'Q')) {
                break;
            }
        }
    }
}

void unit_watchdog_test(void) {
    uint8 WatchDog_TEST_Title[30] = {"  Unit WatchDog Test"};
    uint8 key = 0;

    CLEAR_VT_SCREEN();
    GOTO_VT_XY(0, 0);

    while(1) {
        dp_WatchDog_Title(WatchDog_TEST_Title, 0, 0);

        HTT_Printf("   ------------------------------------------------------\r\n");
        HTT_Printf("   WDI Signal Disable    [d]  \r\n\r\n");
        HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");

        key = bspGetKey(250);
        if((key == 'q') || (key == 'Q')) {
            HTT_SetWDI(WDI_ENABLE);
            break;
        }
        else if(key == 'e' || (key == 'E')) {
            HTT_SetWDI(WDI_ENABLE);
        }
        else if(key == 'd'|| (key == 'D')) {
            HTT_SetWDI(WDI_DISABLE);
        }
    }
}

void unit_can_test(void) {
    uint8 tx_can_buf[8];
    uint8 rx_can_buf[8];

    uint8 key = 0;
    uint32 i = 0;
    sint32 rx_ret = 0;

    uint8 can_result[4];
    uint32 step=0;
    uint32 nok=0;
    uint32 nfail=0;
    uint32 ncnt=0;
    uint32 can_powsts;

    uint8 CAN_TEST_Ch_text[4][11] = {
        {"1CH -> 2CH"},
        {"2CH -> 1CH"},
        {"2CH -> 3CH"}
    };

    uint8 CAN_Power_Status[2][11] = {
        {"Power ON"},
        {"Power OFF"}
    };

    uint8 CAN_PATTERN[4] = {
            {0x00},
            {0x55},
            {0xAA},
            {0xFF}
    };

    uint8 CAN_TEST_Title[30] = {"  Unit CAN Test"};

    HTT_SetCANPower(CAN_POWER_ENABLE);
    can_powsts=0;

    CLEAR_VT_SCREEN();
    GOTO_VT_XY(0, 0);

    while(1) {

        if(step==0){
            ncnt=0;
            /* Display SET */
            dp_Comm_Title(CAN_TEST_Title);

            /* 1 Step 1Ch -> 2Ch */
            for(i = 0 ; i < sizeof(CAN_PATTERN) ; i++) {

                /* CAN Tx Data Setting & CAN Rx Buffer Clear */
                memset(tx_can_buf, CAN_PATTERN[i], sizeof(tx_can_buf));
                memset(rx_can_buf, 0x00, sizeof(rx_can_buf));
                /* CAN Tx (ID 100) */
                HTT_WriteCAN(canREG1, 100U, tx_can_buf, sizeof(tx_can_buf));
                /* CAN Rx (ID 100) */
                rx_ret = HTT_ReadCAN(canREG2, 100U, rx_can_buf, 100U);

                if(rx_ret >= 0) {

                    if(memcmp(tx_can_buf, rx_can_buf, sizeof(tx_can_buf)) == 0) {
                        can_result[i] = 0;
                        ncnt++;
                    }
                    else {
                        can_result[i] = 1;
                    }
                }
                else {
                    can_result[i] = 1;
                }
            }
            dp_Comm_Result(CAN_TEST_Ch_text[0], can_result);

            /* 2 Step 2Ch -> 1Ch */
            for(i = 0 ; i < sizeof(CAN_PATTERN) ; i++) {

                memset(tx_can_buf, CAN_PATTERN[i], sizeof(tx_can_buf));
                memset(rx_can_buf, 0x00, sizeof(rx_can_buf));
                HTT_WriteCAN(canREG2, 101U, tx_can_buf, sizeof(tx_can_buf));
                rx_ret = HTT_ReadCAN(canREG1, 101U, rx_can_buf, 100U);
                if(rx_ret >= 0) {
                    if(memcmp(tx_can_buf, rx_can_buf, sizeof(tx_can_buf)) == 0) {
                        can_result[i] = 0;
                        ncnt++;
                    }
                    else {
                        can_result[i] = 1;
                    }
                }
                else {
                    can_result[i] = 1;
                }
            }
            dp_Comm_Result(CAN_TEST_Ch_text[1], can_result);

            /* 2 Step 2Ch -> 3Ch */
            for(i = 0 ; i < sizeof(CAN_PATTERN) ; i++) {

                memset(tx_can_buf, CAN_PATTERN[i], sizeof(tx_can_buf));
                memset(rx_can_buf, 0x00, sizeof(rx_can_buf));
                HTT_WriteCAN(canREG2, 102U, tx_can_buf, sizeof(tx_can_buf));
                rx_ret = HTT_ReadCAN(canREG3, 102U, rx_can_buf, 100U);

                if(rx_ret >= 0) {
                    if(memcmp(tx_can_buf, rx_can_buf, sizeof(tx_can_buf)) == 0) {
                        can_result[i] = 0;
                        ncnt++;
                    }
                    else {
                        can_result[i] = 1;
                    }
                }
                else {
                    can_result[i] = 1;
                }
            }
            dp_Comm_Result(CAN_TEST_Ch_text[2], can_result);


            if(ncnt == 12){
                nok++;
            }
            else{
                nfail++;
            }

            HTT_Printf("\r\n    %s                                                        \r\n",CAN_Power_Status[can_powsts]);
            HTT_Printf("\r\n   -------------------------------------------------------\r\n");
            HTT_Printf("      Result  OK:%d      Fail:%d \r\n", nok, nfail);
            HTT_Printf("   -------------------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit], 1CH POW ON[o], OFF[f]\r\n");
            key = bspGetKey(1000);
            if((key == 'q') || (key == 'Q')) {
                break;
            }
            else if((key == 'o') || (key == 'O')) {
                HTT_SetCANPower(CAN_POWER_ENABLE);
                can_powsts=0;
            }
            else if((key == 'f') || (key == 'F')) {
                HTT_SetCANPower(CAN_POWER_DISABLE);
                can_powsts=1;
            }

        }
    }
}


/**
*   @fn void unit_tss_test(void)
*   @brief This is function to test Speed Sensor Driver.
*   @param[in] N/A
*   @return N/A
*   @remarks Must include "bsp_tss.h" file.
*/
void unit_tss_test(void) {

    uint8 TSS_TEST_Title[30] = {"  Unit Speed Sensor Test"};
    uint8 input_key = 0;

    tacho_status_t tss_result[4];
    uint32 test_temp = 0;

    HTT_SetModeTacho(TSS_OPER_MODE);

    while(1) {
        dp_Tss_Title(TSS_TEST_Title, HTT_GetModetTacho());
        memset(tss_result, 0x00, sizeof(tss_result));

        for(test_temp = 0U ; test_temp < 4 ; test_temp++) {
            if(HTT_GetTacho(test_temp, &tss_result[test_temp]) != 0) {
                HTT_Printf(" Speed Sensor Test ERROR..! \r\n");
                break;
            }
        }

        dp_Tss_Result(tss_result);

        HTT_Printf("   -------------------------------------------------------\r\n");
        HTT_Printf("   Input Key, Quit[q], Test Mode[t], Operate Mode[o] \r\n");

        input_key = bspGetKey(1000);

        if((input_key == 'q') || (input_key == 'Q')) {
            break;
        }
        else if(input_key == 'o' || (input_key == 'O')) {
            HTT_SetTachoPower(TSS_PW_1CH, TSS_PW_ENABLE);
            HTT_SetTachoPower(TSS_PW_2CH, TSS_PW_ENABLE);

            TACHOSetTPulseEna(TSS_PULSE_1CH, TSS_PULSE_DISABLE);
            TACHOSetTPulseEna(TSS_PULSE_2CH, TSS_PULSE_DISABLE);
            TACHOSetTPulseEna(TSS_PULSE_3CH, TSS_PULSE_DISABLE);
            TACHOSetTPulseEna(TSS_PULSE_4CH, TSS_PULSE_DISABLE);

            HTT_SetModeTacho(TSS_OPER_MODE);

        }
        else if(input_key == 't'|| (input_key == 'T')) {
            TACHOSetTPulse(250);
            HTT_SetTachoPower(TSS_PW_1CH, TSS_PW_ENABLE);
            HTT_SetTachoPower(TSS_PW_2CH, TSS_PW_ENABLE);

            TACHOSetTPulseEna(TSS_PULSE_1CH, TSS_PULSE_ENABLE);
            TACHOSetTPulseEna(TSS_PULSE_2CH, TSS_PULSE_ENABLE);
            TACHOSetTPulseEna(TSS_PULSE_3CH, TSS_PULSE_ENABLE);
            TACHOSetTPulseEna(TSS_PULSE_4CH, TSS_PULSE_ENABLE);

            HTT_SetModeTacho(TSS_TEST_MODE);
        }
        else if(input_key == 'd' || (input_key == 'D')) {
            if(tss_t_val.t_dir == 0) tss_t_val.t_dir=1;
            else tss_t_val.t_dir=0;
        }
        else if(input_key == '1') {
            TACHOSetTPulse(500);
        }
        else if(input_key == '2') {
            TACHOSetTPulse(1000);
        }
        else if(input_key == '3') {
            TACHOSetTPulse(2000);
        }
        else if(input_key == '4') {
            TACHOSetTPulse(5000);
        }
        else if(input_key == '5') {
            TACHOSetTPulse(10000);
        }
        else if(input_key == '6') {
            TACHOSetTPulse(20000);
        }
        else if(input_key == '7') {
            TACHOSetTPulse(25000);
        }
    }
}

void unit_adcmonitor_test(void){

    uint8 key = 0;
    uint32 adc_ch1, adc_ch2;

    HTT_SetTachoPower(TSS_PW_1CH, TSS_PW_DISABLE);
    HTT_SetTachoPower(TSS_PW_2CH, TSS_PW_DISABLE);

    CLEAR_VT_SCREEN();
    GOTO_VT_XY(0, 0);

    while(1) {

        adc_ch1 = HTT_StateTacho(TSS_PW_1CH);
        adc_ch2 = HTT_StateTacho(TSS_PW_2CH);

        dp_Adc_Title(adc_ch1, adc_ch2);
        HTT_Printf("   ------------------------------------------------\r\n");
        HTT_Printf("   Please Input Menu Key :\r\n");
        HTT_Printf("   TACHO 1CH: [1]ON/[2]OFF,     2CH: [3]ON/[4]OFF,    QUIT: [q] \r\n");
        key = bspGetKey(250);

        if(key == 'q' || key == 'Q') {
            break;
        }
        else if(key == '1') {
            HTT_SetTachoPower(TSS_PW_1CH, TSS_PW_ENABLE);
        }
        else if(key == '2') {
            HTT_SetTachoPower(TSS_PW_1CH, TSS_PW_DISABLE);
        }
        else if(key == '3') {
            HTT_SetTachoPower(TSS_PW_2CH, TSS_PW_ENABLE);
        }
        else if(key == '4') {
            HTT_SetTachoPower(TSS_PW_2CH, TSS_PW_DISABLE);
        }

    }
}

static uint8 year=0;
static uint8 month=0;
static uint8 day=0;
static uint8 log_tbuf[512];

sint64 Log_test(uint8 code){
    sint64 ret=0;
    uint32 i;

    switch(code){
        case LOG_WRITE:
            /*SUCCESS: LEN , FAIL:-1*/
            memset(log_tbuf, 0x00, 512);
            day++;
            if(day==31){
                day=0;
                month++;
                if(month==12){
                    month=0;
                    year++;
                    if(year==50){
                        year=0;
                    }
                }
            }

            log_tbuf[0]=0x20+year;
            log_tbuf[1]=0x01+month;
            log_tbuf[2]=0x01+day;

            for(i=3; i<300; i++){
                log_tbuf[i] = i & 0xff;
            }

            ret = HTT_WriteLog(log_tbuf, 300);
            break;
    }

    return ret;
}

void unit_log_test(void) {
    uint8 key = 0;
    sint32 ret = 0;

    uint8 LOG_TEST_Title[30] = {"  Unit Log Test"};

    logifo.step=0;

    CLEAR_VT_SCREEN();

    dp_Log_Title(LOG_TEST_Title, 6, log_ok, log_fail);

    delayMS(3000);

    logifo.mode = LOG_WRITE;

    while(1) {
        ret=0;
        if(logifo.mode == LOG_WRITE){
            ret = Log_test(LOG_WRITE);
        }

        if(ret == 1 || ret > 1){
            log_ok++;
            dp_Log_Title(LOG_TEST_Title, logifo.mode, log_ok, log_fail);
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(250);
            if((key == 'q') || (key == 'Q')) {
                log_ok=0; log_fail=0;
                break;
            }
        }else{
            if(ret==-1){
                log_fail++;
                dp_Log_Title(LOG_TEST_Title, logifo.mode, log_ok, log_fail);
                break;
            }
        }
    }
}

/**
*   @fn void ht_eeprom_test(void)
*   @brief This is function to test EEPROM Driver.
*   @param[in] N/A
*   @return N/A
*   @remarks Must include "ht_eeprom.h" file.
*/
void unit_eeprom_test(void) {
    uint8 wr_eeprom_buf[512];
    uint8 rd_eeprom_buf[512];

    sint32 rd_eeprom_ret = 0;

    uint8 key = 0;
    uint32 j = 0;
    uint8 eeprom_result[4];
    uint32 cnt;


    uint8 EEPROM_TEST_Title[30] = {"    EEPROM Memory Test"};

    uint8 EEPROM_PATTERN[4] = {
            {0x00},
            {0x55},
            {0xAA},
            {0xFF}
    };

    uint32 address = 0;
    uint32 len = 0;
    uint32 nok=0;
    uint32 nfail=0;

    CLEAR_VT_SCREEN();

    while(1) {

        dp_EEP_Mem_Title(EEPROM_TEST_Title);

        address = 0;
        while(1) {
            cnt=0;
            if((address + sizeof(wr_eeprom_buf)) > (EEPROM_SIZE - 1)) {
                len = (EEPROM_SIZE - 1) - address;
            }
            else {
                len = sizeof(wr_eeprom_buf);
            }

            for(j = 0 ; j < 4 ; j++) {
                memset(wr_eeprom_buf, EEPROM_PATTERN[j], sizeof(wr_eeprom_buf));
                memset(rd_eeprom_buf, 0x00, sizeof(rd_eeprom_buf));

                HTT_WriteEeprom(address, wr_eeprom_buf, len);
                rd_eeprom_ret = HTT_ReadEeprom(address, rd_eeprom_buf, len);
                if(rd_eeprom_ret == 0) {
                    if(memcmp(wr_eeprom_buf, rd_eeprom_buf, len) == 0) {
                        eeprom_result[j] = 0;
                        cnt++;
                    }
                    else {
                        eeprom_result[j] = 1;
                        break;
                    }
                }
                else {
                    eeprom_result[j] = 1;
                    break;
                }
            }

            if(cnt == 4){
                nok++;
            }else{
                nfail++;
            }

            dp_EEP_Mem_Result(address, eeprom_result);
            address = address + len;
            memset(eeprom_result, 0x01, sizeof(eeprom_result));

            if(address >= EEPROM_SIZE - 1) {
                break;
            }
        }
        HTT_Printf("\r\n   -------------------------------------------------------\r\n");
        HTT_Printf("      Result  OK:%d      Fail:%d \r\n", nok, nfail);
        HTT_Printf("   -------------------------------------------------------\r\n");
        HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
        key = bspGetKey(250);
        if((key == 'q') || (key == 'Q')) {
            break;
        }
    }
}

/**
*   @fn void unit_switch_test(void)
*   @brief This is function to test Switch Driver.
*   @param[in] N/A
*   @return N/A
*   @remarks Must include "bsp_switch.h" file.
*/
void unit_switch_test(void) {
    uint8 SWITCH_TEST_Title[30] = {"  Unit BoardID Test"};
    uint8 key = 0;

    sint32 rack_id = 0U;

    CLEAR_VT_SCREEN();

    while(1) {
        rack_id = 0;
        rack_id = HTT_GetBoardID();
        dp_Switch_Title(SWITCH_TEST_Title, rack_id);

        HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
        key = bspGetKey(250);
        if((key == 'q') || (key == 'Q')) {
            break;
        }
    }
}

void unit_test(void) {
    uint8 menu_key = 0U;

    while(1) {
        menu_key = dp_Menu_Title();
        switch(menu_key) {

        case TEST_FEE:
            unit_fee_test();            /* FALSH EEPROM TEST*/
        break;

        case TEST_RS485:
            unit_async_test();          /* ASYNCE (RS485) TEST */
        break;

        case TEST_CAN:
            unit_can_test();            /* CAN Test */
        break;

        case TEST_TEMP:
            unit_temp_test();           /* TEMP(TMP75) TEST */
        break;

        case TEST_SYNC:
            unit_sync_test();           /* Sync Test (5Hz) */
        break;

        case TEST_ADC:
            unit_adcmonitor_test();     /* Current Check Test */
        break;

        case TEST_POWER:
            unit_powermonitor_test();   /* PowerMonitor Check Test */
        break;

        case TEST_TSS:
            unit_tss_test();            /* ADC, TSS TEST */
        break;

        case TEST_WATCHDOG:
             unit_watchdog_test();      /* WATCHDOG TEST */
        break;

        case TEST_LOG:
            unit_log_test();            /* SPI1 & MEMBOARD */
        break;

        case TEST_SWITCH:
            unit_switch_test();         /* RACK ID */
        break;

        case TEST_EEPROM:
            unit_eeprom_test();
        break;

        case KEY_ENTER:
            menu_key = dp_Menu_Title();
        break;

        case TEST_QUIT:
            return;                     /* Back */
            //break;
        }

        HTT_Printf("\r\n\r\n");
    }

}




