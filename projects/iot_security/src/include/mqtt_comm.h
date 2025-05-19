#include "lwip/apps/mqtt.h"     
#include "lwipopts.h"

static mqtt_client_t *client;

void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);