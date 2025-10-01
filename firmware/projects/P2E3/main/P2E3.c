/*! @mainpage P2E3
 *
 * @section genDesc Descripción General
 *
 * Este programa mide la distancia con un sensor ultrasónico HC-SR04 y muestra el valor en un display LCD.
 * Además, enciende LEDs según la distancia medida y permite controlar el funcionamiento con teclas incorporando el uso de interrupciones y timer.
 *
 * <a href="https://drive.google.com/file/d/1yIPn12GYl-s8fiDQC3_fr2C4CjTvfixg/view">Ejemplo de operación</a>
 *
 * @section hardConn Hardware Connection
 *
 * |   HC_SR04      |   EDU-CIAA	|
 * |:--------------:|:-------------:|
 * | 	Vcc 	    |	5V      	|
 * | 	Echo		| 	GPIO_3		|
 * | 	Trig	 	| 	GPIO_2		|
 * | 	Gnd 	    | 	GND     	|
 *
 * |   Display      |   EDU-CIAA	|
 * |:--------------:|:-------------:|
 * | 	Vcc 	    |	5V      	|
 * | 	BCD1		| 	GPIO_20		|
 * | 	BCD2	 	| 	GPIO_21		|
 * | 	BCD3	 	| 	GPIO_22		|
 * | 	BCD4	 	| 	GPIO_23		|
 * | 	SEL1	 	| 	GPIO_19		|
 * | 	SEL2	 	| 	GPIO_18		|
 * | 	SEL3	 	| 	GPIO_9		|
 * | 	Gnd 	    | 	GND     	|
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 10/09/2025 | Creación del documento                         |
 * | 01/10/2025 | Finalización del desarrollo                    |
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
#include "timer_mcu.h"
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/

/// @brief Período de medición en milisegundos.
#define CONFIG_PERIODO_MEDICION 1000000

/*==================[internal data definition]===============================*/

/// @brief Variable que almacena la distancia medida.
uint16_t distancia = 0;

/// @brief Variable que indica si el sistema está activado.
bool activar = true;

/// @brief Variable que indica si se debe sostener la lectura.
bool sostener = false;

/// @brief Variable que almacena la tecla presionada.
uint8_t tecla;

/// @brief TaskHandle para la tarea de medir distancia.
TaskHandle_t medirDistanciaTaskHandle = NULL;

/// @brief TaskHandle para la tarea de mostrar distancia en LCD.
TaskHandle_t mostrarDistanciaEnLcdTaskHandle = NULL;

/// @brief TaskHandle para la tarea de prender LED según distancia.
TaskHandle_t prenderLedSegunDistanciaTaskHandle = NULL;

/*==================[internal functions declaration]=========================*/

/** 
* @fn medirDistancia(void *pvParameter)
* @brief Tarea que mide la distancia usando el sensor HC-SR04.
* @param pvParameter Parámetro de la tarea (no se usa).
*/
static void medirDistancia(void *pvParameter)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (activar)
		{
			distancia = HcSr04ReadDistanceInCentimeters();
		}
	}
}

/** 
 * @fn timerA
 * @brief Función que envia notificaciones a las tareas.
 */
static void timerA(){
	vTaskNotifyGiveFromISR(medirDistanciaTaskHandle, pdFALSE);
	vTaskNotifyGiveFromISR(mostrarDistanciaEnLcdTaskHandle, pdFALSE);
	vTaskNotifyGiveFromISR(prenderLedSegunDistanciaTaskHandle, pdFALSE);
}

/** 
 * @fn prenderLedSegunDistancia(void *pvParameter)
* @brief Tarea que prende los LEDs según la distancia medida.
* @param pvParameter Parámetro de la tarea (no se usa).
*/
static void prenderLedSegunDistancia(void *pvParameter)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (activar)
		{
			if (distancia < 10)
			{
				LedOff(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
			else if (distancia >= 10 && distancia < 20)
			{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
			else if (distancia >= 20 && distancia < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}
			else if (distancia >= 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
			}
		}
		else
		{
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
	}
}
/** 
 * @fn switchActivar()
 * @brief Tarea que cambia el estado de la variable de control 'activar'.
*/
static void switchActivar()
{
	activar = !activar;
}

/** 
 * @fn switchSostener()
 * @brief Tarea que cambia el estado de la variable de control 'sostener'.
 */
static void switchSostener()
{
	sostener = !sostener;
}

/** 
 * @fn mostrarDistanciaEnLcd(void *pvParameter)
 * @brief Tarea que muestra la distancia en el display LCD y por UART.
* @param pvParameter Parámetro de la tarea (no se usa).
*/
static void mostrarDistanciaEnLcd(void *pvParameter)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (activar)
		{
			if (!sostener)
			{
				LcdItsE0803Write(distancia);
			}
			UartSendString(UART_PC, (char *)UartItoa(distancia, 10));
			UartSendString(UART_PC, " cm\r\n");
		}
		else
		{
			LcdItsE0803Off();
		}
	}
}

/** 
 * @fn funcionUart(void *param)
 * @brief Función que maneja la comunicación UART para la recepción de comandos por tecla.
 * @param param Parámetro de la función (no se usa).
 */
void funcionUart(void *param){
	uint8_t opcion;
	UartReadByte(UART_PC, &opcion);
	switch (opcion)
	{
		case 'O':
			switchActivar();
			break;
		case 'H':
			switchSostener();
			break;
	}
}
/*==================[external functions definition]==========================*/

/// @brief Función main de la aplicación. Inicializa periféricos, puerto UART y crea tareas.
void app_main(void)
{
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();
	timer_config_t timer1 = {
		.timer = TIMER_A,
		.period = CONFIG_PERIODO_MEDICION,
		.func_p = timerA,
		.param_p = NULL};
	TimerInit(&timer1);

	serial_config_t uart = {
		.port= UART_PC,
		.baud_rate= 9600, //bits por segundo
		.func_p= funcionUart,
		.param_p= NULL
	};
	UartInit(&uart);

	xTaskCreate(&medirDistancia, "Medir Distancia", 512, NULL, 5, &medirDistanciaTaskHandle);
	xTaskCreate(&prenderLedSegunDistancia, "Prender LED Segun Distancia", 512, NULL, 5, &prenderLedSegunDistanciaTaskHandle);
	SwitchActivInt(SWITCH_1, switchActivar, NULL);
	SwitchActivInt(SWITCH_2, switchSostener, NULL);
	xTaskCreate(&mostrarDistanciaEnLcd, "Mostrar Distancia en LCD", 512, NULL, 5, &mostrarDistanciaEnLcdTaskHandle);
	TimerStart(timer1.timer);
}
/*==================[end of file]============================================*/