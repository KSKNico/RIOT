#ifndef TM1637_H
#define TM1637_H

#include "board.h"
#include "periph/gpio.h"

/* configuration paramters for the tm1637 display */
typedef struct {
    /* GPIO for the clock */
    gpio_t clk;
    /* GPIO for the data */
    gpio_t dio;
    /* brightness for the display needs to be in the range of 0 to 7 */
    uint8_t brightness;
} tm1637_params_t;

/* tm1637 driver descriptor */
typedef struct {
    /* configuration parameters */
    tm1637_params_t params;
} tm1637_t;


/**
 * @brief Initializes the tm1637 device
 * 
 * @param[out] dev device descriptor of the display
 * @param[in] params configuration parameters
 * @return 0 on success, error otherwise
 */
int tm1637_init(tm1637_t *dev, const tm1637_params_t *params);

/**
 * @brief Writes an integer to the display
 * 
 * @note The integer can't be bigger than 9999 or smaller than
 * -999 as only 4 digits can be displayed at a time. 
 * With the leading zeros enabled, the display is padded with zeros.
 * For negative integers the leading zeros are added between the minus sign
 * and the number.
 * 
 * 
 * @param[in] dev device descriptor of the display
 * @param[in] number number to write, in the range of 9999 to -999
 * @param[in] dots If enabled, displays dots
 * @param[in] leading_zeros If enabled, displays leading zeros
 */
void tm1637_write_number(const tm1637_t *dev, int16_t number, bool dots, bool leading_zeros);

/**
 * @brief Clear display
 * 
 * @param[in] dev device descriptor of the display
 */
void tm1637_clear(const tm1637_t *dev);


#endif 