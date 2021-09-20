/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bleprph.h"




#include <string.h>
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"





#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "addr_from_stdin.h"
#include "lwip/err.h"
#include "lwip/sockets.h"


#include "freertos/semphr.h"
#include "camera.h"
#include "esp_event_loop.h"


#include "driver/mcpwm.h"


#define SERVO_MIN_PULSEWIDTH_US (1000) // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US (2000) // Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE        (90)   // Maximum angle in degree upto which servo can rotate

#define SERVO_PULSE_GPIO1        (15)   // GPIO connects to the PWM signal line
#define SERVO_PULSE_GPIO2        (16)   // GPIO connects to the PWM signal line
static inline uint32_t example_convert_servo_angle_to_duty_us(int angle)
{
    return (angle + SERVO_MAX_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (2 * SERVO_MAX_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}


static camera_pixelformat_t s_pixel_format=CAMERA_PF_RGB565;

#define CAMERA_PIXEL_FORMAT CAMERA_PF_JPEG
#define CAMERA_FRAME_SIZE  CAMERA_FS_QVGA

#define HOST_IP_ADDR "192.168.6.102"

#define PORT 9999
static const char *payload = "Message from ESP32 ";



#define EXAMPLE_ESP_WIFI_SSID      "vaca"
#define EXAMPLE_ESP_WIFI_PASS      "22345678"
#define EXAMPLE_ESP_MAXIMUM_RETRY  32

static EventGroupHandle_t s_wifi_event_group;
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "wifi station";

static int s_retry_num = 0;



int fuck=0;



#define CONFIG_XCLK_FREQ 10000000
#define CONFIG_D0 5
#define CONFIG_D1 18
#define CONFIG_D2 19
#define CONFIG_D3 21
#define CONFIG_D4 36
#define CONFIG_D5 39
#define CONFIG_D6 2
#define CONFIG_D7 35
#define CONFIG_PCLK 22
#define CONFIG_VSYNC 25
#define CONFIG_HREF 23
#define CONFIG_SDA 26
#define CONFIG_SCL 27






static void tcp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    while (1) {

        if(fuck){

      
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(host_ip);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        while (1) {
              esp_err_t err2 = camera_run();
     
        if (err2 != ESP_OK) {
            ESP_LOGD(TAG, "Camera capture failed with error = %d", err);
            return;
        }
              ESP_LOGI(TAG, "Socket created, connecting to %d", camera_get_data_size());
            int err = send(sock, camera_get_fb(), camera_get_data_size(), 0);
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }

            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(TAG, "%s", rx_buffer);
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
        }
         vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}












static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

        fuck=1;

    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

                   
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}


#define PRINT_CORE tskNO_AFFINITY
static const char *tag = "Snake";
void print_task(void *param)
{

    for(;;)
    {
        vTaskDelay(100);
         MODLOG_DFLT(INFO, "fuck\n");
  
    }
}
static TaskHandle_t print_task_h;
void app_main(void)
{

    /* Initialize NVS it is used to store PHY calibration data */
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
//     {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);


// camera_config_t camera_config = {
//         .ledc_channel = LEDC_CHANNEL_0,
//         .ledc_timer = LEDC_TIMER_0,
//         .pin_d0 = CONFIG_D0,
//         .pin_d1 = CONFIG_D1,
//         .pin_d2 = CONFIG_D2,
//         .pin_d3 = CONFIG_D3,
//         .pin_d4 = CONFIG_D4,
//         .pin_d5 = CONFIG_D5,
//         .pin_d6 = CONFIG_D6,
//         .pin_d7 = CONFIG_D7,
//         .pin_pclk = CONFIG_PCLK,
//         .pin_vsync = CONFIG_VSYNC,
//         .pin_href = CONFIG_HREF,
//         .pin_sscb_sda = CONFIG_SDA,
//         .pin_sscb_scl = CONFIG_SCL,
//         .xclk_freq_hz = CONFIG_XCLK_FREQ,
//     };

//     camera_model_t camera_model;
//     ret = camera_probe(&camera_config, &camera_model);
//     if (ret!= ESP_OK) {
//         ESP_LOGE(TAG, "Camera probe failed with error 0x%x", ret);
//         return;
//     }

//     if (camera_model == CAMERA_OV7725) {
//         s_pixel_format = CAMERA_PIXEL_FORMAT;
//         camera_config.frame_size = CAMERA_FRAME_SIZE;
//         ESP_LOGI(TAG, "Detected OV7725 camera, using %s bitmap format",
//                 CAMERA_PIXEL_FORMAT == CAMERA_PF_GRAYSCALE ?
//                         "grayscale" : "RGB565");
//     } else if (camera_model == CAMERA_OV2640) {
//         ESP_LOGI(TAG, "Detected OV2640 camera, using JPEG format");
//         s_pixel_format = CAMERA_PIXEL_FORMAT;
//         camera_config.frame_size = CAMERA_FRAME_SIZE;
//         if (s_pixel_format == CAMERA_PF_JPEG)
//         camera_config.jpeg_quality = 15;
//     } else {
//         ESP_LOGE(TAG, "Camera not supported");
//         return;
//     }

//     camera_config.pixel_format = s_pixel_format;
//     ret = camera_init(&camera_config);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Camera init failed with error 0x%x", ret);
//         return;
//     }


     mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_PULSE_GPIO1); // To drive a RC servo, one MCPWM generator is enough
mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, SERVO_PULSE_GPIO2);
    mcpwm_config_t pwm_config = {
        .frequency = 50, // frequency = 50Hz, i.e. for every servo motor time period should be 20ms
        .cmpr_a = 0,     // duty cycle of PWMxA = 0
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);

    // while (1) {
    //     for (int angle = -SERVO_MAX_DEGREE; angle < SERVO_MAX_DEGREE; angle++) {
    //         ESP_LOGI(TAG, "Angle of rotation: %d", angle);
          mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A,1500);
           mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B,1000);
    //         vTaskDelay(pdMS_TO_TICKS(100)); //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation under 5V power supply
    //     }
    // }

//  wifi_init_sta();
//     // initBle();

//   xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);

    // xTaskCreate(&print_task, "print", 4096, NULL, 0, &print_task_h);
}
