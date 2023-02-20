/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-20     RT-Thread    first version
 */

#include <rtthread.h>
#include "stm32g4xx_hal.h"
#include <rtdevice.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static rt_device_t vcom_shell_device = NULL;

void change_shell(void)
{
    vcom_shell_device = rt_device_find("vcom");
    if (vcom_shell_device)
    {
        vconsole_switch(vcom_shell_device);
    }
}

static int rt_hw_spi_flash_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    rt_hw_spi_device_attach("spi3", "spi30", GPIOA, GPIO_PIN_15);

    rt_hw_spi_device_attach("spi1", "spi10", GPIOA, GPIO_PIN_4);

    //if (RT_NULL == rt_sfud_flash_probe("W25Q64CV", "spi30"))
    //{
    //    return -RT_ERROR;
    //};

    return RT_EOK;
}
/* 导出到自动初始化 */
INIT_COMPONENT_EXPORT(rt_hw_spi_flash_init);

struct rt_spi_device *  tmc2660_dev = NULL;
static uint8_t tmc2660_send_buf[6] = {0};
static uint8_t tmc2660_recv_buf[6] = {0};

void tmc2660_writeInt(uint8_t motor, uint8_t address, int value)
{
    tmc2660_send_buf[0] = address | 0x80;
    tmc2660_send_buf[1] = (0xFF & (value>>24));
    tmc2660_send_buf[2] = (0xFF & (value>>16));
    tmc2660_send_buf[3] = (0xFF & (value>>8));
    tmc2660_send_buf[4] = (0xFF & (value>>0));
    rt_spi_send(tmc2660_dev,
                tmc2660_send_buf,
                5);
}

uint32_t tmc2660_readInt(uint8_t motor, uint8_t address)
{
    uint32_t value = 0;
    tmc2660_send_buf[0] = address & 0x7f;
    rt_spi_send(tmc2660_dev,
                tmc2660_send_buf,
                1);
    rt_spi_recv(tmc2660_dev,
                tmc2660_recv_buf,
                4);

    value |= tmc2660_recv_buf[0];
    value <<= 8;
    value |= tmc2660_recv_buf[1];
    value <<= 8;
    value |= tmc2660_recv_buf[2];
    value <<= 8;
    value |= tmc2660_recv_buf[3];

    return value;
}

void tmc2660_readWrite(uint8_t motor, uint32_t value)
{
    tmc2660_send_buf[0] = (0xFF & (value>>24));
    tmc2660_send_buf[1] = (0xFF & (value>>16));
    tmc2660_send_buf[2] = (0xFF & (value>>8));
    tmc2660_send_buf[3] = (0xFF & (value>>0));
    rt_spi_transfer(tmc2660_dev,
                    tmc2660_send_buf,
                    tmc2660_recv_buf,
                    4);
}

int main(void)
{
    int count = 1;
    tmc2660_dev = (struct rt_spi_device *)rt_device_find("spi10");

    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
    cfg.max_hz = 2 * 1000 *1000;                           /* 5M */

    rt_spi_configure(tmc2660_dev, &cfg);

    change_shell();
    while (count++)
    {
        //LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
