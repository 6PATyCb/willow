#include "board.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "audio_element.h"
#include "i2s_stream.h"
#include "lvgl.h"
#include "sdkconfig.h"
#include "console.h"
#include "shared.h"
#include "slvgl.h"
#include "system.h"

static const char *TAG = "WILLOW/SYSTEM";
static const char *willow_hw_t[WILLOW_HW_MAX] = {
    [WILLOW_HW_UNSUPPORTED] = "HW-UNSUPPORTED",
    [WILLOW_HW_ESP32_S3_BOX] = "ESP32-S3-BOX",
    [WILLOW_HW_ESP32_S3_BOX_LITE] = "ESP32-S3-BOX-Lite",
    [WILLOW_HW_ESP32_S3_BOX_3] = "ESP32-S3-BOX-3",
    [WILLOW_HW_GRC_AI_MODULE_V0_2] = "GRC-AI-MODULE-v0.2",
    [WILLOW_HW_JC3636] = "ESP32-S3-JC3636",
};

i2c_bus_handle_t hdl_i2c_bus;
volatile bool restarting = false;

const char *str_hw_type(int id)
{
    if (id < 0 || id >= WILLOW_HW_MAX || !willow_hw_t[id]) {
        return "Invalid hardware type.";
    }
    return willow_hw_t[id];
}

static void set_hw_type(void)
{
#if defined(CONFIG_ESP32_S3_BOX_BOARD)
    hw_type = WILLOW_HW_ESP32_S3_BOX;
#elif defined(CONFIG_ESP32_S3_BOX_LITE_BOARD)
    hw_type = WILLOW_HW_ESP32_S3_BOX_LITE;
#elif defined(CONFIG_ESP32_S3_BOX_3_BOARD)
    hw_type = WILLOW_HW_ESP32_S3_BOX_3;
#elif defined(CONFIG_ESP32_S3_GRC_AI_MODULE_V0_2_BOARD)
    hw_type = WILLOW_HW_GRC_AI_MODULE_V0_2;
#elif defined(CONFIG_ESP32_S3_JC3636_BOARD)
    hw_type = WILLOW_HW_JC3636;
#else
    hw_type = WILLOW_HW_UNSUPPORTED;
#endif
    ESP_LOGD(TAG, "hardware type %d (%s)", hw_type, str_hw_type(hw_type));
}

static esp_err_t init_ev_loop()
{
  //  ESP_LOGW(TAG, "begin init_ev_loop!!!");
    esp_err_t ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize default event loop: %s", esp_err_to_name(ret));
    }
    return ret;
}

static void init_i2c(void)
{
  //  ESP_LOGW(TAG, "begin init_i2c!!!");
    int ret = ESP_OK;
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    ret = get_i2c_pins(I2C_NUM_0, &i2c_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to get I2C pins");
    }
    hdl_i2c_bus = i2c_bus_create(I2C_NUM_0, &i2c_cfg);
}

void init_system(void)
{
   // ESP_LOGW(TAG, "begin init_system!!!");
    
    set_hw_type();
    init_i2c();
    ESP_ERROR_CHECK(init_ev_loop());
    ESP_ERROR_CHECK(init_console());
}

void restart_delayed(void)
{
  //  ESP_LOGW(TAG, "begin restart_delayed!!!");
    uint32_t delay = esp_random() % 9;
    if (delay < 3) {
        delay = 3;
    } else if (delay > 6) {
        delay = 6;
    }

    ESP_LOGI(TAG, "restarting after %" PRIu32 " seconds", delay);

    if (lvgl_port_lock(lvgl_lock_timeout)) {
        #if defined(WILLOW_UI_LANG_RU)
            lv_label_set_text_fmt(lbl_ln4, "Перезапустимся через %" PRIu32 " сек", delay);
        #else
            lv_label_set_text_fmt(lbl_ln4, "Restarting in %" PRIu32 " seconds", delay);
        #endif
        lv_obj_clear_flag(lbl_ln4, LV_OBJ_FLAG_HIDDEN);
        lvgl_port_unlock();
    }

    delay *= 1000;
    vTaskDelay(delay / portTICK_PERIOD_MS);
    esp_restart();
}
