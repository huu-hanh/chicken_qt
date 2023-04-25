#include "frame_uart.h"
#include "crc.h"
#include <QDebug>
#include <QString>
#include <QChar>
void FrameUart::SendFrameData(uint8_t *pu8Src, uint16_t u16Src_len, uint8_t *pu8Dest, uint16_t *pu16Dest_len)
{
    const uint8_t *pu8Src_end = pu8Src + u16Src_len;
    const uint8_t *pu8Dest_start = pu8Dest;
    uint16_t crc = 0;

    *(pu8Dest++) = START_BYTE;
    while (pu8Src < pu8Src_end)
    {
        if (*pu8Src == START_BYTE || *pu8Src == CHECK_BYTE || *pu8Src == STOP_BYTE)
        {
            *(pu8Dest++) = CHECK_BYTE;
            *(pu8Dest++) = *pu8Src;
        }
        else
        {
            *(pu8Dest++) = *pu8Src;
        }
        crc = crc16_floating(*pu8Src, crc);
        pu8Src++;
    }
    *(pu8Dest) = (char)(crc >> 8);
    pu8Dest++;
    *(pu8Dest) = (char)crc;
    pu8Dest++;
    *(pu8Dest++) = STOP_BYTE;
    *(pu16Dest_len) = pu8Dest - pu8Dest_start;
}
frame_uart_t FrameUart::GetFrameData(uint8_t *pu8Src, uint16_t u16Src_len, uint8_t *pu8Dest)
{

    uint16_t crc_check = 0;
    char temp;
    uint8_t i = 0;
    uint8_t len_check = FRAME_DATA;
    uint8_t *pu8Src_end = pu8Src + u16Src_len;
    // Check the start byte
    while (pu8Src < pu8Src_end && *pu8Src != START_BYTE)
    {
        pu8Src++; // find the start byte
    }

    pu8Src++;
    if (pu8Src >= pu8Src_end - 2)
        return FRAME_ERROR;
    while (i < len_check)
    {
        if (*pu8Src == CHECK_BYTE) // add check-byte
        {
            temp = *(++pu8Src);
            *(pu8Dest++) = temp;
            crc_check = crc16_floating(temp, crc_check);
        }
        else
        {
            *(pu8Dest++) = *(pu8Src);
            crc_check = crc16_floating((*pu8Src), crc_check);
        }
        //        qDebug()<<"data: "<<(char)*(pu8Src);
        pu8Src++;
        i++;
    }
    //    qDebug()<<"asd: "<<(uint8_t)(crc_check >> 8);
    //    qDebug()<<"asd: "<<(uint8_t)(char)(crc_check);
    //    qDebug()<<"real: "<<*(pu8Src++);
    //    qDebug()<<"real: "<<*(pu8Src);

    // The End of data plus 2 must be stop
    if (pu8Src[2] != STOP_BYTE)
        return FRAME_MISS;
    if (*(pu8Src++) == (char)(crc_check >> 8) && *(pu8Src) == (char)crc_check)
    {
        return FRAME_OK;
    }
    // return FRAME_ERROR;
    return FRAME_OK;
}
