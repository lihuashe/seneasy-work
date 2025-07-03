
#ifndef __FIREFLY_LINK_LAYER_H
#define __FIREFLY_LINK_LAYER_H

#include "stdint.h"
#include "fyll_params.h"

typedef enum
{
    FYLL_IDLE,
    FYLL_DEVICE_ACTIVE,
    FYLL_HOST_ACTIVE,
}fyll_state_t;

typedef enum
{
    FYLL_CHANNEL_PREVIOUS_SUCCESS,
    FYLL_CHANNEL_RANDOM,
    FYLL_CHANNEL_ESTIMATED,
    FYLL_CHANNEL_NEXT_INDEX
}fyll_new_tx_ch_t;

typedef enum
{
    FYLL_DEVICE_MODE_0,
    FYLL_DEVICE_MODE_1,
    FYLL_DEVICE_MODE_2,
    FYLL_DEVICE_MODE_3,
    FYLL_DEVICE_MODE_4
}fyll_device_modes_t;

/**
 * @brief Firefly link layer initialization
 * 
 * 
*/
void fyll_init(void);

/**
 * @brief Set the radio output power.
 * @param power Telink primary link layer radio transmission power modes.
*/
void inline fyll_set_tx_power(trf_tpll_tx_power_t power)
{
    trf_tpll_set_txpower(power);
}

#if FYLL_ROLE == FYLL_HOST

/**
 * @brief Function for starting reception.
 *  This function will make the protocol enter FYLL_HOST_ACTIVE state and start 
 *  listening for incoming data.

 * @see fyll_rx_data_ready(), fyll_rx_fifo_read().
 * 
*/
void fyll_rx_start();

/**
 * @brief Function for uploading the payload to be piggybacked onto the next acknowledgement packet (ACK).
 * 
 *  It is recommended that the Firefly Link Layer is in IDLE state when uploading an acknowledgement packet. 
 *  Alternatively, the application can upload packets during HOST_ACTIVE state as long as the 
 *  TX FIFO is not empty. 
 * 
 * @param src is a pointer to the payload
 * 
 * @param lenght is the number of bytes of payload
 * 
 * @retval true if uploading of the ACK payload to TX FIFO was successfual
 * @retval false if uploading of the ACK payload falied due to the full TX FIFO or lenght above #FYLL_MAX_PAYLOAD_LENGTH
*/
bool fyll_ack_payload_write(uint8_t * src, uint8_t lenght);

#else

/**
 * @brief Function for uploading data to TX FIFO and start data transmission (Device operation).
 * 
 * The protocol does not have to be in FYLL_IDLE state to issue this function.
 * 
 * If the protocol is in FYLL_DEVICE_ACTIVE state, the data will be uploaded to the 
 * TX FIFO and sent immediately after the ongoing transmission has completed.
 * This can be used to stream data.
 * 
 * For this to happen the following must be fulfilled:
 *   The TX FIFO must not be full. (The TX FIFO can hold up to 1536B，rx fifo 512B).
 * 
 * 
 * transmission.
 * 
 * @param src is pointer of the payload to be transmitted.
 * 
 * @param length length is the number of bytes in the payload.
 * 
 * @retval true if data was successfully uploaded to the TX FIFO.
 * This does NOT indicate that the packet was successfully transmitted,
 * which should instead be checked with fyll_tx_success()
 * @retval false if data was not uploaded. In this case any the protocol
 * state remains the same as before the function was called.  
 * 
 * @see fyll_tx_success(),fyll_get_state(), fyll_goto_idle(void)
*/
bool fyll_tx_data(uint8_t * src, uint8_t length);

/**
Function for checking the result of the latest transmit operation.

The return value from this function is only reliable when the protocol 
is in FYLL_IDLE state 

@retval true if the latest packet was successfully transmitted. 
@retval false if the latest packet was not successfully transmitted.
   
@sa fyll_tx_data(), fyll_get_state(), fyll_goto_idle()
*/

/**
 * @brief Function for checking the result of the latest transmit operation.
 * 
 * The return value from this function is only reliable when the protocol 
 * is in FYLL_IDLE state 
 * 
 * @retval true if the latest packet was successfully transmitted. 
 * @retval false if the latest packet was not successfully transmitted.
*/
bool fyll_tx_success(void);

/** 
 * @brief Function used during device mode 2 for minimizing the delay from 
 * fyll_tx_data() is called and until the transmission is started.
 *
 * Device mode 2 is the synchronous frequency agility mode. This means
 * that the firefly link layer for every new transmission will use the 
 * previous successful transmission channel, and await starting the transmission 
 * until the host is actually monitoring this channel.
 * 
 * This wait time will be minimized if the fyll_tx_data() function is called
 * when fyll_device_mode_2_rx_channel_match() returns true.
*/
bool fyll_device_mode_2_rx_channel_match(void);

#endif

/** 
 * @brief Function for achieving the current protocol state.
 *
 * @return Returns the current Firefly protocol state.
 * @retval FYLL_IDLE
 * @retval FYLL_DEVICE_ACTIVE
 * @retval FYLL_HOST_ACTIVE
 *
 * @see fyll_goto_idle()
*/
fyll_state_t fyll_get_state(void);

/**
 * @brief Function for forcing the protocol to FYLL_IDLE state.
 * 
 * The Gazell protocol has three states: FYLL_IDLE, FYLL_DEVICE_ACTIVE and FYLL_HOST_ACTIVE.
 *
 * @see fyll_get_state()
*/
void fyll_go_to_idle();

/**
 * @brief Function for checking for data in the RX FIFO.
 *
 * If a packet is available, the packet can be read from the FIFO by using
 * fyll_rx_fifo_read().
 * 
 * @retval true if data is available
 * @retval false if data is not available
 * 
 * @see fyll_rx_fifo_read()
*/
bool fyll_rx_data_ready();

/**
 * @brief Function for reading a packet from the RX FIFO.
 *
 * Reading the RX FIFO during FYLL_DEVICE_ACTIVE state can cause loss of packets.
 * 
 * @param *dst is the destination for the data.
 * To avoid memory corruption the destination array should at least have space 
 * for #FYLL_MAX_PAYLOAD_LENGTH bytes.
 * 
 * @param *length returns the number of bytes read.
 * By passing a @b NULL pointer this parameter will be ignored.
 *
 * 
 * @return Status of RX FIFO read operation.
 * 
 * @retval true if data was read from the RX FIFO.
 * @retval false if RX FIFO was empty.
 *
 * @see fyll_rx_data_ready()
*/
bool fyll_rx_fifo_read(uint8_t * dst, uint8_t * length);

/**
 * @brief Function for flushing the receive (RX) FIFO.
 * 
 * The function can be used in any protocol state.
 * 
*/
void fyll_rx_fifo_flush(void);

/**
 * @brief Function for flushing the transmit (TX) FIFO. 
 * 
 * This function will only have effect in the FYLL_IDLE state.
*/
void fyll_tx_fifo_flush(void);

/**
 * @brief Function returning the receive signal level for the previous received packet
 * 
 * This function is useful for estimating the relative proximity of the transmitting device.
 *
 * @return rssi of previous received packet.
 * 
 * @see fyll_rx_start(), fyll_rx_fifo_read(), fyll_rx_data_ready()
*/
int fyll_rx_power_rssi(void);

/**
 * @brief Function to be called by the radio interrupt service routine.
 * 
*/
void fyll_radio_isr(void);

/**
 * @brief Function to be called by the radio interrupt service routine.
 * 
*/
void fyll_timer_isr(void);


void fyll_test_task(void);


bool fyll_sample_send_one_packet(uint8_t *fyll_test_data, uint8_t length );
#endif
