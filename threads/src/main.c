/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <zephyr/sys/printk.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

/* size of stack area used by each thread */
#define STACKSIZE 128

void threadA(void *dummy1, void *dummy2, void *dummy3);
void threadB(void *dummy1, void *dummy2, void *dummy3);

K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;

K_THREAD_STACK_DEFINE(threadB_stack_area, STACKSIZE);
static struct k_thread threadB_data;

#define LED DT_ALIAS(csled)
#define LEDY DT_ALIAS(yellowled)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED, gpios);
static const struct gpio_dt_spec ledy = GPIO_DT_SPEC_GET(LEDY, gpios);

void main(void)
{
	// const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	// uint32_t dtr = 0;
	int32_t ret;

	// if (usb_enable(NULL))
	// {
	// 	return;
	// }

	// /* Poll if the DTR flag was set */
	// while (!dtr)
	// {
	// 	uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
	// 	/* Give CPU resources to low priority threads. */
	// 	k_sleep(K_MSEC(100));
	// }

	if (!device_is_ready(led.port))
	{
		// LOG_ERR("LED is not ready");
		return 0;
	}
	if (!device_is_ready(ledy.port))
	{
		// LOG_ERR("YLED is not ready");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		// LOG_ERR("Could not configure led as output");
		return;
	}
	ret = gpio_pin_configure_dt(&ledy, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		// LOG_ERR("Could not configure led as output");
		return;
	}

	// printk("Multi thread example\n");

	k_thread_create(&threadA_data, threadA_stack_area, K_THREAD_STACK_SIZEOF(threadA_stack_area), threadA, NULL, NULL, NULL, 7, 0, K_FOREVER);
	k_thread_name_set(&threadA_data, "thread_a");

	k_thread_create(&threadB_data, threadB_stack_area, K_THREAD_STACK_SIZEOF(threadB_stack_area), threadB, NULL, NULL, NULL, 7, 0, K_FOREVER);
	k_thread_name_set(&threadB_data, "thread_b");

	k_thread_start(&threadA_data);
	k_thread_start(&threadB_data);
}

void threadA(void *dummy1, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	gpio_pin_set_dt(&led, 1);

	while (1)
	{
		printk("ThreadA\n");
		gpio_pin_toggle_dt(&led);
		k_msleep(1000);
	}
}

void threadB(void *dummy1, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	while (1)
	{
		printk("ThreadB\n");
		gpio_pin_toggle_dt(&ledy);
		k_msleep(1000);
	}
}