/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_particle-xenon
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Particle Xenon
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#include "periph_cpu.h"
#include "periph_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 8),
        .tx_pin     = GPIO_PIN(0, 6),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
    {
        .dev        = NRF_UARTE1,
        .rx_pin     = GPIO_PIN(1, 10),
        .tx_pin     = GPIO_PIN(1, 8),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_PIN(1, 3),
        .cts_pin    = GPIO_PIN(1, 11),
#endif
        .irqn       = UARTE1_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)
#define UART_1_ISR          (isr_uarte1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
