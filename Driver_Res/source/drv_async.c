/*
 * drv_async.c
 *
 *  Created on: 2022. 1. 24.
 *      Author: supgns03
 */

#include "drv_async.h"

extern void sciNotification(sciBASE_t *sci, uint32 flags);

static sint32 check_async_ch(sciBASE_t *async_ch) {
    sint32 check_async_ch_result = Result_Fail;

    if((async_ch == sciREG) || (async_ch == scilinREG)) {
    	check_async_ch_result = Result_OK;
    }

    return check_async_ch_result;
}

static sint32 check_async_rate(uint32 async_rate) {
    sint32 check_async_rate_result = Result_Fail;

    switch(async_rate) {
        case 9600:
        case 19200:
        case 38400:
        case 115200:
        	check_async_rate_result = Result_OK;
        break;

        default:
        	check_async_rate_result = Result_Fail;
        break;
    }

    return check_async_rate_result;
}

static sint32 check_async_size(uint32 async_size) {
    sint32 check_async_size_result = Result_Fail;

    if(async_size <= ASYNC_DATA_MAX) {
    	check_async_size_result = Result_OK;
    }

    return check_async_size_result;
}

static sint32 check_async_time(uint32 async_time) {
    sint32 check_async_time_result = Result_Fail;

    if(async_time <= ASYNC_WAIT_TIMEOUT) {
    	check_async_time_result = Result_OK;
    }

    return check_async_time_result;
}

sint32 AsyncSetInit(sciBASE_t *init_async_ch, uint32 init_async_rate) {
    sint32 asyncsetinit_ret = Result_OK;
    uint32 uint_vclk = 80 * 1000000;
    uint32 f = 0;

    if(check_async_ch(init_async_ch) != Result_OK){
        asyncsetinit_ret = Result_Fail;
    }
    if(check_async_rate(init_async_rate) != Result_OK){
        asyncsetinit_ret = Result_Fail;
    }

    if(asyncsetinit_ret == Result_OK) {
        /** @b initialize @b SCI */
        /** - bring SCI out of reset */
        init_async_ch->GCR0 = 0U;
        init_async_ch->GCR0 = 1U;

        /** - Disable all interrupts */
        init_async_ch->CLEARINT    = 0xFFFFFFFFU;
        init_async_ch->CLEARINTLVL = 0xFFFFFFFFU;   /** - Interrupt Level is mapped to the INT1 Line  */

        /** - global control 1 */
        init_async_ch->GCR1 = ((uint32)1U << 25U)       /* enable transmit */
                            | ((uint32)1U << 24U)       /* enable receive */
                            | ((uint32)1U << 5U)        /* internal clock (device has no clock pin) */
                            | ((uint32)(1U-1U) << 4U)   /* number of stop bits */
                            | ((uint32)0U << 3U)        /* even parity, otherwise odd */
                            | ((uint32)0U << 2U)        /* enable parity */
                            | ((uint32)1U << 1U);       /* asynchronous timing mode */

        /** - set baudrate */
        //  init_async_ch->BRS = 520U;  /* baudrate */
        f = ((init_async_ch->GCR1 & 2U) == 2U) ? (uint32)16U : (uint32)1U;
        init_async_ch->BRS = (uint_vclk /(f * (init_async_rate + 1U)));

        /** - transmission length */
        init_async_ch->FORMAT = 8U - 1U;  /* length */

        /** - set SCI pins functional mode */
        init_async_ch->PIO0 = (1U << 2U)  /* tx pin */
                            | (1U << 1U)  /* rx pin */
                            | (0U);  /* clk pin */

        /** - set SCI pins default output value */
        init_async_ch->PIO3 = (0U << 2U)  /* tx pin */
                            | (0U << 1U)  /* rx pin */
                            | (0U);  /* clk pin */

        /** - set SCI pins output direction */
        init_async_ch->PIO1 = (1U << 2U)  /* tx pin */
                            | (0U << 1U)  /* rx pin */
                            | (0U);  /* clk pin */

        /** - set SCI pins open drain enable */
        init_async_ch->PIO6 = (0U << 2U)  /* tx pin */
                            | (0U << 1U)  /* rx pin */
                            | (0U);  /* clk pin */

        /** - set SCI pins pullup/pulldown enable */
        init_async_ch->PIO7 = (0U << 2U)  /* tx pin */
                            | (0U << 1U)  /* rx pin */
                            | (0U);  /* clk pin */

        /** - set SCI pins pullup/pulldown select */
        init_async_ch->PIO8 = (1U << 2U)  /* tx pin */
                            | (1U << 1U)  /* rx pin */
                            | (1U);  /* clk pin */

        /** - set interrupt level */
        init_async_ch->SETINTLVL = ((uint32)0U << 26U)  /* Framing error */
                                 | ((uint32)0U << 25U)  /* Overrun error */
                                 | ((uint32)0U << 24U)  /* Parity error */
                                 | ((uint32)0U << 9U)  /* Receive */
                                 | ((uint32)0U << 8U)  /* Transmit */
                                 | ((uint32)0U << 1U)  /* Wakeup */
                                 | ((uint32)0U << 0U);  /* Break detect */

        /** - set interrupt enable */
        init_async_ch->SETINT = ((uint32)0U << 26U)  /* Framing error */
                              | ((uint32)0U << 25U)  /* Overrun error */
                              | ((uint32)0U << 24U)  /* Parity error */
                              | ((uint32)1U << 9U)  /* Receive */
                              | ((uint32)0U << 1U)  /* Wakeup */
                              | ((uint32)0U << 0U);  /* Break detect */

        /** - initialize global transfer variables */
        AsyncSetFlush(init_async_ch);

        /** - Finaly start SCI */
        init_async_ch->GCR1 |= (1U << 7U);

    }

    delayMS(100U);

    return asyncsetinit_ret;
}


sint32 AsyncWrite(sciBASE_t *wr_async_ch, uint8 wr_async_buf[], uint32 wr_async_size) {
    sint32 asyncwrite_ret = Result_OK;

    uint32 index = (wr_async_ch == sciREG) ? 0U : 1U;
    uint32 buf_index = 0;
    uint32 temp = 0;

    if(check_async_ch(wr_async_ch) != 0){
        asyncwrite_ret = Result_Fail;
    }
    if(check_async_size(wr_async_size) != 0){
        asyncwrite_ret = Result_Fail;
    }

    if(asyncwrite_ret == Result_OK) {

        /** -Wait received end */
        while((g_sciStruct[index].mode & SCI_RX_INT) != 0) {
            ;
        }

        wr_async_ch->CLEARINT |= SCI_RX_INT;  /* RX Interrupt Disable of own. */

        /* RTS Set for transmit */
        if(wr_async_ch == sciREG) {
            HET1SetPinStaus(ASYNC1_RTS, RTS_L);
        }
        else {
            HET1SetPinStaus(ASYNC2_RTS, RTS_L);
        }
        delayMS(1U);

        /* send the data */
        while ((wr_async_size--) > 0U)
        {
            while ((wr_async_ch->FLR & SCI_TX_INT) == 0U) { } /* Wait */
            wr_async_ch->TD = wr_async_buf[buf_index];
            buf_index++;
        }
        while ((wr_async_ch->FLR & SCI_TX_EMPTY) == 0U) { } /* Wait */

        for(temp = 0; temp < 200U; temp++) { }  /* 24us Delay ?? */

        delayMS(1U);
        /* RTS Reset for transmit */
        if(wr_async_ch == sciREG) {
            HET1SetPinStaus(ASYNC1_RTS, RTS_H);
        }
        else {
            HET1SetPinStaus(ASYNC2_RTS, RTS_H);
        }

        wr_async_ch->SETINT |= SCI_RX_INT;    /* RX Interrupt Enable of own. */

        AsyncSetFlush(wr_async_ch);
    }
    else {
        asyncwrite_ret =-1;
    }

    return asyncwrite_ret;
}

sint32 AsyncRead(sciBASE_t *re_async_ch, uint8 re_async_buf[], uint32 re_async_timeout) {
    sint32 asyncread_ret = Result_OK;
    uint32 read_len = 0U;
    sint32 readasync_break = 0;

    uint32 astep=0;

    if(check_async_ch(re_async_ch) != 0){
        asyncread_ret = Result_Fail;
    }
    if(check_async_time(re_async_timeout) != 0){
        asyncread_ret = Result_Fail;
    }

    if(asyncread_ret == Result_OK) {
        if ((re_async_ch->SETINT & SCI_RX_INT) == SCI_RX_INT) {
            /* we are in interrupt mode */
            uint32 index = (re_async_ch == sciREG) ? 0U : 1U;

            while((g_sciStruct[index].mode & SCI_RX_INT) != 0){
                ; 
            }

            while(readasync_break == 0) {
                if(g_sciStruct[index].rx_length == 0) {
                    if(bspTimerCheck(re_async_timeout) == 0) { /* Timer Check Start and Checking */
                        asyncread_ret = Result_Fail;
                        readasync_break = -1;   //break;
                    }
                }
                else { /* if(g_sciStruct[index].rx_length != 0) */
                    while(1){
                        if(astep == 0){
                            read_len = g_sciStruct[index].rx_length;
                            astep=1;
                        }
                        else if(astep==1){
                            //delayMS(10);
                            delayMS(2);
                            if(read_len == g_sciStruct[index].rx_length){
                                astep=2;
                            }else{
                                astep=0;
                            }
                        }
                        else if(astep==2){
                            memcpy(re_async_buf, g_sciStruct[index].rx_data, read_len);
                            asyncread_ret = (sint32)read_len;
                            break;
                        }
                    }
                    astep=0;
                    /* clear error flags */
                    re_async_ch->FLR = ((uint32) SCI_FE_INT | (uint32) SCI_OE_INT | (uint32) SCI_PE_INT);
                    readasync_break = -1;   //break;
                }
            }
            bspTimerCheck(0x00U);      /* Timer Check End */
        }
        else {
            asyncread_ret = Result_Fail;
        }
    }
    
    AsyncSetFlush(re_async_ch);
    return asyncread_ret;
}

sint32 AsyncSetFlush(sciBASE_t *flush_async_ch) {
    sint32 asyncsetflush_ret = Result_OK;
    uint32 index = (flush_async_ch == sciREG) ? 0U : 1U;

    if(check_async_ch(flush_async_ch) == Result_OK) {
        g_sciStruct[index].mode = (uint32)0U << 8U;
        g_sciStruct[index].rx_length = 0U;
        memset(g_sciStruct[index].rx_data, 0x00, sizeof(g_sciStruct[index].rx_data));
    }
    else {
        asyncsetflush_ret = Result_Fail;
    }

    return asyncsetflush_ret;
}

sint32 AsyncGetState(sciBASE_t *sts_async_ch) {
    sint32 asyncgetsts_ret = Result_OK;

    if(check_async_ch(sts_async_ch) == 0) {
        asyncgetsts_ret = (sint32)sts_async_ch->FLR;
    }
    else {
        asyncgetsts_ret = Result_Fail;
    }

    return asyncgetsts_ret;
}

/**
*   @fn void sciNotification(sciBASE_t *sci, uint32 flags)
*   @brief Message notification
*   @param[in] sci : Node Pointer to Async node:
*       - sciREG: SCI1 node pointer
*       - scilinREG: SCI2 node pointer
*   @param[in] flags : Interrupt number of Async node:
*       - SCI_TX_INT: Async TX Interrupt
*       - SCI_RX_INT: Async RX Interrupt
*   @return N/A:
*   @remarks This function is called when interrupt occurs.
*/
void sciNotification(sciBASE_t *sci, uint32 flags) {
    uint32 index = sci == sciREG ? 0U : 1U;
    uint32 notfi_index = 0;

    if((flags & SCI_RX_INT) == SCI_RX_INT) {    /* receive */
        g_sciStruct[index].mode = SCI_RX_INT;

        uint32 byte = (sci->RD & 0x000000FFU);

        notfi_index = g_sciStruct[index].rx_length;
        g_sciStruct[index].rx_data[notfi_index] = (uint8)byte;
        g_sciStruct[index].rx_length++;

        g_sciStruct[index].rx_length = g_sciStruct[index].rx_length % ASYNC_DATA_MAX;

        /* clear error flags */
        sci->FLR = ((uint32) SCI_FE_INT | (uint32) SCI_OE_INT | (uint32) SCI_PE_INT);

        g_sciStruct[index].mode = 0;
    }
}

