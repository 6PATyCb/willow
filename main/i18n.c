#include <stdio.h>
#include <string.h>

static const char *TAG = "WILLOW/i18n";

char *localize_text(char *text)
{
#if defined(WILLOW_UI_LANG_RU)
    if (strcmp(text, "Notification Active") == 0) {
        return "Оповещение активировано";
    } else if (strcmp(text, "Say command...") == 0) {
        return "Ожидаю команду...";
    } else if (strcmp(text, "I heard:") == 0) {
        return "Я услышала:";
    } else if (strcmp(text, "I heard command:") == 0) {
        return "Я услышала команду:";
    } else if (strcmp(text, "Thinking...") == 0) {
        return "Думаю...";
    } else if (strcmp(text, "Response:") == 0) {
        return "Ответ:";
    } else if (strcmp(text, "WAS disconnected") == 0) {
        return "WAS отключился";
    } else if (strcmp(text, "WAS Restart") == 0) {
        return "Перезагрузка от WAS";
    } else if (strcmp(text, "Connecting to WAS...") == 0) {
        return "Подключаемся к WAS...";
    } else if (strcmp(text, "Command status:") == 0) {
        return "Статус команды:";
    } else if (strcmp(text, "Success!") == 0) {
        return "Успешно!";
    } else if (strcmp(text, "Error") == 0) {
        return "Ошибка";
    } else if (strcmp(text, "Cancel") == 0) {
        return "Отмена";
    } else if (strcmp(text, "Welcome to Willow!") == 0) {
        return "Добро пожаловать в Willow!";
    } else if (strcmp(text, "Starting up (server)...") == 0) {
        return "Стартуем (server)...";
    } else if (strcmp(text, "Cannot Reach WIS") == 0) {
        return "Недоступен WIS";
    } else if (strcmp(text, "Check Server & Settings") == 0) {
        return "Проверьте сервер и настройки";
    } else if (strcmp(text, "WIS timeout") == 0) {
        return "WIS таймаут";
    } else if (strcmp(text, "Check server performance") == 0) {
        return "Проверьте производительность сервера";
    } else if (strcmp(text, "WIS auth failed") == 0) {
        return "WIS авторизация не удалась";
    } else if (strcmp(text, "Mute Activated") == 0) {
        return "Микрофон заглушен";
    } else if (strcmp(text, "Unmute to continue") == 0) {
        return "Включите обратно для продолжения";
    } else if (strcmp(text, "Connecting to Wi-Fi...") == 0) {
        return "Подключаемся к Wi-Fi...";
    } else if (strcmp(text, "Configuration Updated") == 0) {
        return "Конфигурация обновлена";
    } else if (strcmp(text, "Starting Upgrade") == 0) {
        return "Начинаем обновление";
    } else if (strcmp(text, "Upgrade Failed") == 0) {
        return "Обновление не удалось";
    } else if (strcmp(text, "Upgrade Done") == 0) {
        return "Обновление завершено";
    } else if (strcmp(text, "Connectivity Updated") == 0) {
        return "Соединение обновлено";
    }

#else
// todo another lang
#endif

    return text;
}