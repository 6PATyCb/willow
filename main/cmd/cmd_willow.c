#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "cmd_willow.h"
#include "nvs_flash.h"


static const char *TAG = "WILLOW/CMD_WILLOW";
char was_url[2048];
char ha_device_id[64];
/** Arguments used by 'write_wifi' function */
static struct {
    struct arg_str *ssid;
    struct arg_str *password;
    struct arg_end *end;
} write_wifi_args;

static struct {
    struct arg_str *url;
    struct arg_end *end;
} write_was_url_args;

static struct {
    struct arg_str *device_id;
    struct arg_end *end;
} write_ha_device_id_args;

static int read_wifi(int argc, char **argv)
{
    ESP_LOGI(TAG, "read_wifi!!!");
    esp_err_t err;
    nvs_handle_t hdl_nvs;
    err = nvs_open("WIFI", NVS_READONLY, &hdl_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to open NVS namespace WIFI: %s", esp_err_to_name(err));
        return 1;
    }

    char psk[64];
    size_t sz = sizeof(psk);
    err = nvs_get_str(hdl_nvs, "PSK", psk, &sz);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to get PSK from NVS namespace WIFI: %s", esp_err_to_name(err));
        return 1;
    }

    char ssid[33];
    sz = sizeof(ssid);
    err = nvs_get_str(hdl_nvs, "SSID", ssid, &sz);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to get SSID from NVS namespace WIFI: %s", esp_err_to_name(err));
        return 1;
    }
    
    ESP_LOGI(TAG, "Wifi SSID: '%s'", ssid);
    ESP_LOGI(TAG, "Wifi PASS: '%s'", psk);

    nvs_close(hdl_nvs);
    return 0;
}

static int write_wifi(int argc, char **argv)
{
    
    ESP_LOGI(TAG, "write_wifi!!!");

    int nerrors = arg_parse(argc, argv, (void **) &write_wifi_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, write_wifi_args.end, argv[0]);
        return 1;
    }
    esp_err_t err;
    nvs_handle_t hdl_nvs;
    err = nvs_open("WIFI", NVS_READWRITE, &hdl_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to open NVS namespace WIFI: %s", esp_err_to_name(err));
        return 1;
    }

    err = nvs_set_str(hdl_nvs, "SSID", write_wifi_args.ssid->sval[0]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed set SSID to NVS namespace WIFI: %s", esp_err_to_name(err));
        nvs_close(hdl_nvs);
        return 1;
    }

    err = nvs_set_str(hdl_nvs, "PSK", write_wifi_args.password->sval[0]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed set PSK to NVS namespace WIFI: %s", esp_err_to_name(err));
        nvs_close(hdl_nvs);
        return 1;
    }

    ESP_LOGI(TAG, "New Wifi SSID: '%s'", write_wifi_args.ssid->sval[0]);
    ESP_LOGI(TAG, "New Wifi PASS: '%s'", write_wifi_args.password->sval[0]);
    ESP_LOGW(TAG, "Restart your device to connect to Wi-Fi with the new credentials");

    nvs_close(hdl_nvs);
    return 0;
}

static int read_was_url(int argc, char **argv)
{
    ESP_LOGI(TAG, "read_was_url!!!");

    esp_err_t err;
    nvs_handle_t hdl_nvs;

    err = nvs_open("WAS", NVS_READONLY, &hdl_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to open NVS namespace WAS: %s", esp_err_to_name(err));
        return 1;
    }
    size_t sz = sizeof(was_url);
    err = nvs_get_str(hdl_nvs, "URL", was_url, &sz);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to get WAS URL from NVS namespace WAS: %s", esp_err_to_name(err));
        return 1;
    }
   
    ESP_LOGI(TAG, "WAS URL: '%s'", was_url);

    nvs_close(hdl_nvs);
    return 0;
}

static int write_was_url(int argc, char **argv)
{
    
    ESP_LOGI(TAG, "write_was_url!!!");

    int nerrors = arg_parse(argc, argv, (void **) &write_was_url_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, write_was_url_args.end, argv[0]);
        return 1;
    }
    esp_err_t err;
    nvs_handle_t hdl_nvs;
    err = nvs_open("WAS", NVS_READWRITE, &hdl_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to open NVS namespace WAS: %s", esp_err_to_name(err));
        return 1;
    }

    err = nvs_set_str(hdl_nvs, "URL", write_was_url_args.url->sval[0]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed set WAS URL to NVS namespace WAS: %s", esp_err_to_name(err));
        nvs_close(hdl_nvs);
        return 1;
    }

    ESP_LOGI(TAG, "New WAS URL: '%s'", write_was_url_args.url->sval[0]);
    ESP_LOGW(TAG, "Restart your device to connect with new WAS URL");

    nvs_close(hdl_nvs);
    return 0;
}

static int read_ha_device_id(int argc, char **argv)
{
    ESP_LOGE(TAG, "read_ha_device_id!!!");

    esp_err_t err;
    nvs_handle_t hdl_nvs;

    err = nvs_open("HA", NVS_READONLY, &hdl_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to open NVS namespace WAS: %s", esp_err_to_name(err));
        return 1;
    }
    size_t sz = sizeof(ha_device_id);
    err = nvs_get_str(hdl_nvs, "DEVICE_ID", ha_device_id, &sz);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to get HA DEVICE_ID from NVS namespace HA: %s", esp_err_to_name(err));
        return 1;
    }
   
    ESP_LOGE(TAG, "HA DEVICE_ID: '%s'", ha_device_id);

    nvs_close(hdl_nvs);
    return 0;
}

static int write_ha_device_id(int argc, char **argv)
{
    
    ESP_LOGE(TAG, "write_ha_device_id!!!");

    int nerrors = arg_parse(argc, argv, (void **) &write_ha_device_id_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, write_ha_device_id_args.end, argv[0]);
        return 1;
    }
    esp_err_t err;
    nvs_handle_t hdl_nvs;
    err = nvs_open("HA", NVS_READWRITE, &hdl_nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to open NVS namespace HA: %s", esp_err_to_name(err));
        return 1;
    }

    err = nvs_set_str(hdl_nvs, "DEVICE_ID", write_ha_device_id_args.device_id->sval[0]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed set HA DEVICE_ID to NVS namespace HA: %s", esp_err_to_name(err));
        nvs_close(hdl_nvs);
        return 1;
    }

    ESP_LOGI(TAG, "New HA DEVICE_ID: '%s'", write_ha_device_id_args.device_id->sval[0]);
    ESP_LOGE(TAG, "Restart your device to use new HA DEVICE_ID");

    nvs_close(hdl_nvs);
    return 0;
}

void register__willow_commands(void)
{
    write_wifi_args.ssid = arg_str1(NULL, NULL, "<ssid>", "SSID of AP");
    write_wifi_args.password = arg_str1(NULL, NULL, "<pass>", "PSK of AP");
    write_wifi_args.end = arg_end(1);

    const esp_console_cmd_t write_wifi_cmd = {
        .command = "write_wifi",
        .help = "Save new WiFi SSID and PASS to nvs",
        .hint = NULL,
        .func = &write_wifi,
        .argtable = &write_wifi_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&write_wifi_cmd) );

    const esp_console_cmd_t read_wifi_cmd = {
        .command = "read_wifi",
        .help = "Print current WiFi SSID and PASS from nvs",
        .hint = NULL,
        .func = &read_wifi,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&read_wifi_cmd) );

    const esp_console_cmd_t read_was_url_cmd = {
        .command = "read_was_url",
        .help = "Print current WAS URL from nvs",
        .hint = NULL,
        .func = &read_was_url,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&read_was_url_cmd) );


    write_was_url_args.url = arg_str1(NULL, NULL, "<url>", "WAS URL");
    write_was_url_args.end = arg_end(0);
    const esp_console_cmd_t write_was_url_cmd = {
        .command = "write_was_url",
        .help = "Save new WAS URL to nvs (example: 'ws://192.168.133.252:8502/ws')",
        .hint = NULL,
        .func = &write_was_url,
        .argtable = &write_was_url_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&write_was_url_cmd) );

    const esp_console_cmd_t read_ha_device_id_cmd = {
        .command = "read_ha_device_id",
        .help = "Print current HA DEVICE_ID from nvs",
        .hint = NULL,
        .func = &read_ha_device_id,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&read_ha_device_id_cmd) );


    write_ha_device_id_args.device_id = arg_str1(NULL, NULL, "<device_id>", "HA device_id");
    write_ha_device_id_args.end = arg_end(0);
    const esp_console_cmd_t write_ha_device_id_cmd = {
        .command = "write_ha_device_id",
        .help = "Save new HA DEVICE_ID to nvs (example: '4d6de9efe261a78924e6e2e4b7cb8302')",
        .hint = NULL,
        .func = &write_ha_device_id,
        .argtable = &write_ha_device_id_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&write_ha_device_id_cmd) );
}
