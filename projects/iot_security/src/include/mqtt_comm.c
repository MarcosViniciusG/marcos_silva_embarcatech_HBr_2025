#include "lwip/apps/mqtt.h" // Biblioteca MQTT do lwIP
#include "mqtt_comm.h"      // Header file com as declarações locais
#include "lwipopts.h"       // Configurações customizadas do lwIP
#include <string.h>

static u32_t current_message_total_len = 0; 
char last_mqtt_string[MAX_MQTT_PAYLOAD_SIZE];
static char received_mqtt_string[MAX_MQTT_PAYLOAD_SIZE];
static u16_t received_offset = 0;
/* Callback de conexão MQTT - chamado quando o status da conexão muda
 * Parâmetros:
 *   - client: instância do cliente MQTT
 *   - arg: argumento opcional (não usado aqui)
 *   - status: resultado da tentativa de conexão */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    if (status == MQTT_CONNECT_ACCEPTED)
    {
        printf("Conectado ao broker MQTT com sucesso!\n");
    }
    else if (status == MQTT_CONNECT_DISCONNECTED)
    {
        printf("MQTT se desconectou\n");
    }
    else
    {
        printf("Falha ao conectar ao broker, código: %d\n", status);
    }
}

void mqtt_incoming_publish_callback(void *arg, const char *topic, u32_t tot_len) {
    received_offset = 0;

    if (tot_len >= MAX_MQTT_PAYLOAD_SIZE) {
        printf("Warning: Incoming payload (%lu) exceeds buffer capacity (%d). Data will be truncated.\n", tot_len, MAX_MQTT_PAYLOAD_SIZE - 1);
    }
}

void mqtt_incoming_data_callback(void *arg, const u8_t *data, u16_t len, u8_t flags) {    
    u16_t remaining_space = MAX_MQTT_PAYLOAD_SIZE - 1 - received_offset; // -1 for null terminator

    if (len > remaining_space) {
        len = remaining_space;
    }

    if (len > 0) {
        memcpy(&received_mqtt_string[received_offset], data, len);
        received_offset += len;
    }

    received_mqtt_string[received_offset] = '\0';

    if (received_offset >= current_message_total_len) {
        printf("Full message received: %s\n", received_mqtt_string);
        strncpy(last_mqtt_string, received_mqtt_string, strlen(received_mqtt_string)+1);
    }
}

/* Função para configurar e iniciar a conexão MQTT
 * Parâmetros:
 *   - client_id: identificador único para este cliente
 *   - broker_ip: endereço IP do broker como string (ex: "192.168.1.1")
 *   - user: nome de usuário para autenticação (pode ser NULL)
 *   - pass: senha para autenticação (pode ser NULL) */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass)
{
    ip_addr_t broker_addr; // Estrutura para armazenar o IP do broker

    // Converte o IP de string para formato numérico
    if (!ip4addr_aton(broker_ip, &broker_addr))
    {
        printf("Erro no IP\n");
        return;
    }

    // Cria uma nova instância do cliente MQTT
    client = mqtt_client_new();
    if (client == NULL)
    {
        printf("Falha ao criar o cliente MQTT\n");
        return;
    }

    // Configura as informações de conexão do cliente
    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id, // ID do cliente
        .client_user = user,    // Usuário (opcional)
        .client_pass = pass     // Senha (opcional)
    };

    mqtt_set_inpub_callback(client, mqtt_incoming_publish_callback, mqtt_incoming_data_callback, NULL);

    // Inicia a conexão com o broker
    // Parâmetros:
    //   - client: instância do cliente
    //   - &broker_addr: endereço do broker
    //   - 1883: porta padrão MQTT
    //   - mqtt_connection_cb: callback de status
    //   - NULL: argumento opcional para o callback
    //   - &ci: informações de conexão
    mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
}

/* Callback de confirmação de publicação
 * Chamado quando o broker confirma recebimento da mensagem (para QoS > 0)
 * Parâmetros:
 *   - arg: argumento opcional
 *   - result: código de resultado da operação */
static void mqtt_pub_request_cb(void *arg, err_t result)
{
    if (result == ERR_OK)
    {
        printf("Publicação MQTT enviada com sucesso!\n");
    }
    else
    {
        printf("Erro ao publicar via MQTT: %d\n", result);
    }
}

/* Função para publicar dados em um tópico MQTT
 * Parâmetros:
 *   - topic: nome do tópico (ex: "sensor/temperatura")
 *   - data: payload da mensagem (bytes)
 *   - len: tamanho do payload */
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len)
{
    // Envia a mensagem MQTT
    err_t status = mqtt_publish(
        client,              // Instância do cliente
        topic,               // Tópico de publicação
        data,                // Dados a serem enviados
        len,                 // Tamanho dos dados
        0,                   // QoS 0 (nenhuma confirmação)
        0,                   // Não reter mensagem
        mqtt_pub_request_cb, // Callback de confirmação
        NULL                 // Argumento para o callback
    );

    if (status != ERR_OK)
    {
        printf("mqtt_publish falhou ao ser enviada: %d\n", status);
    }
}

bool is_connected()
{
    return mqtt_client_is_connected(client);
};

void mqtt_comm_subscribe(const char *topic)
{
    mqtt_subscribe(client, topic, 0, NULL, NULL);
}

void mqtt_comm_unsubscribe(const char *topic)
{
    mqtt_unsubscribe(client, topic, 0, NULL);
}