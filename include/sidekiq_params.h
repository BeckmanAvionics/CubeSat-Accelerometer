#ifndef SIDEKIQ_PARAMS_H
#define SIDEKIQ_PARAMS_H

/*! \file sidekiq_params.h
 * \brief This file contains data structures used to determine the parameters
 * of a given Sidekiq.
 *
 * <pre>
 * Copyright 2017 Epiq Solutions, All Rights Reserved
 * </pre>
 */

#ifdef __cplusplus
extern "C" {
#endif

/***** INCLUDES *****/

#include <stdint.h>
#include <stdbool.h>
#include "sidekiq_types.h"
#include "sidekiq_xport_types.h"

/***** STRUCTS *****/

/** @brief Parameters related to a physical Sidekiq card. */
typedef struct
{
    skiq_xport_init_level_t init_level;
    /**< The initialization level of a given card. */

    skiq_part_t part_type;
    /**< The Sidekiq's part type (e.x. "mPCIe", "M.2", "X2", etc). */

    skiq_part_info_t part_info;
    /**< Vendor information related to a given part and its configuration. */

    skiq_xport_type_t xport;
    /**< Transport configuration for the Sidekiq card (e.x. "PCIe", "USB"). */

    bool is_accelerometer_present;
    /**< Boolean used to indicate if the accelerometer is physically present. */

    uint8_t card;
    /**< Card identifier used for API calls. */

    char serial_string[SKIQ_SERIAL_NUM_STRLEN];
    /**< String representation of the serial number of the Sidekiq card. */

} skiq_card_param_t;

/** @brief Parameter for the Sidekiq's on board FPGA. */
typedef struct
{
    skiq_fpga_tx_fifo_size_t tx_fifo_size;
    /**< Enumerated value of the Tx FIFO depth on the FPGA. */

    uint32_t build_date;
    /**< Date that the FPGA image was build (YYMMDDHH). */

    uint32_t git_hash;
    /**< Git commit hash of the FPGA build. */

    uint64_t sys_timestamp_freq;
    /**< Frequency at what the system timestamp runs in hertz. */

    uint8_t version_major;
    /**< Major version of the FPGA release. */

    uint8_t version_minor;
    /**< Minor version of the FPGA release. */

    uint8_t version_patch;
    /**< Patch version of the FPGA release (available in FPGA bitstreams with version 3.8 or later, =0 otherwise) */

} skiq_fpga_param_t;

/** @brief Parameters for the firmware loaded onto a Sidekiq. */
typedef struct
{
    bool is_present;
    /**< Boolean indicating if firmware is present or absent on the Sidekiq. */

    uint16_t enumeration_delay_ms;
    /**< Delay in milliseconds which firmware waits before enumerating on the
         USB bus. */

    uint8_t version_major;
    /**< Major version of the firmware release. */

    uint8_t version_minor;
    /**< Minor version of the firmware release. */

} skiq_fw_param_t;

/** @brief Parameters for the Sidekiq's RF capabilities.
    \note Must be initialized to skiq_xport_init_level_full to have access to
    certain members of this struct. */
typedef struct
{
    skiq_ref_clock_select_t ref_clock_config;
    /**< Enumerated value of the Sidekiq's reference clock configuration. */

    bool is_rf_port_fixed;
    /**< Boolean indicating if the RF ports can or can not be configured
             dynamically. */

    bool is_rf_port_tdd_supported;
    /**< Boolean indicating if Time Division Duplex is supported. */
    /**< DEPRECATED: replaced by is_rf_port_trx_supported */

    bool is_rf_port_trx_supported;
    /**< Boolean indicating if RF ports can be switched between receive/transmit modes */

    uint8_t num_rx_channels;
    /**< Total number of Rx ports on the Sidekiq. This value can be used to
         index into the skiq_rx_param_t array of skiq_param_t struct.
         \note Must be initialized to skiq_xport_init_level_full */

    uint8_t num_tx_channels;
    /**< Total number of Tx ports on the Sidekiq. This value can be used to
         index into the skiq_tx_param_t array of skiq_param_t struct.
         \note Must be initialized to skiq_xport_init_level_full */

    uint32_t ref_clock_freq;
    /**< The frequency of the reference clock in hertz */

} skiq_rf_param_t;

/** @brief Parameters for each Rx channel on a Sidekiq card.
    \note Must be initialized to skiq_xport_init_level_full to access any
    members of this struct */
typedef struct
{
    skiq_filt_t filters[skiq_filt_max];
    /**< Filters available for the given Rx channel. */

    uint16_t atten_quarter_db_max;
    /**< Maximum attenuation in quarter dB steps. */
    uint16_t atten_quarter_db_min;
    /**< Minimum attenuation in quarter dB steps. */

    uint8_t gain_index_max;
    /**< Maximum index for gain profile selection. */

    uint8_t gain_index_min;
    /**< Minimum index for gain profile selection. */

    uint8_t iq_resolution;
    /**< Number of resolution bits for each I/Q signal, that is, I is N bits
          and Q is N bits. */

    uint64_t lo_freq_max;
    /**< Maximum frequency that the LO can be tuned to in hertz. */

    uint64_t lo_freq_min;
    /**< Minimum frequency that the LO can be tuned to in hertz. */

    uint8_t num_filters;
    /**< Total number of filters available on the Rx channel. */

    uint32_t sample_rate_max;
    /**< Maximum rate at which I/Q sample clock can be driven in hertz. */

    uint32_t sample_rate_min;
    /**< Minimum rate at which I/Q sample clock can be driven in hertz. */

    uint8_t num_fixed_rf_ports;
    /**< Total number of fixed RX ports */

    skiq_rf_port_t fixed_rf_ports[skiq_rf_port_max];
    /**< list of fixed RX RF ports */

    uint8_t num_trx_rf_ports;
    /**< Total number of TRX ports  */

    skiq_rf_port_t trx_rf_ports[skiq_rf_port_max];
    /**< list of TRX RF ports  */
} skiq_rx_param_t;

/** @brief Parameters for each Tx channel on a Sidekiq card.
    \note Must be initialized to skiq_xport_init_level_full to access any
    members of this struct. */
typedef struct
{
    skiq_filt_t filters[skiq_filt_max];
    /**< Filters available for the given Tx channel. */

    uint16_t atten_quarter_db_max;
    /**< Maximum attenuation in quarter dB steps. */
    uint16_t atten_quarter_db_min;
    /**< Minimum attenuation in quarter dB steps. */

    uint8_t iq_resolution;
    /**< Number of resolution bits for each I/Q signal, that is, I is N bits
          and Q is N bits" */

    uint64_t lo_freq_max;
    /**< Maximum frequency that the LO can be tuned to in hertz. */
    uint64_t lo_freq_min;
    /**< Minimum frequency that the LO can be tuned to in hertz. */

    uint8_t num_filters;
    /**< Total number of filters available on the Tx channel. */

    uint32_t sample_rate_max;
    /**< Maximum rate at which I/Q sample clock can be driven in hertz. */

    uint32_t sample_rate_min;
    /**< Minimum rate at which I/Q sample clock can be driven in hertz. */

    uint8_t num_fixed_rf_ports;
    /**< Total number of fixed TX ports */

    skiq_rf_port_t fixed_rf_ports[skiq_rf_port_max];
    /**< list of fixed TX RF ports */

    uint8_t num_trx_rf_ports;
    /**< Total number of TRX ports  */

    skiq_rf_port_t trx_rf_ports[skiq_rf_port_max];
    /**< list of TRX RF ports  */
} skiq_tx_param_t;

/** @brief Parameters for the entire Sidekiq.
    \note Must be initialized to skiq_xport_init_level_full to access all
    members of this struct. */
typedef struct
{
    skiq_card_param_t card_param;
    skiq_fpga_param_t fpga_param;
    skiq_fw_param_t fw_param;
    skiq_rf_param_t rf_param;
    skiq_rx_param_t rx_param[skiq_rx_hdl_end];
    skiq_tx_param_t tx_param[skiq_tx_hdl_end];
} skiq_param_t;

#ifdef __cplusplus
}
#endif

#endif
