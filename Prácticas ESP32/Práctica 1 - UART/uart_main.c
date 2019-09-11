/*
    Práctica 2 ESP32
        -> Comunicacion UART con PC
    
    Fundamentos de Sistemas Embebidos

    Alumnos(as):
        López Santibáñez Jiménez Luis Gerardo
        Ortiz Figueroa María Fernanda
        Robles Uribe Karen Abril
*/

#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "string.h"

#define GPIO_OUTPUT_LED 18
#define GPIO_INPUT_BOTON 34

static const char* TAG = ">> UART ";

static void uart_select_task(void *arg){
    gpio_pad_select_gpio(GPIO_OUTPUT_LED);
    gpio_pad_select_gpio(GPIO_INPUT_BOTON);

    gpio_set_direction(GPIO_OUTPUT_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_INPUT_BOTON, GPIO_MODE_INPUT);
    
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 2*1024, 0, 0, NULL, 0);

    while (1) {
        int fd;
        int boton;
        
        // Instalando el controlador
        if ((fd = open("/dev/uart/0", O_RDWR)) == -1) {
            ESP_LOGE(TAG, "No se puede abrir UART");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }

        // Controlador instalado

        // Configuración de las funciones de lectura/escritura
        // para usar el controlador
        esp_vfs_dev_uart_use_driver(0);

        while (1) {
            int s;
            fd_set rfds;

            struct timeval tv = {
                .tv_sec = 2,
                .tv_usec = 0,
            };

            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            s = select(fd + 1, &rfds, NULL, NULL, &tv);

            if (s < 0) {
                ESP_LOGE(TAG, "Seleccion fallida: errno %d", errno);
                break;
            } 
            else {
                if (FD_ISSET(fd, &rfds)) {
                    char buf;
                    // El bufer contiene varios caracteres, pero se leen uno a uno
                    // mediante llamadas posteriores al select, que luego regresara
                    // inmediatamente sin tiempo de espera
                    if (read(fd, &buf, 1) > 0) {
                        // Se verifica si el contenido del buffer es algun
                        // comando para indicar si el led esta encendido o apagado
                        if (buf == 'e'){
                            ESP_LOGI(TAG, "LED encendido");
                            gpio_set_level(GPIO_OUTPUT_LED, 1);
                            vTaskDelay(1000 / portTICK_PERIOD_MS);
                        }

                        if (buf == 'a'){
                            ESP_LOGI(TAG, "LED apagado");
                            gpio_set_level(GPIO_OUTPUT_LED, 0);
                            vTaskDelay(1000 / portTICK_PERIOD_MS);
                        }
                    } else {
                        ESP_LOGE(TAG, "** Error de lectura UART");
                        break;
                    }
                } 
            }
            
            // Se verifica el estado de la entrada externa, es decir, del botón
            boton = gpio_get_level(GPIO_INPUT_BOTON);
            ESP_LOGI(TAG, "Estado botón externo %d",boton);
        }
        close(fd);
    }
    vTaskDelete(NULL);
}

void app_main(void){
    xTaskCreate(uart_select_task, "uart_select_task", 4*1024, NULL, 5, NULL);
}
