#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

void app_main(void)
{
    int i=0;
    while(1){
        printf("%d\n", i);
        vTaskDelay(40 / portTICK_PERIOD_MS);
        i+=40;
    }
}
