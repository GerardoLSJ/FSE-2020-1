//  PROYECTO FINAL GSM/GPRS
//     Sensado de temperatura con alertas SMS

// Alumnos:
//      López Santibáñez Jiménez Luis Gerardo
//      Ortiz Figueroa María Fernanda
//      Robles Uribe Karen Abril

// Asignatura:
//      Fundamentos de Sistemas Embebidos

// Descripción:
//      Lectura de temperatura a través de un GPIO de la ESP32 proveniente del 
//      sensor LM335 (10mV/K°), por lo que se lee el valor en mV,de tal forma 
//      que se hace lo siguiente:
//      *****
//          Conversión para tener la temperatura en Kelvin y Centigrados para 
//          enviar vía UART del ESP32 a la Computadora, para verificar la 
//          información que se esta obteniendo del sensor
//      *****
//          Suponemos que se requiere enviar mensajes cada hora (1 minuto por 
//          fines prácticos) con la temperatura sensada en ese momento


#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_adc_cal.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "driver/adc.h"

// Pines para la comunicación UART entre la ESP32 y el SIM800L
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

// Pin para el led de verificación de la lectura de temperatura
#define GPIO_OUTPUT_LED 18

//Voltaje de referencia para obtener una mejor estimación
#define DEFAULT_VREF    1100 
//Número de muestras para el multimuestreo       
#define NO_OF_SAMPLES   64          

static esp_adc_cal_characteristics_t *adc_chars;
//GPIO36 para ADC1, corresponde al pin 14 VP
static const adc_channel_t channel = ADC1_CHANNEL_0;
//Atenuación     
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

//Valores de calibración, voltaje de referencia para ADC
static void check_efuse(void){
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } 
    else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } 
    else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type){
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } 
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } 
    else {
        printf("Characterized using Default Vref\n");
    }
}

void inicio(void) {
    // Configuración del UART
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    // TX y RX UART 2, para comunicar con EL SIM800L
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // El buffer no se usa para mandar datos
    uart_driver_install(UART_NUM_2, 1024 * 2, 0, 0, NULL, 0);

    // TX y RX UART 0, para comunicar con la    
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 2*1024, 0, 0, NULL, 0);

    //Verificación del voltaje de referencia
    check_efuse();

    //Configuración ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, atten);
    } 
    else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Caracterización ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
}

// Transmisión de datos vía UART de la ESP32 al SIM800L
int enviaDatosTX(const char* logName, const char* data){
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_2, data, len);
   
    ESP_LOGI(logName, "Se han transmitido %d bytes a la SIM800L", txBytes);
    
    return txBytes;
}

// Recepción de datos vía UART deL SIM800L a la ESP32
// Función de verificación de datos transmitidos
static void recibeDatosRX(){
    esp_log_level_set("TAREA RX ** ", ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(2048+1);
    const int rxBytes = uart_read_bytes(UART_NUM_2, data, 2048, 1000 / portTICK_RATE_MS);
    printf("/////");
    if (rxBytes > 0) {
        data[rxBytes] = 0;
        ESP_LOGI("TAREA RX ** ", "Read %d bytes: %s", rxBytes, data);
    }

    free(data);
}

int temperatura(void){
    //Muestra de ADC
    uint32_t adc_reading = 0;

    //Multimuestreo
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        if (unit == ADC_UNIT_1) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        } else {
            int raw;
            adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
            adc_reading += raw;
        }
    }

    adc_reading /= NO_OF_SAMPLES;

    //Convirtiendo el valor leído por ADC a voltaje en mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);

    vTaskDelay(pdMS_TO_TICKS(1000));

    return voltage;
}

static void uart(void *arg){
    char tempLeida [5];
    int fd,tomarTemperatura, tempAux;
    float k, c;
    
    // Configurando puerto de salida para observar
    // si se hace uno la lectura de la temperatura
    gpio_pad_select_gpio(GPIO_OUTPUT_LED);
    gpio_set_direction(GPIO_OUTPUT_LED, GPIO_MODE_OUTPUT);
    int cont = 0;
    while (1){
        //Instalando el controlador
        if ((fd = open("/dev/uart/0", O_RDWR)) == -1) {
            ESP_LOGE("UART ", "Cannot open UART");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
        
        // Configuración de las funciones de lectura/escritura
        // para usar el controlador
        esp_vfs_dev_uart_use_driver(0);
        
        while (cont <= 2) {
            int s;
            fd_set rfds;

            struct timeval tv = {
                .tv_sec = 0.1,
                .tv_usec = 0,
            };

            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            s = select(fd + 1, &rfds, NULL, NULL, &tv);

            if (s < 0) {
                ESP_LOGE("Tarea TX UART-PC ** ", "Conexion fallida: errno %d", errno);
                break; 
            } 
            else {
                if (FD_ISSET(fd, &rfds)) {
                    char buf;
                    if (read(fd, &buf, 1) > 0) {
                    } else {
                        ESP_LOGE("Tarea TX UART-PC ** ", "** Error de lectura UART");
                        break;
                    }
                }
            }
        
            // Calculo de la temperatura leida en K y C
            tempAux = temperatura();
            k = tempAux/10.0;
            c = k - 273.15; 
            
            // Transmisión de los valores leídos de la temperatura vía UART de la ESP32 a la Computadora
            ESP_LOGI("Tarea TX UART-PC ** ", ">> Voltaje: %dmV  Kelvin: %.2f  Centigrados: %.2f", tempAux, k, c);
            
            // Se verifica comunicación de la PC con el SIM800L
            enviaDatosTX("Tarea TX UART-SIM800L ** ", "AT+CMGF=1\x0D");
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS); 
            
            // Se verifica la señal que se tiene, la cual nos la muestra en decibeles
            enviaDatosTX("Tarea TX UART-SIM800L ** ", "AT+CSQ\x0D");
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);

            // Se verifica que este conectado a alguna red, debe ser diferente de 0
            enviaDatosTX("Tarea TX UART-SIM800L ** ", "AT+CREG?\x0D");
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            // Transmisión del voltaje leido del sensor al SIM800L vía UART
            // indicando que se envíe un mensaje con dicha información al numero especificado
            enviaDatosTX("Tarea TX UART-SIM800L ** ", "AT+CMGS=\"+525614772058\"\x0D");
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);

            enviaDatosTX("Tarea TX UART-SIM800L ** ", "Enviado desde la ESP32\n Temperatura sensada\n");
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            // Conversión de la temperatura leída en Kelvin del sensor a cadena para enviar
            itoa(k, tempLeida, 10);

            enviaDatosTX("Tarea TX UART-SIM800L ** ", " ->Kelvin: ");
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);

            enviaDatosTX("Tarea TX UART-SIM800L ** ", tempLeida);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            // Conversión de la temperatura leída en Centigrados del sensor a cadena para enviar
            itoa(c, tempLeida, 10);

            enviaDatosTX("Tarea TX UART-SIM800L ** ", "\n ->Centigrados: ");
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);

            enviaDatosTX("Tarea TX UART-SIM800L ** ", tempLeida);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);

            enviaDatosTX("Tarea TX UART-SIM800L ** ", "\x1A");
            vTaskDelay(250 / portTICK_PERIOD_MS);

            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);

            recibeDatosRX();
            vTaskDelay(250 / portTICK_PERIOD_MS);
            
            cont++;

        }
        close(fd);
    }
    vTaskDelete(NULL);
    
    exit(0);
}

void app_main(void){

    inicio();
    xTaskCreate(uart, "UART-PC-SIM800L", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);

}
