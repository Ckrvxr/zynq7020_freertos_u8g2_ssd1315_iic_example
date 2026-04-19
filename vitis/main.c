#include "FreeRTOS.h"
#include "task.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "display.h"
#include <stdio.h>

TaskHandle_t xDisplayTaskHandle;

static void vDisplayTask(void *pvParameters) {
    Display_Init();

    int count = 0;
    char buf[32];

    for( ;; ) {
        u8g2_ClearBuffer(&u8g2);
        u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
        u8g2_DrawStr(&u8g2, 0, 15, "u8g2 on Zynq!");
        
        snprintf(buf, sizeof(buf), "Count: %d", count++);
        u8g2_DrawStr(&u8g2, 0, 35, buf);
        
        u8g2_SendBuffer(&u8g2);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main( void )
{
    xTaskCreate(vDisplayTask, "Display", 2048, NULL, tskIDLE_PRIORITY + 2, &xDisplayTaskHandle);

    xil_printf("System Starting...\r\n");
    
    vTaskStartScheduler();

    for( ;; );
}
