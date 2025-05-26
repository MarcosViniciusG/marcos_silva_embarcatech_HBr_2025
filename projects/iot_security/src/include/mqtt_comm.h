#include "lwip/apps/mqtt.h"     
#include "lwipopts.h"

#define MAX_MQTT_PAYLOAD_SIZE 256
static mqtt_client_t *client;
extern char last_mqtt_string[MAX_MQTT_PAYLOAD_SIZE];

void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
void mqtt_comm_subscribe(const char *topic);
void mqtt_comm_unsubscribe(const char *topic);
bool is_connected();