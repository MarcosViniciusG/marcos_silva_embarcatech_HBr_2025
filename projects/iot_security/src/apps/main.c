#include "wifi_conn.h"
#include "mqtt_comm.h"
#include <stdio.h>
#include <string.h>

int main()
{
    stdio_init_all();
    char ssid[256];
    char password[256];
    get_ssid_and_password(ssid, password);
    connect_to_wifi(ssid, password);

    char client_id[] = "client";
    char broker_ip[] = "172.19.10.168";
    mqtt_setup(client_id, broker_ip, NULL, NULL);
    mqtt_publish(client, "escola/sala1/temperatura", "26.5", strlen("26.5"), 0, 0, NULL, NULL);
    while(true) {
        tight_loop_contents();
    }
    return 0;
}
