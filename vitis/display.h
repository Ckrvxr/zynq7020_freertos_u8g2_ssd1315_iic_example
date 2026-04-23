#pragma once

#include "FreeRTOS.h"
#include "task.h"

#include "xparameters.h"
#include "xil_types.h"
#include "xiicps.h"

#include "u8g2.h"

extern u8g2_t u8g2;

void Display_Init(void);
