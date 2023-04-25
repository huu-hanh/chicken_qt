/*
 * frame_uart.h
 *
 *  Created on: Apr 22, 2023
 *      Author: DELL
 */

#ifndef FRAME_UART_H_
#define FRAME_UART_H_

#include "stdint.h"
#include "stdlib.h"
typedef enum
{
    FRAME_OK = 0,
    FRAME_MISS,
    FRAME_ERROR

} frame_uart_t;

#define START_BYTE 69
#define CHECK_BYTE 96
#define STOP_BYTE 196

#define FRAME_DATA          8
#define FRAME_DATA_HANDLE   FRAME_DATA * 2 + 4 // STOP + 2CRC + STOP

// #define FRAME_DATA_RX       FRAME_DATA_TX + 4
class FrameUart
{
public:
    FrameUart()
    {

    }
    void call_create_frameQt();
    frame_uart_t call_get_dataFromSTM();
    void SendFrameData(uint8_t *pu8Src, uint16_t u16Src_len, uint8_t *pu8Dest, uint16_t *pu16Dest_len);
    frame_uart_t GetFrameData(uint8_t *pu8Src, uint16_t u16Src_len, uint8_t *pu8Dest);
    uint8_t au8TxBuffer[FRAME_DATA];
    uint8_t au8RxBuffer[FRAME_DATA];
    bool bRxCpltflag;
    bool bTxCpltflag;
    int8_t i8ErrorCode;
};
#endif /* FRAME_UART_H_ */
