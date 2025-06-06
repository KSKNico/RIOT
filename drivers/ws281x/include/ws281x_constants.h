/*
 * Copyright (C) 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 * @file
 * @brief       Constants for WS2812/SK6812 RGB LEDs
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timing parameters for WS2812/SK6812 RGB LEDs
 * @{
 */

/**
 * @brief   Data transmission time in nanoseconds
 *
 * For the SK6812, WS2812 and WS2812b this is 1.25 µs. This is the total time
 * required to transmit one bit.
 */
#define WS281X_T_DATA_NS                (1250U)

/**
 * @brief The high-times in nanoseconds.
 * @{
 */
#define WS281X_T_DATA_ONE_NS            (650U)
#define WS281X_T_DATA_ZERO_NS           (325U)
/**@}*/

/**
 * @brief   Time in microseconds to pull the data line low to signal end of data
 *
 * For the WS2812 it is ≥ 50µs, for the SK6812 it is ≥ 80µs. We choose 80µs to
 * be compatible with both.
 */
#define WS281X_T_END_US                 (80U)
/**@}*/

/**
 * @name    Data encoding parameters for WS2812/SK6812 RGB LEDs
 * @{
 */
/**
 * @brief   Offset for the red color component
 */
#define WS281X_OFFSET_R                 (1U)
/**
 * @brief   Offset for the green color component
 */
#define WS281X_OFFSET_G                 (0U)
/**
 * @brief   Offset for the blue color component
 */
#define WS281X_OFFSET_B                 (2U)
/**@}*/

#ifdef __cplusplus
}
#endif

/** @} */
