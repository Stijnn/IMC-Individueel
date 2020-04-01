#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "esp_log.h"
#include "esp_peripherals.h"
#include "periph_touch.h"
#include "board_pins_config.h"


#include "nvs_flash.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "i2s_stream.h"
#include "board.h"
#include "filter_resample.h"
#include "audio_mem.h"
#include "bluetooth_service.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif