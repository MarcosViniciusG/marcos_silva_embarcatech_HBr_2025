#include "wifi_conn.h"
#include "mqtt_comm.h"
#include <stdio.h>
#include <string.h>

int main()
{
    stdio_init_all();
    sleep_ms(3000);
    printf("Comecado\n");
    char ssid[256];
    char password[256];
    get_ssid_and_password(ssid, password);
    connect_to_wifi(ssid, password);
    char client_id[] = "client";
    char broker_ip[] = "192.168.18.7";
    mqtt_setup(client_id, broker_ip, NULL, NULL);
    sleep_ms(3000);
    while(true) {
        mqtt_comm_publish("escola/sala1/temperatura", "26.5", strlen("26.5"));
        sleep_ms(1000);
    }
    return 0;
}