#include "wifi_conn.h"
#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

/**
 * Função: connect_to_wifi
 * Objetivo: Inicializar o chip Wi-Fi da Pico W e conectar a uma rede usando SSID e senha fornecidos.
 */
bool connect_to_wifi(const char *ssid, const char *password, char *err_msg) {
    // Habilita o modo estação (STA) para se conectar a um ponto de acesso.
    cyw43_arch_enable_sta_mode();

    // Tenta conectar à rede Wi-Fi com um tempo limite de 30 segundos (30000 ms).
    // Utiliza autenticação WPA2 com criptografia AES.
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        strcpy(err_msg, "Erro ao conectar\n");  // Se falhar, imprime mensagem de erro.
        return false;
    } else {        
        strcpy(err_msg, "Conectado ao Wi-Fi\n"); // Se conectar com sucesso, exibe confirmação.
        return true;
    }
}