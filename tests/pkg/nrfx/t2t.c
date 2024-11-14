#include "nrfx_nfct.h"
#include "container.h"
#include "log.h"
#include "panic.h"
#include "cpu.h"

#include "t2t.h"

#define BUFFER_SIZE 256

static nfct_type_2_tag_t *tag;
static event_queue_t event_queue;

/*
static uint8_t receive_data_size = 0;
static uint8_t transmit_data_size = 0;
*/

static bool initialized = false;
static bool enabled = false;
static bool selected = false;

static uint8_t data_buffer_rx[BUFFER_SIZE];
static uint8_t data_buffer_tx[BUFFER_SIZE];
static const uint32_t buffer_size = BUFFER_SIZE;

// static nrfx_nfct_data_desc_t data_desc_tx;
// static nrfx_nfct_data_desc_t data_desc_rx;

static t2t_receive_event_t receive_event;
// static t2t_transmit_event_t transmit_event;
static event_t select_event;
static event_t end_of_transmission_event;
static event_t field_detected_event;
static event_t start_of_rx_event;

static void configure_autocolres(uint8_t *nfctid1, uint8_t nfctid1_size) {
    nrfx_nfct_nfcid1_t nfcid1 = {
        .p_id = nfctid1,
        .id_size = nfctid1_size,
    };

    nrfx_nfct_param_t param = {
        .id = NRFX_NFCT_PARAM_ID_NFCID1,
        .data.nfcid1 = nfcid1
    };
    nrfx_nfct_parameter_set(&param);
    nrfx_nfct_autocolres_enable();
}

// send the data at the given block address
// always sends 16 bytes
static void process_read_command(uint8_t block_address) {
    uint8_t start_block = block_address;
    uint8_t end_block = block_address + 4;

    uint8_t start_byte = start_block * T2T_BLOCK_SIZE;
    uint8_t end_byte = end_block * T2T_BLOCK_SIZE;


    assert(end_byte - start_byte == 16);

    // Copy 16 bytes of data from the tag's memory to the response buffer
    // unless there are no more bytes left in the tag's memory
    for (uint32_t i = 0; i < 16; i++) {
        data_buffer_tx[i] = tag->memory[start_byte + i];
    }

    // Set up the data descriptor for the response
    nrfx_nfct_data_desc_t data_desc_tx = {
        .p_data = data_buffer_tx,
        .data_size = 16
    };

    // Transmit the response
    nrfx_err_t error = nrfx_nfct_tx(&data_desc_tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
    assert(error == NRFX_SUCCESS);
} 

static void parse_received_data(const uint8_t *data, uint32_t size) {
    if (size == 0) {
        LOG_DEBUG("Received empty data\n");
        return;
    } else if (size == 1) {
        LOG_DEBUG("Received command byte only\n");
        return;
    }

    uint8_t command = data[0];
    uint8_t *data_buffer = (uint8_t *) data + 1;
    uint8_t data_size = size - 1;

    if (command == T2T_READ_COMMAND) {
        uint8_t block_address = data[1];
        LOG_DEBUG("Read command received for block %d\n", block_address);
        process_read_command(block_address);
    } else if (command == T2T_HALT_COMMAND) {
        LOG_DEBUG("Halt command received\n");
        selected = false;
    } else {
        LOG_WARNING("Unknown command received (0x%02X)\n", command);
    }
}

static void receive_handler(event_t * event) {
    t2t_receive_event_t *e = container_of(event, t2t_receive_event_t, super);
    parse_received_data(data_buffer_rx, e->size);
}

/*
static void transmit_handler(event_t * event) {
    t2t_transmit_event_t *e = container_of(event, t2t_transmit_event_t, super);
    memset(data_buffer_tx, 0, buffer_size);
    nrfx_nfct_data_desc_t data_desc_tx = {
        .p_data = data_buffer_tx,
        .data_size = e->size
    };

    nrfx_err_t error = nrfx_nfct_tx(&data_desc_tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
}
*/

static void end_of_tx_handler(event_t * event) {
    memset(data_buffer_rx, 0, buffer_size);
    nrfx_nfct_data_desc_t data_desc_rx = {
        .p_data = data_buffer_rx,
        .data_size = buffer_size
    };
    nrfx_nfct_rx(&data_desc_rx);
}

static void start_of_rx_handler(event_t * event) {
    LOG_DEBUG("Receiving data\n");
}

static void selected_handler(event_t * event) {
    memset(data_buffer_rx, 0, buffer_size);
    nrfx_nfct_data_desc_t data_desc_rx = {
        .p_data = data_buffer_rx,
        .data_size = buffer_size
    };
    selected = true;
    nrfx_nfct_rx(&data_desc_rx);
}

static void field_detected_handler(event_t * event) {
    LOG_DEBUG("[EVENT] Field detected\n");
    // nrfx_nfct_state_force(NRFX_NFCT_STATE_ACTIVATED);
}

static void field_lost_handler(event_t * event) {
    LOG_DEBUG("[EVENT] Field lost\n");
    selected = false;
}

void print_state(nrf_nfct_tag_state_t state) {
    /*
    NRF_NFCT_TAG_STATE_DISABLED 	
    NRF_NFCT_TAG_STATE_RAMP_UP 	
    NRF_NFCT_TAG_STATE_IDLE 	
    NRF_NFCT_TAG_STATE_RECEIVE 	
    NRF_NFCT_TAG_STATE_FRAME_DELAY 	
    NRF_NFCT_TAG_STATE_TRANSMIT 	
    */

   if (state == NRF_NFCT_TAG_STATE_DISABLED) {
       LOG_DEBUG("Tag is disabled\n");
   } else if (state == NRF_NFCT_TAG_STATE_RAMP_UP) {
       LOG_DEBUG("Tag is ramping up\n");
   } else if (state == NRF_NFCT_TAG_STATE_IDLE) {
       LOG_DEBUG("Tag is idle\n");
   } else if (state == NRF_NFCT_TAG_STATE_RECEIVE) {
       LOG_DEBUG("Tag is receiving data\n");
   } else if (state == NRF_NFCT_TAG_STATE_FRAME_DELAY) {
       LOG_DEBUG("Tag is counting frame delay time\n");
   } else if (state == NRF_NFCT_TAG_STATE_TRANSMIT) {
       LOG_DEBUG("Tag is transmitting data\n");
   }
}

// this is the irq callback function that is called by the nfct driver
void irq_event_handler(nrfx_nfct_evt_t const * event) {
    // print_state(nrf_nfct_tag_state_get(NRF_NFCT));
    LOG_DEBUG("[IRQ] ");
    if (event->evt_id == NRFX_NFCT_EVT_RX_FRAMEEND) {
        LOG_DEBUG("Received frame of size %lu\n", event->params.rx_frameend.rx_data.data_size);
        receive_event.super.handler = receive_handler;
        receive_event.size = event->params.rx_frameend.rx_data.data_size;
        event_post(&event_queue, (event_t *) &receive_event);   
    } else if (event->evt_id == NRFX_NFCT_EVT_TX_FRAMEEND) {
        LOG_DEBUG("End of transmission\n");
        end_of_transmission_event.handler = end_of_tx_handler;
        event_post(&event_queue, &end_of_transmission_event);
    } else if (event->evt_id == NRFX_NFCT_EVT_SELECTED) {
        LOG_DEBUG("Field has been selected\n");
        select_event.handler = selected_handler;
        event_post(&event_queue, &select_event);
    } else if (event->evt_id == NRFX_NFCT_EVT_TX_FRAMESTART) {
        LOG_DEBUG("Transmitting data\n");
    } else if (event->evt_id == NRFX_NFCT_EVT_RX_FRAMESTART) {
        start_of_rx_event.handler = start_of_rx_handler;
        event_post(&event_queue, &start_of_rx_event);
    } else if (event->evt_id == NRFX_NFCT_EVT_ERROR) {
        nrfx_nfct_error_t error = event->params.error.reason;
        if (error == NRFX_NFCT_ERROR_FRAMEDELAYTIMEOUT) {
            if (selected) {
                LOG_DEBUG("ERROR: %X\n", event->params.error.reason);
            }
        }
        
    } else if (event->evt_id == NRFX_NFCT_EVT_FIELD_DETECTED) {
        LOG_DEBUG("Field detected\n");
        field_detected_event.handler = field_detected_handler;
        event_post(&event_queue, &field_detected_event);
    } else if (event->evt_id == NRFX_NFCT_EVT_FIELD_LOST) {
        LOG_DEBUG("Field lost\n");
        selected = false;
    } else {
        LOG_WARNING("Unknown event received\n");
    }
}




void initialize_t2t(nfct_type_2_tag_t *_tag, bool autocolres, uint8_t *nfcid1, uint8_t nfcid1_size) {
    if (initialized) {
        LOG_WARNING("T2T already initialized\n");
        return;
    }

    tag = _tag;
    event_queue_init(&event_queue);
    initialized = true;
    nrfx_nfct_config_t config = {
        .rxtx_int_mask = NRFX_NFCT_EVT_FIELD_DETECTED |
                         NRFX_NFCT_EVT_ERROR          |
                         NRFX_NFCT_EVT_SELECTED       |
                         NRFX_NFCT_EVT_FIELD_LOST     |
                         NRFX_NFCT_EVT_RX_FRAMEEND    |
                         NRFX_NFCT_EVT_RX_FRAMESTART  |
                         NRFX_NFCT_EVT_TX_FRAMEEND    |
                         NRFX_NFCT_EVT_TX_FRAMESTART 
                         /* NRF_NFCT_INT_RXERROR_MASK    |
                         NRF_NFCT_RX_FRAME_STATUS_CRC_MASK    |
                         NRF_NFCT_RX_FRAME_STATUS_PARITY_MASK |
                         NRF_NFCT_RX_FRAME_STATUS_OVERRUN_MASK |
                         NRF_NFCT_ERROR_FRAMEDELAYTIMEOUT_MASK*/
                        ,
        .cb = irq_event_handler,
        .irq_priority = 2
    };

    nrfx_err_t error = nrfx_nfct_init(&config);
    assert(error == NRFX_SUCCESS);

    LOG_DEBUG("Initialization of nfct driver successful!\n");

    if (autocolres) {
        configure_autocolres(nfcid1, nfcid1_size);
    } else {
        nrfx_nfct_autocolres_disable();
    }

    nrf_nfct_shorts_enable(NRF_NFCT, NRF_NFCT_SHORT_FIELDDETECTED_ACTIVATE_MASK | NRF_NFCT_SHORT_FIELDLOST_SENSE_MASK);


}

void enable_t2t(void) {
    if (!initialized) {
        LOG_WARNING("T2T not initialized\n");
        return;
    }

    if (enabled) {
        LOG_WARNING("T2T already enabled\n");
        return;
    }

    LOG_DEBUG("Enabling T2T\n");

    nrfx_nfct_enable();
    enabled = true;
}

void start_event_loop(void) {
    LOG_DEBUG("Starting event loop.\n");
    event_loop(&event_queue);
}