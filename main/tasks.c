#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
static const char *TAG = "WILLOW/TASKS";
#ifdef CONFIG_WILLOW_DEBUG_RUNTIME_STATS
void task_debug_runtime_stats(void *data)
{
    ESP_LOGW(TAG, "begin task_debug_runtime_stats!!!");
    char buf[2048];
    while (true) {
        ESP_LOGW(TAG, "begin task_debug_runtime_stats while (true)!!!");
        vTaskGetRunTimeStats(buf);
        printf("%s\n", buf);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
#endif