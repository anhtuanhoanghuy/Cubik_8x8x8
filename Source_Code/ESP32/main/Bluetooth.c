#include "Bluetooth.h"
#include "Wifi.h"
#include <string.h>
#include "Defines.h"
#include "esp_log.h"
#include "esp_err.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "Utils.h"

static const char *TAG = "BLUETOOTH";
QueueHandle_t ble_rx_queue = NULL;
static ble_state_t ble_state = BLE_STATE_OFF;
static uint16_t conn_handle = BLE_HS_CONN_HANDLE_NONE;

static uint8_t BLE_tx_buffer[256] = {0};
/* =========================================================
 * FORWARD DECLARATIONS
 * ========================================================= */
static void ble_host_task(void *param);
static void ble_on_sync(void);
static void ble_on_reset(int reason);
static int ble_gap_event_cb(struct ble_gap_event *event, void *arg);
static void bluetooth_deinit(void);

/* =========================================================
 * GATT SERVER - CUSTOM SERVICE
 * ========================================================= */

// Custom Service UUID: 12345678-1234-5678-1234-56789abcdef0
static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
                     0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

// Characteristic UUID: 12345678-1234-5678-1234-56789abcdef1
static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(0xf1, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
                     0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

/* GATT Service Definition */
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &gatt_svr_chr_uuid.u,
                .access_cb = gatt_svr_chr_access,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            },
            {0} // Kết thúc
        }
    },
    {0} // Kết thúc
};

/* =========================================================
 * GATT CHARACTERISTIC ACCESS CALLBACK
 * ========================================================= */
static int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        os_mbuf_append(ctxt->om, BLE_tx_buffer, sizeof(BLE_tx_buffer));
        ESP_LOGI(TAG, "BLE send:");
        return 0;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        uint8_t data_received[256]; 
        // ---------------------------------------------------------
        uint16_t total_len = OS_MBUF_PKTLEN(ctxt->om);
        uint8_t buffer[256];

        os_mbuf_copydata(ctxt->om, 0, total_len, buffer);

        printf("Packet: ");
        for (int i = 0; i < total_len; i++) {
            printf("%02X ", buffer[i]);
        }
        printf("\n");
        // ---------------------------------------------------------

        // Copy command_id + len
        os_mbuf_copydata(ctxt->om, 0, 3, data_received);
        if (data_received[0] == 0xAA) {

            uint16_t total_len = OS_MBUF_PKTLEN(ctxt->om);
            // Kiểm tra tối thiểu
            if (total_len < 4) {   
                return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
            } 
            ble_command_t cmd = {0};
            cmd.command_id = data_received[1];
            cmd.len        = data_received[2];

            // Validate len
            if (total_len != (4 + cmd.len)) {
                ESP_LOGI(TAG, "Size of packet is invalid.");   
                return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
            } 
            os_mbuf_copydata(ctxt->om, 3, total_len - 1, &data_received[3]);

            // Validate Checksum
            if (!validateChecksum(data_received, total_len - 1, data_received[total_len - 1])) {         
                return BLE_ATT_ERR_INVALID_PDU;
            } 
            memcpy(cmd.data,&data_received[3],cmd.len);
            // Đẩy sang task xử lý
            xQueueSendFromISR(ble_rx_queue, &cmd, NULL);
            ESP_LOGI(TAG, "BLE receive:%d bytes", total_len);  
            // Return ngay!
            return 0;
        }
        return BLE_ATT_ERR_INVALID_PDU;

    default:  
        return BLE_ATT_ERR_UNLIKELY;
    }
}

/* =========================================================
 * GAP EVENT CALLBACK
 * ========================================================= */
static int ble_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {

    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "Connection event, status=%d", event->connect.status);

        if (event->connect.status == 0) {
            conn_handle = event->connect.conn_handle;
            ble_state = BLE_STATE_CONNECTED;
            ESP_LOGI(TAG, "✅ BLE CONNECTED! Handle=%d", conn_handle);
        } else {
            ESP_LOGW(TAG, "❌ Connection failed, status=%d", event->connect.status);
            conn_handle = BLE_HS_CONN_HANDLE_NONE;
            ble_state = BLE_STATE_READY;
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "🔌 BLE DISCONNECTED, reason=%d", event->disconnect.reason);
        conn_handle = BLE_HS_CONN_HANDLE_NONE;
        ble_state = BLE_STATE_READY;
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "Advertising completed");
        if (ble_state == BLE_STATE_ADVERTISING) {
            ble_state = BLE_STATE_READY;
        }
        break;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(TAG, "Connection parameters updated");
        break;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "MTU update: %d", event->mtu.value);
        break;

    default:
        break;
    }
    return 0;
}

/* =========================================================
 * BLE SYNC & RESET CALLBACKS
 * ========================================================= */
static void ble_on_sync(void)
{
    ESP_LOGI(TAG, "🔵 NimBLE synced, Bluetooth ready!");
    ble_state = BLE_STATE_READY;

    // In địa chỉ MAC của Bluetooth
    uint8_t addr[6] = {0};
    int rc = ble_hs_id_copy_addr(BLE_ADDR_PUBLIC, addr, NULL);
    if (rc == 0) {
        ESP_LOGI(TAG, "📍 BLE MAC: %02X:%02X:%02X:%02X:%02X:%02X",
                 addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    }
}

static void ble_on_reset(int reason)
{
    ESP_LOGE(TAG, "⚠️ BLE stack reset! Reason=%d", reason);
    ble_state = BLE_STATE_OFF;
}

/* =========================================================
 * NIMBLE HOST TASK
 * ========================================================= */
static void ble_host_task(void *param)
{
    ESP_LOGI(TAG, "NimBLE host task started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/* =========================================================
 * PUBLIC API: INIT
 * ========================================================= */
void bluetooth_init(void)
{   
    if (ble_state != BLE_STATE_OFF) {
        ESP_LOGW(TAG, "⚠️ Bluetooth already initialized");
        return;
    }

    ESP_LOGI(TAG, "🚀 Initializing NimBLE...");

    ESP_ERROR_CHECK(nimble_port_init());

    // Cấu hình callbacks
    ble_hs_cfg.sync_cb = ble_on_sync;
    ble_hs_cfg.reset_cb = ble_on_reset;

    // Khởi tạo GAP và GATT services
    ble_svc_gap_init();
    ble_svc_gatt_init();

    // Đăng ký GATT services
    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);

    // Đặt tên thiết bị
    ble_svc_gap_device_name_set("CUBIK_ESP32");

    // Start NimBLE host task
    nimble_port_freertos_init(ble_host_task);
    ble_rx_queue = xQueueCreate(
        8,
        sizeof(ble_command_t)
    );
    configASSERT(ble_rx_queue);

    ESP_LOGI(TAG, "✅ Bluetooth initialized successfully");
    ESP_LOGI(TAG, "✅ Queue for Bluetooth initialized successfully");
}

/* =========================================================
 * PUBLIC API: DEINIT
 * ========================================================= */
static void bluetooth_deinit(void)
{
    if (ble_state == BLE_STATE_OFF) {
        return;
    }

    ESP_LOGI(TAG, "Stopping Bluetooth...");

    if (ble_state == BLE_STATE_CONNECTED) {
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }

    if (ble_state == BLE_STATE_ADVERTISING) {
        ble_gap_adv_stop();
    }

    nimble_port_stop();
    nimble_port_deinit();

    ble_state = BLE_STATE_OFF;
    conn_handle = BLE_HS_CONN_HANDLE_NONE;

    ESP_LOGI(TAG, "✅ Bluetooth stopped");
}

/* =========================================================
 * PUBLIC API: START (CONNECTABLE)
 * ========================================================= */
void bluetooth_start(void)
{
    if (ble_state != BLE_STATE_READY) {
        ESP_LOGW(TAG, "⚠️ Cannot start BLE, state=%d", ble_state);
        return;
    }

    ESP_LOGI(TAG, "🔵 Starting BLE (connectable)...");

    struct ble_gap_adv_params adv_params = {0};
    struct ble_hs_adv_fields fields = {0};
    int rc;

    // Cấu hình advertising data
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    // Đặt tên thiết bị (hardcoded để chắc chắn)
    const char *name = "CUBIK_ESP32";
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    ESP_LOGI(TAG, "🔍 Device name: %s (len=%d)", name, fields.name_len);

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "❌ Error setting adv fields: rc=%d", rc);
        return;
    }

    // Cấu hình scan response data (thêm UUID vào đây)
    struct ble_hs_adv_fields rsp_fields = {0};
    rsp_fields.uuids128 = &gatt_svr_svc_uuid;
    rsp_fields.num_uuids128 = 1;
    rsp_fields.uuids128_is_complete = 1;

    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "⚠️ Error setting scan rsp fields: rc=%d", rc);
    }

    // CHO PHÉP KẾT NỐI
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // ← Connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    adv_params.itvl_min = 0x20;  // 20ms
    adv_params.itvl_max = 0x40;  // 40ms

    rc = ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,
        NULL,
        BLE_HS_FOREVER,
        &adv_params,
        ble_gap_event_cb,
        NULL
    );

    if (rc == 0) {
        ble_state = BLE_STATE_ADVERTISING;
        ESP_LOGI(TAG, "✅ BLE started - Devices can see and CONNECT");
        ESP_LOGI(TAG, "📱 Look for: CUBIK_ESP32");
    } else {
        ESP_LOGE(TAG, "❌ Failed to start BLE, rc=%d", rc);
    }
}

/* =========================================================
 * PUBLIC API: STOP
 * ========================================================= */
void bluetooth_stop(void)
{
    if (ble_state == BLE_STATE_ADVERTISING || ble_state == BLE_STATE_CONNECTED) {
        if (ble_state == BLE_STATE_CONNECTED) {
            ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        } else {
            ble_gap_adv_stop();
        }
        ble_state = BLE_STATE_READY;
        ESP_LOGI(TAG, "🛑 BLE stopped");
    }
}

/* =========================================================
 * PUBLIC API: CHECK CONNECTION STATUS
 * ========================================================= */
bool bluetooth_is_connected(void)
{
    return (ble_state == BLE_STATE_CONNECTED);
}


/* =========================================================
 * COMMAND PROCESS
 * ========================================================= */

 void ble_process_task(void *param)
{
    ble_command_t cmd;
    while (1) {
        if (xQueueReceive(ble_rx_queue, &cmd, portMAX_DELAY)) {
            process_ble_command(&cmd);
        }
    }
}


void process_ble_command(ble_command_t *cmd)
{
    ESP_LOGI("BLE", "CMD: 0x%02X, Len: %d", cmd->command_id, cmd->len);
    
    switch (cmd->command_id) {
    
    case CMD_BLE_WIFI_CONNECT: {
        char ssid[WIFI_SSID_MAX] = {0};
        char password[WIFI_PASS_MAX] = {0};

        uint8_t ssid_len = cmd->data[0];
        uint8_t pass_len = cmd->data[1];
        ESP_LOGI("BLE", "ssid_len: %d",ssid_len);
        ESP_LOGI("BLE", "pass_len %d",pass_len);
        if (ssid_len > WIFI_SSID_MAX || pass_len > WIFI_PASS_MAX) return;

        memcpy(ssid, &cmd->data[2], ssid_len);
        memcpy(password, &cmd->data[2 + ssid_len], pass_len);        
        wifi_connect(ssid, password);
        ESP_LOGI("BLE", "CMD_BLE_WIFI_CONNECT");
        ESP_LOGI("BLE", "Connect to %s : %s",ssid, password);
        break;
    }
    
    case CMD_BLE_WIFI_DISCONNECT: {
        ESP_LOGI("BLE", "CMD_BLE_WIFI_DISCONNECT");
        wifi_disconnect();
        break;
    }

    case CMD_BLE_WIFI_ON_OFF:
        ESP_LOGI("BLE", "CMD_BLE_WIFI_ON_OFF");
        uint8_t state = cmd->data[0];
        switch (state)
        {
            case OFF:
                wifi_off();
                break;
            case ON:
                wifi_on();
                break;
            default:
                break;
        }
        break;

    case CMD_BLE_TEST_MODE:
        break;
    
    default:
        ESP_LOGW("BLE", "⚠️ Unknown CMD: 0x%02X", cmd->command_id);
    }
}
