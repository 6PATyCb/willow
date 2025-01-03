#include "esp_peripherals.h"
#include "i2c_bus.h"

enum willow_hw_t {
    WILLOW_HW_UNSUPPORTED = 0,
    WILLOW_HW_ESP32_S3_BOX,
    WILLOW_HW_ESP32_S3_BOX_LITE,
    WILLOW_HW_ESP32_S3_BOX_3,
    WILLOW_HW_GRC_AI_MODULE_V0_2,
    WILLOW_HW_JC3636,
    WILLOW_HW_MAX, // keep this last
};

enum willow_state {
    STATE_INIT = 0,
    STATE_NVS_OK,
    STATE_CONFIG_OK,
    STATE_READY,
};

extern enum willow_hw_t hw_type;
extern enum willow_state state;
extern esp_periph_set_handle_t hdl_pset;
extern i2c_bus_handle_t hdl_i2c_bus;

extern volatile bool restarting;

const char *str_hw_type(int id);
void init_system(void);
void restart_delayed(void);
