/*
 * drv_async.h
 *
 *  Created on: 2022. 1. 24.
 *      Author: supgns03
 */

#ifndef __DRV_ASYNC__H__
#define __DRV_ASYNC__H__

#include "sci.h"
#include "drv_het1.h"
#include "drv_sys.h"

#define RTS_H   1
#define RTS_L   0

#define ASYNC_DATA_MAX      256
#define ASYNC_DATA_LIMIT    (ASYNC_DATA_MAX-1)
#define ASYNC_WAIT_TIMEOUT                  100     /* 100ms wait */

#define WAIT_TIMEOUT_AFTER_ONE_CHAR_RX      2       /* 2ms wait */

enum sciIntFlags_t
{
    SCI_TX_EMPTY = 0x00000800U  /* Transmit empty flag */
};

typedef struct
{
    uint32  mode;           /* Used to check for TX interrupt Enable */
    uint32  rx_length;      /* Receive data length in number of Bytes */
    uint8   rx_data[ASYNC_DATA_MAX];    /* Receive data pointer */
    uint8   rx_data2[ASYNC_DATA_MAX];    /* Receive data pointer */
} t_sciTransfer;

static t_sciTransfer g_sciStruct[2U];


sint32 AsyncSetInit(sciBASE_t *init_async_ch, uint32 init_async_rate);
sint32 AsyncWrite(sciBASE_t *wr_async_ch, uint8 wr_async_buf[], uint32 wr_async_size);
sint32 AsyncRead(sciBASE_t *re_async_ch, uint8 re_async_buf[], uint32 re_async_timeout);
sint32 AsyncSetFlush(sciBASE_t *flush_async_ch);
sint32 AsyncGetState(sciBASE_t *sts_async_ch);

#endif /* DRIVER_RES_INCLUDE_DRV_ASYNC_H_ */
