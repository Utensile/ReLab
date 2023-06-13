#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

void app_main(void)
{
    float i=0;
    while(1){
        printf("%f\n", i);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        i+=0.1;
    }
}
