#include "wifi_conn.h"
#include "mqtt_comm.h"
#include <stdio.h>
#include <string.h>
#include "cryptography.h"

const char CLIENT_ID[] = "bitdog1";

int main()
{
    stdio_init_all();
    char ssid[256];
    char password[256];
    get_ssid_and_password(ssid, password);
    connect_to_wifi(ssid, password);
    char broker_ip[] = "192.168.18.7";

    bool authentication = false;
    if(authentication) {    
        mqtt_setup(CLIENT_ID, broker_ip, "aluno", "senha123");
    }
    else mqtt_setup(CLIENT_ID, broker_ip, NULL, NULL);

    char *msg = "26.5"; 

    while(true) {
         sleep_ms(5000);

        // Timestamp
        char buffer[256];
        sprintf(buffer, "{\"valor\":26.5,\"ts\":%lu}", time(NULL));

        // Cryptography
        uint8_t criptografada[256];
        xor_encrypt((uint8_t *)buffer, criptografada, strlen(buffer), 42);

        // Without cryptography
        mqtt_comm_publish("escola/sala1/temperatura", buffer, strlen(buffer));

        // With cryptography
        mqtt_comm_publish("escola/sala1/temperatura", criptografada, strlen(criptografada));
    }
    return 0;
}