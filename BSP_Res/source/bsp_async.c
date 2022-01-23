/**
* @file     bsp_async.c
* @brief    Doppler Sensor Communication Function Definition File.
* @date     22.01.24
* @author   supgns03
* @version  00.2.0
* @vender   (c) VCTech R&D Center.
*
* This file contains:
* - function
*
* SCI1 RS485 Communication
* SCI2 RS485 Communication
*
* Configure driver code generation:
*    @ Driver Enabable
*   - Enable SCI1, SCI2 driver
*
*    @ Global (Common)
*    - TX INT Enable
*    - RX INT Enable
*
*    @ Data Format (Common)
*    - baudrate : 9600, 19200, 38400, 115200 (Select)
*    - stop bit : 1
*
*/

/* driver include */
#include "bsp_async.h"

/**
*	@fn HTT_InitAsync(sciBASE_t *async_init_node, uint32 async_init_rate)
*   @brief Initializes Async Driver
*	@param[in] async_init_node : Node point to Async node.
*		- sciREG    : SCI1 node pointer
*		- scilinREG : SCI2 node pointer
*	@param[in] async_init_rate : Bit rate of Ascyn node
*		- 9600 : 9600 bps
*		- 19200 : 19200 bps
*		- 38400 : 38400 bps
*		- 115200 : 115200 bps
*   @return The function will return:
*		- 0 : The Initialize of the Async driver the TX and  the RX was successful.
*		- -1: The Initialize of the Async driver the TX and  the RX wasn't successful.
*   @remarks This function initializes the Async driver.
*/
sint32 HTT_InitAsync(sciBASE_t *async_init_node, uint32 async_init_rate) {

    sint32 initasync_ret;

    initasync_ret = AsyncSetInit(async_init_node, async_init_rate);

	return initasync_ret;
}

/**
*   @fn sint32 HTT_ReadAsync(sciBASE_t *async_re_node, uint8 async_re_buf[], uint32 async_re_timeout)
*   @brief Recevied a Async message
*   @param[in] async_re_node : Node point to Async node.
*       - sciREG    : SCI1 node pointer
*       - scilinREG : SCI2 node pointer
*   @param[in] async_re_buf : Data Pointer to Async RX data.
*   @param[in] async_re_timeout : timeout to Async RX data.
*   @return The function will return:
*       - Length: Receive Data length of the Async driver the RX Data.
*       - -1    : The Received of the Async driver the RX Data wasn't successful.
*   @remarks This function reads from Async Driver using SCI1 and SCI2.
*/
sint32 HTT_ReadAsync(sciBASE_t *async_re_node, uint8 async_re_buf[], uint32 async_re_timeout) {
    sint32 readasync_ret = 0;

    readasync_ret = AsyncRead(async_re_node, async_re_buf, async_re_timeout);

    return readasync_ret;
}

/**
*   @fn sint32 HTT_WriteAsync(sciBASE_t *async_wr_node, uint8 async_wr_buf[], uint32 async_wr_size)
*   @brief Transmits a Async message
*   @param[in] async_wr_node : Node point to Async node.
*       - sciREG    : SCI1 node pointer
*       - scilinREG : SCI2 node pointer
*   @param[in] async_wr_buf : Data Pointer to Async TX data.
*   @param[in] async_wr_size : Data size to Async TX data.
*   @return The function will return:
*       - 0 : The Transmits of the Asybc driver the TX Data was successful.
*       - -1: The Transmits of the Async driver the TX Data wasn't successful.
*   @remarks This function writes to Async Driver using SCI1 and SCI2.
*/
sint32 HTT_WriteAsync(sciBASE_t *async_wr_node, uint8 async_wr_buf[], uint32 async_wr_size) {
    sint32 writeasync_ret = 0;

    writeasync_ret = AsyncWrite(async_wr_node, async_wr_buf, async_wr_size);

    return writeasync_ret;
}

/**
*	@fn void HTT_FlushAsync(sciBASE_t *async_flash_node)
*   @brief Clear the internal buffer of Async Driver.
*   @param[in] async_flash_node : node Pointer to Async node:
*		- sciREG    : SCI1 node pointer
*		- scilinREG : SCI2 node pointer
*   @return N/A:
*   @remarks This function clear the internal buffer of Async Driver.
*/
sint32 HTT_FlushAsync(sciBASE_t *async_flash_node) {
    sint32 flashasync_ret = 0;

    flashasync_ret = AsyncSetFlush(async_flash_node);

    return flashasync_ret;
}

/**
*	@fn sint32 HTT_StateAsync(sciBASE_t *async_se_node)
*   @brief Get state Async Commnuication.
*	@param[in] async_wr_node : Node point to Async node.
*		- sciREG    : SCI1 node pointer
*		- scilinREG : SCI2 node pointer
*   @return The function will return:
*		- The Value of Async Status Register.
*   @remarks This function is read the SCI FLR Register of SCI Controller.
*/
sint32 HTT_StateAsync(sciBASE_t *async_se_node) {
	sint32 stateasync_ret = 0;

    stateasync_ret = AsyncGetState(async_se_node);

	return stateasync_ret;
}
