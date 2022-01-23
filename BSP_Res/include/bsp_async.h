/**
* @file     bsp_async.c
* @brief    RS485 Communication Function Definition File.
* @date     22.01.24
* @author   supgns03
* @version  00.2.0
* @vender   (c) VCTech R&D Center.
*   
*/


#ifndef __BSP_ASYNC__H__
#define __BSP_ASYNC__H__

/* base include */
#include "drv_async.h"

sint32 HTT_InitAsync(sciBASE_t *async_init_node, uint32 async_init_rate);
sint32 HTT_WriteAsync(sciBASE_t *async_wr_node, uint8 async_wr_buf[], uint32 async_wr_size);
sint32 HTT_ReadAsync(sciBASE_t *async_re_node, uint8 async_re_buf[], uint32 async_re_timeout);
sint32 HTT_FlushAsync(sciBASE_t *async_flash_node);
sint32 HTT_StateAsync(sciBASE_t *async_se_node);

#endif /* __BSP_ASYNC__H__ */


