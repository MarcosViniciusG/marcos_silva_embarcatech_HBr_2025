#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

void get_ssid_and_password(char *ssid, char *password);
void connect_to_wifi(const char *ssid, const char *password);