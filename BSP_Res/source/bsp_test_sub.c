/*
 * bsp_test_sub.c
 *
 *  Created on: 2022. 1. 24.
 *      Author: supgns03
 */

#include "bsp_test_sub.h"

uint32 tOk, tFail;
uint32 tBoardID;

#ifdef  SUB_SYNC_USE
/**
*   @fn void unit_sync_test(void)
*   @brief This is function to test other sync signal.
*   @param[in] N/A
*   @return N/A
*   @remarks Must include "bsp_sync.h" file.
*/
void subrack_sync_test(void){

    uint8 Sub_Test_Title[30] = {"  Sub Sync Test"};
    uint8 step = 0;
    sint32 sync_ret = 0;
    uint8 key=0;

    tOk = 0;
    tFail = 0;

    CLEAR_VT_SCREEN();
    GOTO_VT_XY(0, 0);

    while(1) {

        if(step==0){
            sync_ret = bspGetOtherSyncResult();

            if(sync_ret==1)tOk++;
            else if(sync_ret==-1)tFail++;

            dp_OwnSync_Title(Sub_Test_Title, tOk, tFail);
            HTT_Printf("   -------------------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(1000);
            if((key == 'q') || (key == 'Q')) {
                break;
            }
        }
        else if (step==1){
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
#endif

#ifdef  SUB_LCAN_USE
/**
*   @fn void unit_sync_test(void)
*   @brief This is function to test other sync signal.
*   @param[in] N/A
*   @return N/A
*   @remarks Must include "bsp_sync.h" file.
*/
void subrack_loopcan_test(void){

    sint32 atp_can_ret = 0U;
    sint8  nRet;
    uint32 nFlag =1;
    uint8 key=0;

    uint8 Sub_Test_Title[30] = {"  Sub LOOP CAN Test"};

    uint32 i, j;
    sint8 result[4]={0,};

    uint8 can_tx_buf[8];
    uint8 can_rx_buf[8];
    uint8 can_sync_check[8] = {'s','t','a','r','t','c','a','n'};

    nRet = HTT_InitCAN(canREG2, 500);
    if (nRet == -1){
        HTT_Printf("#A CANLoopTest, [Error]no 0ch Init\n");
    }

    nRet = HTT_InitCAN(canREG3, 500);
    if (nRet == -1){
        HTT_Printf("#B CANLoopTest, [Error]no 1ch Init\n");
    }

    tBoardID = HTT_GetBoardID();

    uint8 CAN_PATTERN[4] = {
            {0x00},
            {0x55},
            {0xAA},
            {0xFF}
    };

    HTT_SetCANPower(CAN_POWER_ENABLE);

    tOk = 0;
    tFail = 0;

    dp_Evcom_Title(Sub_Test_Title, tBoardID, tOk, tFail);

    Timecnt.wait = 0;


    while(1) {
        if(tBoardID == SDOU_A){

            while(0 < nFlag){
                HTT_WriteCAN(canREG2, 30U, can_sync_check, sizeof(can_sync_check));
                atp_can_ret = HTT_ReadCAN(canREG2, 31U, can_rx_buf, 100U);
                if(atp_can_ret > 0){
                    if(memcmp(can_rx_buf, can_sync_check, atp_can_ret ) == 0){
                        Timecnt.wait =0;
                        nFlag = 0;
                    }
                }

                if(Timecnt.wait > WAITTIME){
                    nFlag = 0;
                }
            }

            for(j = 0 ; j < sizeof(CAN_PATTERN) ; j++) {
                for(i=0; i<2; i++){
                    memset(can_tx_buf, CAN_PATTERN[j], sizeof(can_tx_buf));
                    memset(can_rx_buf, 0x00, sizeof(can_rx_buf));

                    if(i==0){
                        HTT_WriteCAN(canREG2, 10U, can_tx_buf, sizeof(can_tx_buf));
                        atp_can_ret = HTT_ReadCAN(canREG2, 11U, can_rx_buf, 100U);
                    }else{
                        HTT_WriteCAN(canREG3, 20U, can_tx_buf, sizeof(can_tx_buf));
                        atp_can_ret = HTT_ReadCAN(canREG3, 21U, can_rx_buf, 100U);
                    }
                    if(atp_can_ret > 0) {
                        if(memcmp(can_rx_buf, can_tx_buf, sizeof(can_tx_buf)) == 0) {
                            result[i]=1;
                        }else{
                            result[i]=-1;
                        }
                    }
                    else {
                        result[i]=-1;
                    }
                }
            }

            for(i=0; i<2; i++){
                if (result[i] == 1) tOk++;
                else tFail++;
            }

            dp_Evcom_Title(Sub_Test_Title, tBoardID, tOk, tFail);
            HTT_Printf("   -----------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(100);
            if((key == 'q') || (key == 'Q')) {
                break;
            }


        }
        /* CPU #2 <=> SDOU #2 */
        else if(tBoardID == SDOU_B){

            memset(can_rx_buf, 0x00, sizeof(can_rx_buf));

            while(0 < nFlag){
                atp_can_ret = HTT_ReadCAN(canREG2, 30U, can_rx_buf, 100U);
                if(atp_can_ret > 0){
                    if(memcmp(can_rx_buf, can_sync_check, atp_can_ret ) == 0){
                        nRet = HTT_WriteCAN(canREG2, 31U, can_sync_check, 8U);
                        Timecnt.wait = 0;
                        nFlag = 0;
                    }
                }

                if(Timecnt.wait > WAITTIME){
                    HTT_Printf("   -----------------------------------------------\r\n");
                    HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
                    key = bspGetKey(1000);
                    if((key == 'q') || (key == 'Q')) {
                        Timecnt.wait =0;
                        break;
                    }
                }
            }

            atp_can_ret = HTT_ReadCAN(canREG2, 10U, can_rx_buf, 25U);

            if(atp_can_ret > 0) {
                HTT_WriteCAN(canREG2, 11U, can_rx_buf, atp_can_ret);
                Timecnt.wait = 0;
            }

            atp_can_ret = HTT_ReadCAN(canREG3, 20U, can_rx_buf, 25U);

            if(atp_can_ret > 0) {
                HTT_WriteCAN(canREG3, 21U, can_rx_buf, atp_can_ret);
                Timecnt.wait = 0;
            }

            if(Timecnt.wait > WAITTIME){
                HTT_Printf("   -----------------------------------------------\r\n");
                HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
                key = bspGetKey(1000);
                if((key == 'q') || (key == 'Q')) {
                    break;
                }
            }
        }
    }


}
#endif

#ifdef  SUB_CCAN_USE
void subrack_can_test(void) {

    sint32 atp_can_ret = 0;
    sint8 nRet;
    uint8 key=0;

    uint32 canid1, canid2;
    uint8 nFlag =1;

    uint8 temp_rx_buf[8];
    uint8 bufack[8] = {'s','t','a','r','t','c','a','n'};

    uint8 Sub_Test_Title[30] = {"  Sub CPU CAN Test"};

    Timecnt.wait = 0;
    tOk = 0;
    tFail = 0;

    nRet = HTT_InitCAN(canREG1, 500);
    if (nRet == -1){
        HTT_Printf("#A CANLoopTest, [Error]no 0ch Init\n");
    }

    nRet = HTT_InitCAN(canREG2, 500);
    if (nRet == -1){
        HTT_Printf("#B CANLoopTest, [Error]no 1ch Init\n");
    }

    HTT_SetCANPower(CAN_POWER_ENABLE);

    tBoardID = HTT_GetBoardID();

    if (tBoardID == SDOU_A){
        canid1 = 0x104U;
        canid2 = 0x105U;
    }
    else if (tBoardID == SDOU_B){
        canid1 = 0x104U;
        canid2 = 0x105U;
    }

    memset(temp_rx_buf, 0x00, sizeof(temp_rx_buf));

    dp_EvCan_Title(Sub_Test_Title, tBoardID, temp_rx_buf);

    while(0 < nFlag){
        atp_can_ret = HTT_ReadCAN(canREG1, canid1, temp_rx_buf, 100U);
        if(atp_can_ret > 0){
            if(memcmp(temp_rx_buf, bufack, atp_can_ret ) == 0){
                nRet = HTT_WriteCAN(canREG1, canid2, bufack, 8);
                Timecnt.wait = 0;
                nFlag = 0;
            }
        }

        if(Timecnt.wait > WAITTIME){
            nFlag = 0;
        }
    }

    while(1) {
        /* CPU #1 <=> SDOU #1 */
        memset(temp_rx_buf, 0x00, sizeof(temp_rx_buf));

        atp_can_ret = HTT_ReadCAN(canREG1, canid1, temp_rx_buf, 100U);

        if(atp_can_ret > 0) {
            dp_EvCan_Title(Sub_Test_Title, tBoardID, temp_rx_buf);
            atp_can_ret = HTT_WriteCAN(canREG1, canid2, temp_rx_buf, atp_can_ret);
            Timecnt.wait = 0;
        }

        if(Timecnt.wait > WAITTIME){
            HTT_Printf("   -----------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(1000);
            if((key == 'q') || (key == 'Q')) {
                Timecnt.wait =0;
                break;
            }
        }
    }
}
#endif


#ifdef  SUB_ASYNC_USE
void subrack_async_test(void) {
    /* SDOU #A <=> SDOU #B */

    uint8 nFlag = 1;
    uint32 j;
    uint8 key=0;

    sint32 rx_async_ret = 0;

    uint8 rx_async_buf[256];
    uint8 tx_async_buf[256];

    uint8 RACKASYNC_TEST_Title[30]={"Sub_Rack_Async_Test"};

    uint8 ASYNC_PATTERN[4] = {
            {0x11},
            {0x55},
            {0xAA},
            {0xFF}
    };
    uint8 ACK_PATTERN[8] = {'s','t','a','r','t','s','c','i'};

    HTT_InitAsync(sciREG, 115200);
    HTT_InitAsync(scilinREG, 115200);

    tBoardID = HTT_GetBoardID();

    Timecnt.wait = 0;
    tOk = 0;
    tFail = 0;
    key=0;

    dp_Evcom_Title(RACKASYNC_TEST_Title, tBoardID, tOk, tFail);

    while(1){
    delayMS(1);
    if(tBoardID == SDOU_A){

            while(nFlag){
                memcpy(tx_async_buf, ACK_PATTERN, sizeof(ACK_PATTERN));
                memset(rx_async_buf, 0x00, sizeof(rx_async_buf));

                HTT_WriteAsync(sciREG, tx_async_buf, sizeof(ACK_PATTERN));
                delayMS(10);
                rx_async_ret = HTT_ReadAsync(sciREG, rx_async_buf, ASYNC_WAIT_TIMEOUT);

                if(rx_async_ret > 0) {
                    if(memcmp(tx_async_buf, rx_async_buf, rx_async_ret) == 0) {
                        nFlag=0;
                    }
                }
            }

            if(j==0 || j==2){
                memset(tx_async_buf, ASYNC_PATTERN[j], sizeof(tx_async_buf));
                tx_async_buf[0] = 0x01;

                HTT_WriteAsync(sciREG, tx_async_buf, 41);

                memset(rx_async_buf, 0x00, sizeof(rx_async_buf));
                rx_async_ret = HTT_ReadAsync(sciREG, rx_async_buf, ASYNC_WAIT_TIMEOUT);
            }else{
                memset(tx_async_buf, ASYNC_PATTERN[j], sizeof(tx_async_buf));
                tx_async_buf[0] = 0x02;

                HTT_WriteAsync(scilinREG, tx_async_buf, 41);

                memset(rx_async_buf, 0x00, sizeof(rx_async_buf));
                rx_async_ret = HTT_ReadAsync(scilinREG, rx_async_buf, ASYNC_WAIT_TIMEOUT);
            }

            if(rx_async_ret > 0) {

                if(memcmp(tx_async_buf, rx_async_buf, 41) == 0) {
                    tOk++;
                }else{
                    tFail++;
                }
            }else{
                tFail++;
            }

            dp_Evcom_Title(RACKASYNC_TEST_Title , tBoardID, tOk, tFail);
            HTT_Printf("   -------------------------------------------------------\r\n");
            HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
            key = bspGetKey(1000);

            if((key == 'q') || (key == 'Q')) {
                break;
            }

            j = (j + 1) % 4;

    }else if(tBoardID == SDOU_B){

            while(nFlag){
                memset(rx_async_buf, 0x00, sizeof(rx_async_buf));
                rx_async_ret = HTT_ReadAsync(sciREG, rx_async_buf, ASYNC_WAIT_TIMEOUT);

                if(rx_async_ret > 0) {
                    if(memcmp(rx_async_buf, ACK_PATTERN, rx_async_ret) == 0) {
                        HTT_WriteAsync(sciREG, rx_async_buf, rx_async_ret);
                        nFlag=0;
                    }
                }
            }

            memset(rx_async_buf, 0x00, sizeof(rx_async_buf));
            rx_async_ret = HTT_ReadAsync(sciREG, rx_async_buf, 25);

            if(rx_async_ret > 0){
                if(rx_async_buf[0] == 0x01){
                   HTT_WriteAsync(sciREG, rx_async_buf, rx_async_ret);
                   Timecnt.wait = 0;
                }
            }

            memset(rx_async_buf, 0x00, sizeof(rx_async_buf));
            rx_async_ret = HTT_ReadAsync(scilinREG, rx_async_buf, 25);

            if(rx_async_ret > 0){
                if(rx_async_buf[0] == 0x02){
                   HTT_WriteAsync(scilinREG, rx_async_buf, rx_async_ret);
                   Timecnt.wait = 0;
                }
            }

            if(Timecnt.wait > 10000){
                HTT_Printf("   -----------------------------------------------\r\n");
                HTT_Printf("   Please Input Menu Key [q : Quit]\r\n");
                key = bspGetKey(1000);
                if((key == 'q') || (key == 'Q')) {
                    HTT_FlushAsync(sciREG);
                    HTT_FlushAsync(scilinREG);
                    break;
                }
            }

        }
    }

}
#endif


void sub_test(void) {

    uint8 menu_key = 0U;

    while(1) {
        menu_key = dp_Subrack_Title();
        switch(menu_key) {
#ifdef SUB_SYNC_USE
        case TEST_S_SYNC:
            subrack_sync_test();
            break;
#endif

#ifdef SUB_LCAN_USE
        case TEST_S_LCAN:
            subrack_loopcan_test();
            break;
#endif

#ifdef SUB_CCAN_USE
        case TEST_S_CCAN:
            subrack_can_test();
            break;
#endif

#ifdef SUB_ASYNC_USE
        case TEST_S_ASYNC:
            subrack_async_test();
            break;
#endif
        case TEST_S_BACK:
            return;

        }
    }
}
















