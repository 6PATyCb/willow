#include "player.h"
#include "argtable3/argtable3.h"
#include "audio.h"
#include "board.h"
#include "config.h"
#include "display.h"
#include "esp_console.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "i18n.h"
#include "lvgl.h"
#include "sdcard_list.h"
#include "sdcard_scan.h"
#include "slvgl.h"
#include "system.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "WILLOW/PLAYER";
static volatile struct play_data *p_play_active;

struct play_data {
    char *audio_url;
    bool backlight;
    bool backlight_max;
    bool cancel;
    char *text;
    int volume;
};

playlist_operator_handle_t sdcard_list_handle = NULL;

// static void play_task(void *data);

// static void sdcard_url_save_cb(void *user_data, char *url);

char *extractFileName(char *filePath)
{
    // Находим позицию последнего символа слеша в строке
    int lastSlashPos = strrchr(filePath, '/') - filePath;

    // Если строка не содержит слешей, то берем всю строку как имя файла
    if (lastSlashPos == -1) {
        return filePath;
    }

    // Возвращаем подстроку от конца строки до позиции последнего слеша
    return &filePath[lastSlashPos + 1];
}

static void sdcard_url_save_cb(void *user_data, char *url)
{
    // ESP_LOGE(TAG, "begin sdcard_url_save_cb!!!");
    playlist_operator_handle_t sdcard_handle = (playlist_operator_handle_t)user_data;
    esp_err_t ret = sdcard_list_save(sdcard_handle, url);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to save sdcard url to sdcard playlist");
    }
}

void p_cb_btn_cancel_notify(lv_event_t *ev)
{
    //  ESP_LOGW(TAG, "begin p_cb_btn_cancel_notify!!!");
    ESP_LOGD(TAG, "btn_cancel pressed");
    volatile struct play_data *pd_local = p_play_active;
    pd_local->cancel = true;
    esp_audio_stop(hdl_ea, TERMINATION_TYPE_NOW);
}

void cb_btn_player_notify(lv_event_t *ev)
{
    //  ESP_LOGW(TAG, "begin cb_btn_cancel_notify!!!");
    ESP_LOGI(TAG, "btn_player pressed");
    player_play();
}

static void play_task(void *data)
{
   // ESP_LOGW(TAG, "begin play_task %p!!!", data);
    esp_err_t ret;
    struct play_data *pd = (struct play_data *)data;

    if (!pd) {
        ESP_LOGW(TAG, "play_task called with empty data");
        goto out;
    }

    p_play_active = pd;

    if (pd->audio_url != NULL) {
        volume_set(pd->volume);
        gpio_set_level(get_pa_enable_gpio(), 1);
        char *audio_url = NULL;
        while (1) {
            if (pd->cancel) {
                break;
            }
            if (lvgl_port_lock(lvgl_lock_timeout)) {
                lv_label_set_text(lbl_ln3, pd->text);
                lv_obj_remove_event_cb(btn_cancel, cb_btn_cancel);
                lv_obj_add_flag(lbl_ln1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(lbl_ln2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(lbl_ln4, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(lbl_ln5, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_event_cb(btn_cancel, p_cb_btn_cancel_notify, LV_EVENT_PRESSED, NULL);
                lv_obj_clear_flag(lbl_ln3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(btn_cancel, LV_OBJ_FLAG_HIDDEN);
                lv_label_set_long_mode(lbl_ln3, LV_LABEL_LONG_SCROLL);
                lvgl_port_unlock();
            }

            reset_timer(hdl_display_timer, config_get_int("display_timeout", DEFAULT_DISPLAY_TIMEOUT), true);
            display_set_backlight(pd->backlight, pd->backlight_max);

         //   ESP_LOGE(TAG, "play_task before esp_audio_sync_play!!!");
            ret = esp_audio_sync_play(hdl_ea, pd->audio_url, 0);
         //   ESP_LOGE(TAG, "play_task after esp_audio_sync_play!!!");
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Fail to esp_audio_sync_play: %s", esp_err_to_name(ret));
                break;
            }
          //  ESP_LOGE(TAG, "play_task before pd->cancel %d!!!", pd->cancel);
            if (pd->cancel) {
                break;
            }

         //   ESP_LOGE(TAG, "play_task before sdcard_list_next!!!");
            sdcard_list_next(sdcard_list_handle, 1, &audio_url);
        //    ESP_LOGE(TAG, "play_task after sdcard_list_next!!!");
            free(pd->text);
            free(pd->audio_url);
            pd->audio_url = strndup(audio_url, strlen(audio_url));
            char *text = extractFileName(audio_url);
            pd->text = strndup(text, strlen(text));
            //  vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
      //  ESP_LOGE(TAG, "play_task before free!!!");
        free(pd->text);
        free(pd->audio_url);
        gpio_set_level(get_pa_enable_gpio(), 0);
        volume_set(-1);
    }

    if (lvgl_port_lock(lvgl_lock_timeout)) {
        lv_obj_add_flag(btn_cancel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_event_cb(btn_cancel, p_cb_btn_cancel_notify);
        lvgl_port_unlock();
    }

    reset_timer(hdl_display_timer, config_get_int("display_timeout", DEFAULT_DISPLAY_TIMEOUT), false);

out:
  //  ESP_LOGE(TAG, "play_task out %p !!!", pd);
    free(pd);
    p_play_active = NULL;
    vTaskDelete(NULL);
}

void player_init(void)
{
    if (!sd_card_inited) {
        ESP_LOGE(TAG, "sd card was not inited");
        return;
    }
    
    if (lvgl_port_lock(lvgl_lock_timeout)) {
        lv_label_set_text(lbl_btn_player, LV_SYMBOL_WARNING LV_SYMBOL_AUDIO);
        lvgl_port_unlock();
    }
    esp_err_t err;
    if (sdcard_list_handle == NULL) {
        err = sdcard_list_create(&sdcard_list_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail to sdcard_list_create: %s", esp_err_to_name(err));
            return;
        }

        ESP_ERROR_CHECK(
            sdcard_scan(sdcard_url_save_cb, "/sdcard", 1, (const char *[]){"mp3", "wav"}, 2, sdcard_list_handle));
        err = sdcard_list_show(sdcard_list_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail to sdcard_list_show: %s", esp_err_to_name(err));
            return;
        }
        char *audio_url = NULL;
        err = sdcard_list_current(sdcard_list_handle, &audio_url);
        //  ESP_LOGE(TAG, "after sdcard_list_current!!!");
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail to sdcard_list_current: %s", esp_err_to_name(err));
            return;
        }
    }else{
        ESP_LOGE(TAG, "player_init allready inited");
        return;
    }
    if (lvgl_port_lock(lvgl_lock_timeout)) {
        lv_obj_add_event_cb(btn_player, cb_btn_player_notify, LV_EVENT_PRESSED, NULL);
        // lv_obj_clear_flag(btn_player, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(lbl_btn_player, LV_SYMBOL_AUDIO);
        lvgl_port_unlock();
    }
}

void player_play(void)
{
    if (!sd_card_inited) {
        ESP_LOGE(TAG, "sd card was not inited");
        return;
    }
    // if (p_play_active != NULL) {
    //     ESP_LOGW(TAG, "play_task called with allready played");
    //     return 1;
    // }
    esp_err_t err;

    char *audio_url = NULL;

    if (p_play_active == NULL) {
        //  ESP_LOGE(TAG, "before sdcard_list_current!!!");
        err = sdcard_list_current(sdcard_list_handle, &audio_url);
        //  ESP_LOGE(TAG, "after sdcard_list_current!!!");
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail to sdcard_list_current: %s", esp_err_to_name(err));
            return;
        }
    } else {
        // ESP_LOGE(TAG, "before esp_audio_stop!!!");
        p_play_active->cancel = true;
        err = esp_audio_stop(hdl_ea, TERMINATION_TYPE_NOW);
        // ESP_LOGE(TAG, "after esp_audio_stop!!!");
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail to esp_audio_stop: %s", esp_err_to_name(err));
        }

        // ESP_LOGE(TAG, "before sdcard_list_next!!!");
        sdcard_list_next(sdcard_list_handle, 1, &audio_url);
        //  ESP_LOGE(TAG, "after sdcard_list_next!!!");
        while (1) {
            if (p_play_active == NULL) {
                break;
            }
            ESP_LOGD(TAG, "waiting stop old notify task");
            vTaskDelay(250 / portTICK_PERIOD_MS);
        }
    }
    ESP_LOGI(TAG, "Play URL: %s", audio_url);
   // ESP_LOGE(TAG, "Play address=%p URL: %s!!!", (void *)audio_url, audio_url);
    // sdcard_list_destroy(sdcard_list_handle);
    struct play_data *pd = (struct play_data *)calloc(1, sizeof(struct play_data));
    // const char *audio_url = "spiffs://spiffs/user/audio/success.wav";
    //  const char *audio_url = "file://sdcard/music/bayonetta.mp3";
    // const char *audio_url = "file://sdcard/gettysburg10.wav";
    //  const char *audio_url = "https://www2.cs.uic.edu/~i101/SoundFiles/gettysburg10.wav";
    // const char *audio_url = "https://download.samplelib.com/mp3/sample-15s.mp3";
    // const char *text = "Player active!";
    char *text = extractFileName(audio_url);
   // ESP_LOGE(TAG, "extractFileName address=%p URL: %s!!!", (void *)text, text);
    pd->audio_url = strndup(audio_url, strlen(audio_url));
    //ESP_LOGE(TAG, "pd->audio_url address=%p URL: %s!!!", (void *)pd->audio_url, pd->audio_url);
    pd->backlight = true;
    pd->backlight_max = true;
    pd->text = strndup(text, strlen(text));
    pd->volume = 90;
   // ESP_LOGE(TAG, "before xTaskCreatePinnedToCore play_task!!!");
    xTaskCreatePinnedToCore(&play_task, "play_task", 4096, pd, 4, NULL, 0);
    // goto cleanup;
}

void player_stop(void)
{
    if (!sd_card_inited) {
        ESP_LOGE(TAG, "sd card was not inited");
        return;
    }
    volatile struct play_data *pd_local = p_play_active;
    // if (p_play_active == NULL) {
    //     ESP_LOGW(TAG, "play_task is not started");
    //     return 1;
    // }
    // ESP_LOGE(TAG, "before esp_audio_stopp!!!");
    if (pd_local != NULL) {
        pd_local->cancel = true;
    }
    esp_err_t err = esp_audio_stop(hdl_ea, TERMINATION_TYPE_NOW);
    //  ESP_LOGE(TAG, "after esp_audio_stopp!!!");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail to esp_audio_stopp: %s", esp_err_to_name(err));
    }
}

bool is_player_active(void)
{
    return p_play_active != NULL;
}