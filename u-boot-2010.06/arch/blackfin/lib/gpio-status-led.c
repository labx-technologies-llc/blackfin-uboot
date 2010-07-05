/*
 * U-boot - status leds via GPIOs
 *
 * Copyright (c) 2005-2010 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <config.h>
#include <command.h>
#include <status_led.h>
#include <asm/gpio.h>

static void set_led(unsigned gpio, int state)
{
	switch (state) {
	case STATUS_LED_OFF:      gpio_set_value(gpio, 0); break;
	case STATUS_LED_BLINKING: bfin_gpio_toggle_value(gpio); break;
	case STATUS_LED_ON:       gpio_set_value(gpio, 1); break;
	}
}

static void set_leds(led_id_t mask, int state)
{
#ifdef GPIO_LED0
	if (mask & 0x1) set_led(GPIO_LED0, state);
#endif
#ifdef GPIO_LED1
	if (mask & 0x2) set_led(GPIO_LED1, state);
#endif
#ifdef GPIO_LED2
	if (mask & 0x4) set_led(GPIO_LED2, state);
#endif
}

static void __led_request(unsigned gpio, int state)
{
	gpio_request(gpio, "status-led");
	gpio_direction_output(gpio, state);
}

void __led_init(led_id_t mask, int state)
{
#ifdef GPIO_LED0
	__led_request(GPIO_LED0, state);
#endif
#ifdef GPIO_LED1
	__led_request(GPIO_LED1, state);
#endif
#ifdef GPIO_LED2
	__led_request(GPIO_LED2, state);
#endif
}

void __led_set(led_id_t mask, int state)
{
	set_leds(mask, state);
}

void __led_toggle(led_id_t mask)
{
	set_leds(mask, STATUS_LED_BLINKING);
}
