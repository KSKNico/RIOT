#ifndef TM1637_H
#define TM1637_H

#include "board.h"
#include "periph/gpio.h"

typedef struct {
    gpio_t clk;
    gpio_t dio;
    uint8_t brightness;
} tm1637_params_t;

typedef struct {
    tm1637_params_t params;
} tm1637_t;

int tm1637_init(tm1637_t *dev, const tm1637_params_t *params);

void tm1637_turn_on(void);

void tm1637_turn_off(void);

void tm1637_write_number(const tm1637_t *dev, int16_t number);

void tm1637_write_hex(int hex);

void tm1637_turn_on_colon(void);

void tm1637_turn_off_colon(void);


#endif 