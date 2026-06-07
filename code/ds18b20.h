#ifndef DS18B20_H_
#define DS18B20_H_

#include <msp430.h>
#include <stdint.h>

#define DS_DIR P2DIR
#define DS_OUT P2OUT
#define DS_IN  P2IN
#define DS_PIN BIT0

uint8_t ds18b20_reset(void);
void ds18b20_write_byte(uint8_t data);
uint8_t ds18b20_read_byte(void);
int16_t ds18b20_get_temp(void);

#endif /* DS18B20_H_ */