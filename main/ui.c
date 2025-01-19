#include "esp_err.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "i18n.h"
#include "config.h"
#include "shared.h"
#include "slvgl.h"
#include <time.h>

static const char *TAG = "WILLOW/UI";
static int global_offset_x = 0;
static int global_offset_y = 60;

static int left_offset_x = 20;

static int buttom_offset_y = -20;

void time_refresh_task(void *data)
{
    time_t now;
    char strftime_buf[16];
    struct tm timeinfo;
    while (true) {
        time(&now);
        // Set timezone to China Standard Time
       // setenv("TZ", "CST-8", 1);
       // tzset();

        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%H:%M", &timeinfo);
       // ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
        if (lvgl_port_lock(lvgl_lock_timeout)) {
            lv_label_set_text_static(lbl_clk, strftime_buf);
            lvgl_port_unlock();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void init_ui(void)
{
  //  ESP_LOGW(TAG, "begin init_ui!!!");
    if (ld == NULL) {
      //  ESP_LOGE(TAG, "lv_disp_t ld is NULL!!!!");
    } else {
        char *speech_rec_mode = config_get_char("speech_rec_mode", DEFAULT_SPEECH_REC_MODE);

        if (lvgl_port_lock(lvgl_lock_timeout)) {
            lv_obj_t *scr_act = lv_disp_get_scr_act(ld);
            lv_obj_t *lbl_hdr = lv_label_create(scr_act);
            lbl_clk = lv_label_create(scr_act); 
            btn_cancel = lv_btn_create(scr_act);
            btn_player = lv_btn_create(scr_act);
            lbl_btn_player = lv_label_create(btn_player);
            lbl_btn_cancel = lv_label_create(btn_cancel);
            lbl_ln1 = lv_label_create(scr_act);
            lbl_ln2 = lv_label_create(scr_act);
            lbl_ln3 = lv_label_create(scr_act);
            lbl_ln4 = lv_label_create(scr_act);
            lbl_ln5 = lv_label_create(scr_act);
            lv_obj_set_style_text_align(lbl_ln3, LV_TEXT_ALIGN_CENTER, 0);
            lv_label_set_recolor(lbl_ln4, true);
            lv_label_set_recolor(lbl_ln5, true);
            lv_obj_add_event_cb(scr_act, cb_scr, LV_EVENT_ALL, NULL);

#ifdef CONFIG_LV_USE_FS_POSIX
            // Willow style
            static lv_style_t lv_st_willow;
            lv_style_init(&lv_st_willow);

            static lv_style_t lv_st_willow_tansp;
            lv_style_init(&lv_st_willow_tansp);

            lv_style_set_bg_opa(&lv_st_willow_tansp, LV_OPA_TRANSP);   // Set background opacity to transparent
            lv_style_set_outline_width(&lv_st_willow_tansp, 0);
            lv_style_set_shadow_width(&lv_st_willow_tansp, 0);
            //lv_style_set_border_width(&lv_st_willow_tansp, 0);         // No border
       

            // Willow colors
            lv_color_t lv_clr_willow = lv_color_hex(0x583759);
            lv_color_t lv_clr_willow_alt = lv_color_hex(0xfbe870);

            // Attach background color to screen
            lv_obj_set_style_bg_color(scr_act, lv_clr_willow, LV_PART_MAIN);

            // White text
            lv_obj_set_style_text_color(scr_act, lv_color_hex(0xffffff), LV_PART_MAIN);

            // Cancel button to alt with black text
            lv_obj_set_style_bg_color(btn_cancel, lv_clr_willow_alt, LV_PART_MAIN);
            lv_obj_set_style_text_color(btn_cancel, lv_color_hex(0x000000), LV_PART_MAIN);

             // Player button to alt with black text
            lv_obj_set_style_bg_color(btn_player, lv_clr_willow_alt, LV_PART_MAIN);
            lv_obj_set_style_text_color(btn_player, lv_color_hex(0xffffff), LV_PART_MAIN);

            // Willow font
            lv_font_t *lv_font_willow;
            //lv_font_willow = lv_font_load("A/spiffs/user/font/tonnelier.bin");
            lv_font_willow = lv_font_load("A/spiffs/user/font/my_montserrrat16.bin");

            // Attach font to style
            lv_style_set_text_font(&lv_st_willow, lv_font_willow);

            // Set Willow style on objects
            lv_obj_add_style(lbl_clk, &lv_st_willow, 0);
            lv_obj_add_style(lbl_hdr, &lv_st_willow, 0);
            lv_obj_add_style(lbl_ln1, &lv_st_willow, 0);
            lv_obj_add_style(lbl_ln2, &lv_st_willow, 0);
            lv_obj_add_style(lbl_ln3, &lv_st_willow, 0);
            lv_obj_add_style(lbl_ln4, &lv_st_willow, 0);
            lv_obj_add_style(lbl_ln5, &lv_st_willow, 0);
            lv_obj_add_style(lbl_btn_cancel, &lv_st_willow, 0);
            lv_obj_add_style(btn_player, &lv_st_willow_tansp, 0);
            lv_obj_set_style_border_opa(btn_player, 0, LV_STATE_FOCUSED);
            lv_obj_set_style_outline_opa(btn_player, 0, LV_STATE_FOCUSED);
            //lv_obj_set_size(btn_player, 25, 25);
#endif
            lv_label_set_text_static(lbl_clk, "00:00");
            lv_label_set_text_static(lbl_btn_cancel, localize_text("Cancel"));
            //lv_label_set_text(lbl_btn_player, LV_SYMBOL_PLAY);
            lv_label_set_text(lbl_btn_player, LV_SYMBOL_WARNING LV_SYMBOL_SD_CARD);
            lv_label_set_text_static(lbl_hdr, localize_text("Welcome to Willow!"));          
            lv_obj_add_flag(btn_cancel, LV_OBJ_FLAG_HIDDEN);
          //  lv_obj_add_flag(btn_player, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(lbl_ln1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(lbl_ln2, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(lbl_ln3, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(lbl_ln4, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(lbl_ln5, LV_OBJ_FLAG_HIDDEN);       
            lv_obj_align(lbl_clk, LV_ALIGN_TOP_MID, global_offset_x + 30, global_offset_y - 35);
            lv_obj_align(btn_player, LV_ALIGN_TOP_MID, global_offset_x - 30, global_offset_y - 50);
            lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_MID, 0, buttom_offset_y -10);
            lv_obj_align(lbl_btn_cancel, LV_ALIGN_CENTER, 0, 0);
            lv_obj_align(lbl_hdr, LV_ALIGN_TOP_MID, global_offset_x + 0, global_offset_y + 5);
            lv_obj_align(lbl_ln1, LV_ALIGN_TOP_LEFT, left_offset_x + global_offset_x + 10, global_offset_y + 40);
            lv_obj_align(lbl_ln2, LV_ALIGN_TOP_LEFT, left_offset_x + global_offset_x + 10, global_offset_y + 70);
            lv_obj_align(lbl_ln3, LV_ALIGN_TOP_LEFT, left_offset_x + global_offset_x + 10, global_offset_y + 100);
            lv_obj_align(lbl_ln4, LV_ALIGN_TOP_LEFT, left_offset_x + global_offset_x + 10, global_offset_y + 130);
            lv_obj_align(lbl_ln5, LV_ALIGN_TOP_LEFT, left_offset_x + global_offset_x + 10, global_offset_y + 160);
            lv_label_set_long_mode(lbl_ln2, LV_LABEL_LONG_SCROLL);
            lv_obj_set_width(lbl_ln1, 300);
            lv_obj_set_width(lbl_ln2, 300);
            lv_obj_set_width(lbl_ln3, 300);
            lv_obj_set_width(lbl_ln4, 300);
            lv_obj_set_width(lbl_ln5, 300);

            if (strcmp(speech_rec_mode, "Multinet") == 0) {
#if defined(WILLOW_SUPPORT_MULTINET)
                lv_label_set_text_static(lbl_ln3, "Starting up (local)...");
#else
                lv_label_set_text_static(lbl_ln3, "Multinet Not Supported");
#endif
            } else if (strcmp(speech_rec_mode, "WIS") == 0) {
                lv_label_set_text_static(lbl_ln3, localize_text("Starting up (server)..."));
            }
            lv_obj_clear_flag(lbl_ln3, LV_OBJ_FLAG_HIDDEN);

            lvgl_port_unlock();
        }
        xTaskCreate(&time_refresh_task, "time_refresh_task", 2048, NULL, 5, NULL);
        free(speech_rec_mode);
    }
}

void ui_pr_err(char *ln3, char *ln4)
{
  //  ESP_LOGW(TAG, "begin ui_pr_err!!!");
    if (ld == NULL) {
        ESP_LOGE(TAG, "display not initialized, writing error on console");
        if (ln3 != NULL) {
            ESP_LOGE(TAG, "%s", ln3);
        }
        if (ln4 != NULL) {
            ESP_LOGE(TAG, "%s", ln4);
        }
        return;
    }

    if (lvgl_port_lock(lvgl_lock_timeout)) {
        lv_obj_add_flag(lbl_ln1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(lbl_ln2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(lbl_ln5, LV_OBJ_FLAG_HIDDEN);

        if (ln3 == NULL) {
            lv_obj_add_flag(lbl_ln3, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_label_set_text(lbl_ln3, ln3);
            lv_obj_set_style_text_align(lbl_ln3, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_clear_flag(lbl_ln3, LV_OBJ_FLAG_HIDDEN);
        }
        if (ln4 == NULL) {
            lv_obj_add_flag(lbl_ln4, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_label_set_text(lbl_ln4, ln4);
            lv_obj_set_style_text_align(lbl_ln4, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_clear_flag(lbl_ln4, LV_OBJ_FLAG_HIDDEN);
        }

        lvgl_port_unlock();
    }
}

void show_center_text(char *ln3)
{
    if (lvgl_port_lock(lvgl_lock_timeout)) {
        lv_label_set_text_static(lbl_ln3, localize_text(ln3));
        lv_obj_add_flag(lbl_ln1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(lbl_ln2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(lbl_ln4, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(lbl_ln5, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(lbl_ln3, LV_OBJ_FLAG_HIDDEN);
        lvgl_port_unlock();
    }

}

