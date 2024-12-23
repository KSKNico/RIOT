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

/* tm1637 driver struct */
typedef struct {
    /* configuration parameters */
    tm1637_params_t params;
} tm1637_t;


/**
 * @brief Initializes the tm1637 device.
 * 
 * @param dev Driver structure to initialize
 * @param params Configuration parameters
 * @return 0 on success, -1 on error
 */
int tm1637_init(tm1637_t *dev, const tm1637_params_t *params);

/**
 * @brief Writes an integer to the display's register.
 * 
 * @note The integer can't be bigger than 9999 or smaller than
 * -999 as only 4 digits can be displayed at a time. 
 * With the leading zeros enabled, the display is padded with zeros.
 * For negative integers the leading zeros are added between the minus sign
 * and the number.
 * 
 * 
 * 
 * @param dev Device to write to
 * @param number Number to write. Needs to be between 9999 and -999
 * @param dots If enabled, will display dots
 * @param leading_zeros If enabled, will display leading zeros
 */
void tm1637_write_number(const tm1637_t *dev, int16_t number, bool dots, bool leading_zeros);

/**
 * @brief clears the display
 * 
 * @param dev Device to clear
 */
void tm1637_clear(const tm1637_t *dev);


#endif 