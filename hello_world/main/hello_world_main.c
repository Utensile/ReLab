#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_vfs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lcd_display.h"
#include "image.h"

#define TAG "main"

void app_main()
{
    ESP_LOGI(TAG, "Initializing SD card...");

    ESP_ERROR_CHECK(lcd_display_init());  // Initialize the LCD display

    ESP_LOGI(TAG, "Mounting SD card...");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5
    };

    sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD card (%s)", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "SD card mounted");

    ESP_LOGI(TAG, "Displaying image...");

    lcd_display_clear();  // Clear the display

    ret = lcd_display_image(image_data, image_width, image_height);  // Display the image

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to display image (%s)", esp_err_to_name(ret));
    }

    ESP_LOGI(TAG, "Image displayed");

    esp_vfs_fat_sdmmc_unmount();
    ESP_LOGI(TAG, "SD card unmounted");
}
