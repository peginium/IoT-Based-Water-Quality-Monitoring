#include "ds18b20.h"

#define DELAY_US(x) __delay_cycles((x))

uint8_t ds18b20_reset(void) {
    uint8_t presence;
    DS_DIR |= DS_PIN;
    DS_OUT &= ~DS_PIN;
    DELAY_US(480);
    
    DS_DIR &= ~DS_PIN;
    DELAY_US(60);
    
    presence = (DS_IN & DS_PIN);
    DELAY_US(420);
    
    return presence;

void ds18b20_write_bit(uint8_t bit) {
    DS_DIR |= DS_PIN;
    DS_OUT &= ~DS_PIN;
    DELAY_US(2);
    
    if (bit) {
        DS_DIR &= ~DS_PIN;
    }
    DELAY_US(60);
    DS_DIR &= ~DS_PIN;
    DELAY_US(2);
}

uint8_t ds18b20_read_bit(void) {
    uint8_t bit = 0;
    DS_DIR |= DS_PIN;
    DS_OUT &= ~DS_PIN;
    DELAY_US(2);
    
    DS_DIR &= ~DS_PIN;
    DELAY_US(10);
    
    if (DS_IN & DS_PIN) {
        bit = 1;
    }
    DELAY_US(50);
    return bit;
}

void ds18b20_write_byte(uint8_t data) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        ds18b20_write_bit(data & 0x01);
        data >>= 1;
    }
}

uint8_t ds18b20_read_byte(void) {
    uint8_t i, data = 0;
    for (i = 0; i < 8; i++) {
        if (ds18b20_read_bit()) {
            data |= (1 << i);
        }
    }
    return data;
}

int16_t ds18b20_get_temp(void) {
    uint8_t lsb, msb;
    
    if (ds18b20_reset() != 0) {
        return 9999;
    }

    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0x44);

    while(ds18b20_read_bit() == 0); 

    ds18b20_reset();
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0xBE);

    lsb = ds18b20_read_byte();
    msb = ds18b20_read_byte();

    return (msb << 8) | lsb;
}