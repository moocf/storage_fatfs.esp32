#include <nvs_flash.h>
#include <esp_vfs_fat.h>
#include "macros.h"


static esp_err_t nvs_init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ERET( nvs_flash_erase() );
    ERET( nvs_flash_init() );
  }
  ERET( ret );
  return ESP_OK;
}


static esp_err_t storage_fatfs() {
  wl_handle_t handle = WL_INVALID_HANDLE;
  printf("- Mount FATFS from SPI flash as VFS\n");
  printf("(VFS enables accessing file with stdlib)\n");
  const esp_vfs_fat_mount_config_t config = {
    .max_files = 4,
    .format_if_mount_failed = true,
    .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
  };
  ERET( esp_vfs_fat_spiflash_mount("/fatfs", "storage", &config, &handle) );
  printf("- Open /fatfs/poke.txt and write to it\n");
  FILE *file = fopen("/fatfs/poke.txt", "a");
  if (file == NULL) return ESP_FAIL;
  fprintf(file, "charmender, squirtle, bulbasaur\n");
  fclose(file);
  printf("- Open /fatfs/poke.txt and read from it\n");
  file = fopen("/fatfs/poke.txt", "rb");
  char buff[1024];
  fread(buff, 1, sizeof(buff), file);
  fclose(file);
  printf("- Text read:\n%s", buff);
  return ESP_OK;
}


void app_main() {
  printf("- Initialize NVS\n");
  ESP_ERROR_CHECK( nvs_init() );
  ESP_ERROR_CHECK( storage_fatfs() );
}
