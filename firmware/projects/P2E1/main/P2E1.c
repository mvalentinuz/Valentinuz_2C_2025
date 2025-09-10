/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Mauro Valentinuz (maurovalentinuz@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "gpio_mcu.h"
#include "switch.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_PERIODO_MEDICION 1000
/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;
TaskHandle_t led2_task_handle = NULL;
TaskHandle_t led3_task_handle = NULL;
uint16_t distancia = 0;
/*==================[internal functions declaration]=========================*/
static void medirDistancia(void *pvParameter)
{
	while(true){
		distancia= HcSr04ReadDistanceInCentimeters();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

static void prenderLedSegunDistancia(void *pvParameter)
{
	while(true){
		if(distancia < 10){
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}else if(distancia >= 10 && distancia < 20){
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}else if(distancia >= 20 && distancia < 30){
			LedOn(LED_1);
			LedOn(LED_2);
			LedOff(LED_3);
		}else if(distancia >= 30){
			LedOn(LED_1);
			LedOn(LED_2);
			LedOn(LED_3);
		}
		vTaskDelay(CONFIG_PERIODO_MEDICION / portTICK_PERIOD_MS);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	uint8_t teclas;
	LedsInit();
	SwitchesInit();
	HcSr04Init();
	LcdItsE0803Init();
	xTaskCreate(medirDistancia, "Medir Distancia", 2048, NULL, 2, NULL);
	xTaskCreate(prenderLedSegunDistancia, "Prender LED Segun Distancia", 2048, NULL, 2, NULL);
}
/*==================[end of file]============================================*/