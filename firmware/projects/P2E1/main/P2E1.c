/*! @mainpage Template
 *
 * @section genDesc Descripción General
 *
 * Este programa mide la distancia con un sensor ultrasónico HC-SR04 y muestra el valor en un display LCD.
 * Además, enciende LEDs según la distancia medida y permite controlar el funcionamiento con teclas.
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
 * | 17/09/2025 | Finalización del desarrollo                    |
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
/// @brief Período de medición en milisegundos.
#define CONFIG_PERIODO_MEDICION 1000
/// @brief Período de lectura de teclas en milisegundos.
#define CONFIG_PERIODO_TECLAS 100
/*==================[internal data definition]===============================*/
/// @brief Variable que almacena la distancia medida.
uint16_t distancia = 0;
/// @brief Variable que indica si el sistema está activado.
bool activar = true;
/// @brief Variable que indica si se debe sostener la lectura.
bool sostener = false;
/// @brief Variable que almacena la tecla presionada.
uint8_t tecla;
/*==================[internal functions declaration]=========================*/
/** 
* @fn medirDistancia
* @brief Tarea que mide la distancia usando el sensor HC-SR04.
* @param pvParameter Parámetro de la tarea (no se usa).
*/
static void medirDistancia(void *pvParameter)
{
	while (true)
	{
		if (activar)
		{
			distancia = HcSr04ReadDistanceInCentimeters();
		}
		vTaskDelay(CONFIG_PERIODO_MEDICION / portTICK_PERIOD_MS);
	}
}

/** 
 * @fn prenderLedSegunDistancia
* @brief Tarea que prende los LEDs según la distancia medida.
* @param pvParameter Parámetro de la tarea (no se usa).
*/
static void prenderLedSegunDistancia(void *pvParameter)
{
	while (true)
	{
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
		vTaskDelay(CONFIG_PERIODO_MEDICION / portTICK_PERIOD_MS);
	}
}

/** 
 * @fn leerTeclas
* @brief Tarea que lee el estado de las teclas y actualiza variables de control.
* @param pvParameter Parámetro de la tarea (no se usa).
*/
static void leerTeclas(void *pvParameter)
{
	while (true)
	{
		tecla = SwitchesRead();
		switch (tecla)
		{
		case SWITCH_1:
			activar = !activar;
			break;
		case SWITCH_2:
			sostener = !sostener;
			break;
		}
		vTaskDelay(CONFIG_PERIODO_TECLAS / portTICK_PERIOD_MS);
	}
}

/** 
 * @fn mostrarDistanciaEnLcd
 * @brief Tarea que muestra la distancia en el display LCD.
* @param pvParameter Parámetro de la tarea (no se usa).
*/
static void mostrarDistanciaEnLcd(void *pvParameter)
{
	while (true)
	{
		if (activar)
		{
			if (!sostener)
			{
				LcdItsE0803Write(distancia);
			}
		}
		else
		{
			LcdItsE0803Off();
		}
		vTaskDelay(CONFIG_PERIODO_MEDICION / portTICK_PERIOD_MS);
	}
}
/*==================[external functions definition]==========================*/
/// @brief Función main de la aplicación. Inicializa periféricos y crea tareas.
void app_main(void)
{
	LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();

	xTaskCreate(&medirDistancia, "Medir Distancia", 512, NULL, 5, NULL);
	xTaskCreate(&prenderLedSegunDistancia, "Prender LED Segun Distancia", 512, NULL, 5, NULL);
	xTaskCreate(&leerTeclas, "Leer Teclas", 512, NULL, 5, NULL);
	xTaskCreate(&mostrarDistanciaEnLcd, "Mostrar Distancia en LCD", 512, NULL, 5, NULL);
}
/*==================[end of file]============================================*/