#include "cmd_player.h"
#include "player.h"
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_event.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "WILLOW/CMD_PLAYER";



static int play_mp3(int argc, char **argv)
{
    ESP_LOGI(TAG, "begin player_play");
    player_play();
    return 0;
}

static int stop_mp3(int argc, char **argv)
{
    ESP_LOGI(TAG, "begin player_stop");
    player_stop();
    return 0;
}



void register_player_commands(void)
{
    const esp_console_cmd_t play_cmd = {
        .command = "pplay",
        .help = "Play mp3 file from sdcard. Can be used as a 'Next Track' command when executed during playback",
        .hint = NULL,
        .func = &play_mp3,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&play_cmd));
    const esp_console_cmd_t stop_cmd = {
        .command = "pstop",
        .help = "Stop playing mp3 file from sdcard",
        .hint = NULL,
        .func = &stop_mp3,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&stop_cmd));
}



