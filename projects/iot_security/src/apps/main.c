#include "wifi_conn.h"
#include "mqtt_comm.h"
#include <stdio.h>
#include <string.h>
#include "cryptography.h"
#include "init_hardware.h"
#include "ssd1306.h"
#include "virtual_keyboard.h"
#include "hardware/adc.h"

#define N_INITIAL 3
#define N_CONFIG 5
#define N_AUTH 2

const char CLIENT_ID[] = "bitdog2";

typedef enum
{
    STATE_INITIAL,
    STATE_CONFIG,
    STATE_AUTH,
    STATE_KEY,
    STATE_AUTH_USER,
    STATE_AUTH_PASS,
    STATE_SECURITY,
    STATE_PUBLISHER,
    STATE_SUBSCRIBER
} state_t;

static uint8_t _y_initial = 0, _y_config = 0, _y_auth=0;
static uint32_t _last_action_time = 0; // For debouncing
static bool _wifi_connected = false, _mqtt_connected = false;
static char _broker_ip[MAX_TEXT_LENGTH + 1] = "192.168.18.69";
static char _ssid[MAX_TEXT_LENGTH + 1] = "REDEDOMARCOS";
static char _password[MAX_TEXT_LENGTH + 1] = "45612300";
static bool _auth_mqtt = false;
static char _user_mqtt[MAX_TEXT_LENGTH + 1] = "aluno";
static char _pass_mqtt[MAX_TEXT_LENGTH + 1] = "senha123";
static char _topic[MAX_TEXT_LENGTH + 1] = "escola/sala1/temperatura";
static bool _cryp = false;
static int _key = 42;
static bool _timestamp = true;
static char _msg[MAX_TEXT_LENGTH + 1];
static bool _subscribed = false;

ssd1306_t *display_internal;
const state_t INITIAL_PATHS[] = {STATE_PUBLISHER, STATE_SUBSCRIBER, STATE_CONFIG};
const state_t CONFIG_PATHS[] = {STATE_KEY, STATE_AUTH};
const state_t AUTH_PATHS[] = {STATE_AUTH_USER, STATE_AUTH_PASS};

bool is_all_digits(const char *str) {
  if (str == NULL) {
    return false;
  }
  for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i])) {
      return false;
    }
  }
  return true;
}

static void _handle_scroller(state_t *current_state, uint8_t *y, const int N, const state_t *PATHS)
{
    // Debounce inputs
    if (to_ms_since_boot(get_absolute_time()) - _last_action_time < 200)
    {
        return;
    }

    bool action_taken = false;
    adc_select_input(0); // Select ADC0 (GP26) for VRy
    uint16_t vr_y_raw = adc_read();

    // Handle joystick movement for option select
    if (vr_y_raw > JOYSTICK_THRESHOLD_HIGH)
    { // Up
        if (*y == 0)
            *y = N - 1;
        else
            *y = (*y) - 1;
        action_taken = true;
    }
    else if (vr_y_raw < JOYSTICK_THRESHOLD_LOW)
    { // Down
        *y = (*y + 1) % N;
        action_taken = true;
    }

    bool button_a_pressed = !gpio_get(BUTTON_A_PIN);
    bool button_b_pressed = !gpio_get(BUTTON_B_PIN);
    bool joystick_sw_pressed = !gpio_get(JOYSTICK_SW_PIN);

    if (button_a_pressed || joystick_sw_pressed)
    {
        if(*current_state==STATE_CONFIG) {
            if(*y==0) _timestamp = !_timestamp;
            else if(*y==1) _cryp = !_cryp;
            else if(*y==2) *current_state = PATHS[0];
            else if(*y==3) _auth_mqtt = !_auth_mqtt;
            else if(*y==4) *current_state = PATHS[1];
        }
        else {
            *current_state = PATHS[*y];
        }
        action_taken = true;
    }
    else if (button_b_pressed)
    {
        *current_state = STATE_INITIAL;
        action_taken = true;
    }

    if (action_taken)
    {
        _last_action_time = to_ms_since_boot(get_absolute_time());
    }
}

static bool _handle_wifi_conn()
{
    char err_msg[23];
    if (!_wifi_connected)
    {
        ssd1306_clear(display_internal);
        ssd1306_draw_string(display_internal, 0, 32, 1, "Conectando ao Wi-Fi");
        ssd1306_show(display_internal);

        _wifi_connected = connect_to_wifi(_ssid, _password, err_msg);

        ssd1306_clear(display_internal);
        ssd1306_draw_string(display_internal, 0, 32, 1, err_msg);
        ssd1306_show(display_internal);

        sleep_ms(5000);
        if (!_wifi_connected)
        {
            return false;
        }
    }

    return true;
}

static bool _handle_mqtt_conn()
{
    if (!_mqtt_connected)
    {
        ssd1306_clear(display_internal);
        ssd1306_draw_string(display_internal, 0, 32, 1, "Conectando ao broker");
        ssd1306_show(display_internal);

        if(!_auth_mqtt)
            mqtt_setup(CLIENT_ID, _broker_ip, NULL, NULL);
        else
            mqtt_setup(CLIENT_ID, _broker_ip, _user_mqtt, _pass_mqtt);

        sleep_ms(5000);
        _mqtt_connected = is_connected();
    }

    return true;
}

static void _handle_msg() {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    if(_timestamp) {
        sprintf(buffer, "{\"valor\":%s,\"ts\":%ld}", _msg, time(NULL));
    }
    else {
        strncpy(buffer, _msg, strlen(_msg));
    }
    if(_cryp) {
        xor_encrypt(buffer, buffer, strlen(buffer), _key);
    }

    mqtt_comm_publish(_topic, buffer, strlen(buffer));
}

void draw_initial()
{
    ssd1306_clear(display_internal);
    const char *text[] = {
        "Publisher",
        "Subscriber",
        "Config"
    };

    for (uint8_t y = 0; y < N_INITIAL; y++)
    {
        uint8_t _y_off = 32 + (y * 9);
        ssd1306_draw_string(display_internal, 32, _y_off, 1, text[y]);
        if (y == _y_initial)
            ssd1306_draw_string(display_internal, 5, _y_off, 1, "=>");
    }
}

void draw_config()
{
    ssd1306_clear(display_internal);
    const char *text[] = {
        "Timestamp: ",
        "Cryp: ",
        "Cryp key: ",
        "Auth: ",
        "Auth config"
    };

    for (uint8_t y = 0; y < N_CONFIG; y++)
    {
        uint8_t _y_off = 16 + (y * 9);
        ssd1306_draw_string(display_internal, 16, _y_off, 1, text[y]);
        if (y == _y_config)
            ssd1306_draw_string(display_internal, 5, _y_off, 1, "=>");
        if(y==0) {
            if(_timestamp)
                ssd1306_draw_string(display_internal, 16 + strlen(text[y])*6, _y_off, 1, "ON");
            else
                ssd1306_draw_string(display_internal, 16 + strlen(text[y])*6, _y_off, 1, "OFF");
        }
        else if(y==1) {
            if(_cryp)
                ssd1306_draw_string(display_internal, 16 + strlen(text[y])*6, _y_off, 1, "ON");
            else
                ssd1306_draw_string(display_internal, 16 + strlen(text[y])*6, _y_off, 1, "OFF"); 
        }
        else if(y==2) {
            char str[MAX_TEXT_LENGTH+1];
            snprintf(str, sizeof(str), "%d", _key);
            ssd1306_draw_string(display_internal, 16 + strlen(text[y])*6, _y_off, 1, str);
        }
        else if(y==3) {
            if(_auth_mqtt)
                ssd1306_draw_string(display_internal, 16 + strlen(text[y])*6, _y_off, 1, "ON");
            else
                ssd1306_draw_string(display_internal, 16 + strlen(text[y])*6, _y_off, 1, "OFF");
        }
    }
}

void draw_auth()
{
    ssd1306_clear(display_internal);
    const char *text[] = {
        "Auth user"
        "Auth pass"
    };

    for (uint8_t y = 0; y < N_AUTH; y++)
    {
        uint8_t _y_off = 32 + (y * 9);
        ssd1306_draw_string(display_internal, 32, _y_off, 1, text[y]);
        if (y == _y_auth)
            ssd1306_draw_string(display_internal, 5, _y_off, 1, "=>");
    }
}

state_t handle_state(state_t current_state)
{
    state_t next_state = current_state;

    bool finished = false;

    switch (current_state)
    {
    case STATE_INITIAL:
        _handle_scroller(&next_state, &_y_initial, N_INITIAL, INITIAL_PATHS);
        draw_initial();
        break;
    case STATE_CONFIG:
        _handle_scroller(&next_state, &_y_config, N_CONFIG, CONFIG_PATHS);
        draw_config();
        break;
    case STATE_AUTH:
        _handle_scroller(&next_state, &_y_auth, N_AUTH, AUTH_PATHS);
        draw_auth();
        break;
    case STATE_AUTH_USER:
        finished = virtual_keyboard_handle_input();
        virtual_keyboard_draw(display_internal, false);
        if (finished)
        {
            strncpy(_user_mqtt, virtual_keyboard_get_text(), strlen(virtual_keyboard_get_text()));
            virtual_keyboard_clear_text(); 
            next_state = STATE_AUTH;
        }
        break;
    
    case STATE_AUTH_PASS:
        finished = virtual_keyboard_handle_input();
        virtual_keyboard_draw(display_internal, false);
        if (finished)
        {
            strncpy(_pass_mqtt, virtual_keyboard_get_text(), strlen(virtual_keyboard_get_text()));
            virtual_keyboard_clear_text(); 
            next_state = STATE_AUTH;
        }
        break;
    case STATE_KEY:
        finished = virtual_keyboard_handle_input();
        virtual_keyboard_draw(display_internal, false);
        char str[MAX_TEXT_LENGTH+1];
        if (finished)
        {
            strncpy(str, virtual_keyboard_get_text(), strlen(virtual_keyboard_get_text()));
            if(is_all_digits(str)) {
                _key = atoi(str);
            }
            virtual_keyboard_clear_text(); 
            next_state = STATE_CONFIG;
        }
        break;

    case STATE_PUBLISHER:
        if (!_handle_wifi_conn())
        {
            next_state = STATE_INITIAL;
            break;
        }

        if (!_handle_mqtt_conn())
        {
            next_state = STATE_INITIAL;
            break;
        }

        finished = virtual_keyboard_handle_input();
        virtual_keyboard_draw(display_internal, false);

        memset(_msg, 0, sizeof(_msg));
        if (finished)
        {
            strncpy(_msg, virtual_keyboard_get_text(), strlen(virtual_keyboard_get_text()));
            _handle_msg();
            virtual_keyboard_clear_text(); 
            next_state = STATE_INITIAL;
        }
        break;
    case STATE_SUBSCRIBER:
        if (!_handle_wifi_conn())
        {
            next_state = STATE_INITIAL;
            break;
        }

        if (!_handle_mqtt_conn())
        {
            next_state = STATE_INITIAL;
            break;
        }

        bool button_b_pressed = !gpio_get(BUTTON_B_PIN);
        if(button_b_pressed) {
            mqtt_comm_unsubscribe(_topic);
            next_state = STATE_INITIAL;
            _subscribed = false;
            break;
        }
        if(!_subscribed) {
            mqtt_comm_subscribe(_topic);
            _subscribed = true;
        }
        ssd1306_clear(display_internal);
        if(_cryp) xor_encrypt(last_mqtt_string, last_mqtt_string, sizeof(last_mqtt_string), _key);
        ssd1306_draw_string(display_internal, 5, 5, 1, last_mqtt_string);

        break;
    }

    return next_state;
}

int main()
{
    stdio_init_all();

    ssd1306_t disp;
    bool success = init_hardware(&disp);
    if (!success)
    {
        printf("Failed to init display. Program finished\n");
        return 0;
    }

    if (cyw43_arch_init()) {
        printf("Failed to init Wi-Fi\n");
        return 0;
    }

    display_internal = &disp;

    state_t current_state = STATE_INITIAL;
    while (true)
    {
        current_state = handle_state(current_state);
        ssd1306_show(&disp);
        sleep_ms(50);
    }
    return 0;
}