/*! @mainpage P1E4E5E6
 *
 * @section genDesc General Description
 *
 * Este programa convierte un número a BCD y muestra cada dígito en un periférico LCD utilizando pines GPIO.
 *
 * @section hardConn Hardware Connection
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
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 * | 10/09/2025 | Finalización del desarrollo                    |
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
/**
 * @struct gpioConf_t
 * @brief Estructura para la configuración de un GPIO.
 */
typedef struct
{
	gpio_t pin; /*!< GPIO pin number */
	io_t dir;	/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;
/**
 * @brief Vector que mapea los bits BCD a los GPIOs para mostrar el dígito.
 */
gpio_t mapaGPIO[4] = {GPIO_20, GPIO_21, GPIO_22, GPIO_23};
/**
 * @brief Vector que mapea los selectores de dígito a los GPIOs.
 */
gpio_t mapaGPIO2[3] = {GPIO_19, GPIO_18, GPIO_9};
/*==================[internal functions declaration]=========================*/
/**
 * @brief Convierte un número en un arreglo de dígitos BCD.
 * @param data Número a convertir.
 * @param digits Cantidad de dígitos a extraer.
 * @param bcd_number Puntero al arreglo donde se guardan los dígitos BCD.
 */
void convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	for (int i = digits - 1; i >= 0; i--)
	{
		bcd_number[i] = (data % 10);
		data /= 10;
	}
}

/**
 * @brief Setea el estado de los GPIOs según el dígito BCD.
 * @param digito Dígito BCD.
 * @param gpio Arreglo de estructuras gpioConf_t para el dígito.
 */
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

/**
 * @brief Muestra cada dígito de un número en el periférico usando los GPIOs.
 * @param numero Número a mostrar.
 * @param digitos Cantidad de dígitos.
 * @param gpio Arreglo de gpioConf_t para mostrar el dígito.
 * @param gpio_map Arreglo de gpioConf_t para seleccionar el dígito.
 */
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
/**
 * @brief Función principal de la aplicación. Inicializa los GPIOs y ejecuta la función de graficar dígitos.
 */
void app_main(void)
{
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