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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin; /*!< GPIO pin number */
	io_t dir;	/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal functions declaration]=========================*/
void convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	for (int i = digits - 1; i >= 0; i--)
	{
		bcd_number[i] = (data % 10);
		data /= 10;
	}
}

gpio_t mapaGPIO[4] = {GPIO_20, GPIO_21, GPIO_22, GPIO_23};
gpio_t mapaGPIO2[3] = {GPIO_19, GPIO_18, GPIO_9};

void setGPIO(uint8_t digito, gpioConf_t *gpio)
{
	for (int i = 0; i < 4; i++)
	{
		if (digito & (1 << i)) // compara desplazandose por los bits para encender el LED correspondiente
		{
			GPIOOn(gpio[i].pin);
		}
		else
		{
			GPIOOff(gpio[i].pin);
		}
	}
}

void graficarDigitos(uint32_t numero, uint8_t digitos, gpioConf_t *gpio, gpioConf_t *gpio_map)
{
	uint8_t arreglo[digitos];
	convertToBcdArray(numero, digitos, arreglo);
	for (int i = 0; i < digitos; i++)
	{
		setGPIO(arreglo[i], gpio);
		GPIOOn(gpio_map[i].pin);
		GPIOOff(gpio_map[i].pin);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	/*
	uint8_t arreglo[3];
	convertToBcdArray(137, 3, arreglo);

	for (int i = 0; i < 3; i++)
	{
		printf("%d", arreglo[i]);
	}
	*/
	gpioConf_t gpio[] = {{mapaGPIO[0], GPIO_OUTPUT}, {mapaGPIO[1], GPIO_OUTPUT}, {mapaGPIO[2], GPIO_OUTPUT}, {mapaGPIO[3], GPIO_OUTPUT}};

	gpioConf_t gpio_map[] = {{mapaGPIO2[0], GPIO_OUTPUT}, {mapaGPIO2[1], GPIO_OUTPUT}, {mapaGPIO2[2], GPIO_OUTPUT}};
	for (int i = 0; i < 4; i++)
	{
		GPIOInit(gpio[i].pin, gpio[i].dir);
	}

	for (int i = 0; i < 3; i++)
	{
		GPIOInit(gpio_map[i].pin, gpio_map[i].dir);
	}

	graficarDigitos(251, 3, gpio, gpio_map);
}
/*==================[end of file]============================================*/