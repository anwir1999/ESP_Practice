/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO18: output
 * GPIO19: output
 * GPIO0:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.

 *
 */

#define GPIO_OUTPUT_IO_0    18
#define GPIO_OUTPUT_IO_1    19
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
#define GPIO_INPUT_IO_0     0
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))

int statusButton = 0;
int countButton = 0;
bool flagCheckStatus = false;
bool flagGetValue = false;
int64_t timeDebounce = 0;
const char* TAG = "ESP32";
void buttonProcess(void* arg)
{
	while(1)
	{
		statusButton = gpio_get_level(GPIO_INPUT_IO_0);
		if(!statusButton && !flagCheckStatus){
			flagGetValue = true;
			flagCheckStatus = true;
			countButton++;
			timeDebounce = esp_timer_get_time();
		}
		else if(statusButton)
		{
			flagCheckStatus = false;
		}
		if((esp_timer_get_time() - timeDebounce > 1000000) && flagGetValue)
		{
			flagGetValue = false;
			ESP_LOGI(TAG, "count: %d",countButton );
			/*
			 * xu ly bai toan nut nhan
			 */
			countButton = 0;
		}
		vTaskDelay(1);
	}
}
void app_main(void)
{
	//zero-initialize the config structure.
	gpio_config_t io_conf = {};
	//disable interrupt
	io_conf.intr_type = GPIO_INTR_DISABLE;
	//set as output mode
	io_conf.mode = GPIO_MODE_OUTPUT;
	//bit mask of the pins that you want to set,e.g.GPIO18/19
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 0;
	//disable pull-up mode
	io_conf.pull_up_en = 0;
	//configure GPIO with the given settings
	gpio_config(&io_conf);

	//interrupt of rising edge
	io_conf.intr_type = GPIO_INTR_POSEDGE;
	//bit mask of the pins, use GPIO4/5 here
	io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
	//set as input mode
	io_conf.mode = GPIO_MODE_INPUT;
	//enable pull-up mode
	io_conf.pull_up_en = 1;
	gpio_config(&io_conf);

	timeDebounce = esp_timer_get_time();
	xTaskCreate(buttonProcess, "check value button", 2048, NULL, 10, NULL);
}
