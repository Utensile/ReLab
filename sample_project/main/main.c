#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

void app_main(void)
{
    int k=0;
    //hel
    printf("ANCESCHIPIRAMIDALE");
    while(1){
        for(int i=0; i<k; i++){
            printf("*");
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        printf("\n");
        k++;
    }
}
