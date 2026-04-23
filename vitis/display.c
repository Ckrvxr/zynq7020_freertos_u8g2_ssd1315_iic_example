#include "display.h"

u8g2_t u8g2;

static XIicPs IicInstance;
static uint8_t i2c_buffer[128];
static uint8_t i2c_buf_idx = 0;

static uint8_t u8x8_byte_zynq_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    uint8_t *data;
    switch(msg) {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while(arg_int > 0) {
                if (i2c_buf_idx < sizeof(i2c_buffer)) {
                    i2c_buffer[i2c_buf_idx++] = *data;
                }
                data++;
                arg_int--;
            }
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            i2c_buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            // u8g2 默认传进来的地址是 8-bit (例如 0x78)，而 Zynq XIicPs 默认使用 7-bit 地址，所以要右移 1 位 (变成 0x3C)
            XIicPs_MasterSendPolled(&IicInstance, i2c_buffer, i2c_buf_idx, u8x8_GetI2CAddress(u8x8) >> 1);
            while (XIicPs_BusIsBusy(&IicInstance));
            break;
        default:
            return 0;
    }
    return 1;
}

static uint8_t u8x8_gpio_and_delay_zynq(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch(msg) {
        case U8X8_MSG_DELAY_MILLI:
            vTaskDelay(pdMS_TO_TICKS(arg_int));
            break;
        default:
            return 0;
    }
    return 1;
}

void Display_Init(void) {
    // 1. 初始化 Zynq I2C 硬件
    XIicPs_Config *Config = XIicPs_LookupConfig(XPAR_PS7_I2C_0_DEVICE_ID);
    XIicPs_CfgInitialize(&IicInstance, Config, Config->BaseAddress);
    XIicPs_SetSClk(&IicInstance, 400000);
    // 2. 等待 IIC 信道初始化完成并稳定
    vTaskDelay(pdMS_TO_TICKS(100));
    // 3. 初始化 u8g2
    u8g2_Setup_ssd1315_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_zynq_hw_i2c, u8x8_gpio_and_delay_zynq);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}
