/*
* Copyright 2016-2023, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*/

/** @file
 *
 *
 * This file provides definitions for this vendor specific model.
 */

#pragma pack(1)
#ifndef PACKED
#define PACKED
#endif

#define MESH_VENDOR_CYPRESS_COMPANY_ID              0x131   // Cypress Company ID
#define MESH_VENDOR_SELF_CONFIG_MODEL_ID            0x02    // Self Config Model

// For the Cypress model 0x03, we will use opcode 0x03 for configuration message
#define MESH_VENDOR_CYPRSESS_OPCODE_CONFIG          0x03

// The first byte (sub code) defines the operation
#define MESH_VENDOR_OPCODE_CONFIGURE_SET            0x00    // Start Self Config operation
#define MESH_VENDOR_OPCODE_CONFIGURE_STATUS         0x01    // Start message received
#define MESH_VENDOR_OPCODE_CONFIGURE_COMPLETE       0x02    // Self Config completed

// The first byte (sub code) defines the operation
#define MESH_CY_VENDOR_SUBCODE_RSSI_TEST_TX_START           0x04
#define MESH_CY_VENDOR_SUBCODE_RSSI_TEST_TX_START_STATUS    0x05
#define MESH_CY_VENDOR_SUBCODE_RSSI_TEST_DATA               0x06
#define MESH_CY_VENDOR_SUBCODE_RSSI_TEST_RX_VALUES_GET      0x07
#define MESH_CY_VENDOR_SUBCODE_RSSI_TEST_RX_VALUES_STATUS   0x08

#define BEFORE_PROVISION_DELAY              5
#define PROVISION_RETRY_DELAY               2
#define EMBEDDED_PROV_MAX_NODES             64

#define EMBEDDED_PROV_LOCAL_ADDR            1

#define EMBEDDED_PROV_APP_KEY_IDX           0
#define EMBEDDED_PROV_NET_KEY_IDX           0
#define EMBEDDED_PROV_GROUP_ADDR            0xC000
#define EMBEDDED_PROV_PUB_PERIOD            0
#define EMBEDDED_PROV_PUB_REXMIT_INTERVAL   0
#define EMBEDDED_PROV_PUB_REXMIT_COUNT      1
#define EMBEDDED_PROV_PUB_TTL               7
#define EMBEDDED_PROV_PUB_CREDENTIALS       0
#define EMBEDDED_PROV_DEF_TRANSITION_TIME   200
#define EMBEDDED_PROV_DISTR_NODE_TIMEOUT    300     // Client Timeout=[12000*(Timeout Base+1)+100*Update TTL]  milliseconds

#define EMBEDDED_PROV_NODE_ADDR_FIRST   WICED_NVRAM_VSID_START
#define EMBEDDED_PROV_NODE_ADDR_LAST    (WICED_NVRAM_VSID_START + EMBEDDED_PROV_MAX_NODES)

#define EMBEDDED_PROV_MIN_RSSI              -85

// Scanner will allocate up to 5 unprovisioned nodes, and will restart scan after nodes are provisioned and 1 left in the queue
#define EMBEDDED_PROV_UNPROV_HI_THRESH      5       // embedded provisioner will provision up to this number of nodes simulteneously
#define EMBEDDED_PROV_UNPROV_LO_THRESH      2

/*
 * If EMBEDDED_PROV_RSSI_THRESHOLD is defined, the embedded provisioner will try to
 * add to the network every device which comes too close to any of the mesh nodes which
 * supports remote provisioning service
 */
// #define EMBEDDED_PROV_RSSI_THRESHOLD        -50

/*
 * If EMBEDDED_PROV_RSSI_THRESHOLD is not defined but EMBEDDED_PROV_UUID_MAGIC_NUBMER is
 * defined, the embedded provisioner will try to add to the network every device with UUID which
 * has last 8 bytes matching the magic number
 */
// defined in the makefile
// #define EMBEDDED_PROV_UUID_MAGIC_NUBMER     0xb25d9a2fc46b4b2c

typedef struct
{
    uint8_t  uuid[16];
    uint16_t addr;
    uint8_t  num_elements;
    uint8_t  is_relay;          // the node is currently set as a relay
    uint8_t  dev_key[16];
    int8_t   rssi_table[EMBEDDED_PROV_MAX_NODES];
} mesh_node_t;

typedef struct mesh_unprov_node__t
{
    struct mesh_unprov_node__t* p_next;
    uint16_t rpr_addr;
    uint8_t  uuid[16];
    int8_t   rssi_table[EMBEDDED_PROV_MAX_NODES];
    wiced_timer_t timer;
} mesh_unprov_node_t;

typedef struct
{
    uint16_t addr;
#define RPR_STATE_IDLE              0
#define RPR_STATE_STARTING_SCAN     1
#define RPR_STATE_SCANNING          2
#define RPR_STATE_RESTART_REQUIRED  3
#define RPR_STATE_FAILED            4
    uint8_t rpr_scan_state;
#define RPR_STATE_PROVISIONING      1
#define RPR_STATE_DISCONNECTING     2
    uint8_t rpr_link_state;
    uint8_t num_elements;
    uint16_t new_node_addr;
    wiced_timer_t scan_status_timer;
} mesh_rpr_node_t;

typedef struct
{
#define EMBEDDED_PROVISIONER_STATE_IDLE             0
#define EMBEDDED_PROVISIONER_STATE_ACTIVE           1
#define EMBEDDED_PROVISIONER_STATE_RESET            2
    uint8_t state;
    uint16_t first_free_node_addr;
    mesh_rpr_node_t rpr_node[EMBEDDED_PROV_MAX_NODES];
    mesh_unprov_node_t* p_unprov_node_first;
    wiced_timer_t scan_restart_timer;
    wiced_bt_mesh_event_t* p_out_event;        // Outstanding VS messages
    uint8_t dev_key[16];                        // current dev_key
    wiced_timer_t reset_timer;
#if defined(RSSI_TEST)
    struct rssi_test_
    {
        uint8_t  state;
        uint16_t interval;
        uint16_t count;
        uint16_t dst;
        uint16_t cur_tx_start_node;
        uint16_t cur_tx_start_node_addr;
        uint16_t cur_rx_values_get_node;
    } rssi_test;
#endif
} mesh_embedded_provisioner_state_t;

extern mesh_embedded_provisioner_state_t *p_provisioner_state;

#pragma pack()

#if defined(RSSI_TEST)
extern void rssi_test_init(void);
extern void rssi_test_start(uint16_t dst, uint16_t count, uint8_t interval, uint16_t start_delay);
extern void mesh_vendor_rssi_test_process_tx_start(wiced_bt_mesh_event_t* p_event, uint8_t* p_data, uint16_t data_len);
extern void mesh_vendor_rssi_test_process_tx_start_status(wiced_bt_mesh_event_t* p_event, uint8_t* p_data, uint16_t data_len);
extern void mesh_vendor_rssi_test_process_data(wiced_bt_mesh_event_t* p_event, uint8_t* p_data, uint16_t data_len);
extern void mesh_vendor_rssi_test_process_rx_values_get(wiced_bt_mesh_event_t* p_event, uint8_t* p_data, uint16_t data_len);
extern void mesh_vendor_rssi_test_process_rx_values_status(wiced_bt_mesh_event_t* p_event, uint8_t* p_data, uint16_t data_len);
extern void rssi_test_send_hci_result(uint16_t src, uint16_t report_addr, uint16_t rx_count, int8_t rx_rssi);
#endif
