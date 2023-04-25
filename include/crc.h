/*
 * crc.h
 *
 *  Created on: Apr 23, 2023
 *      Author: DELL
 */

#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

uint16_t crc16_buff(const uint8_t *buf, int len);
uint16_t crc16_floating(uint8_t next, uint16_t seed);

#endif /* CRC_H_ */
