#include "Dri_NVS.h"


void Dri_NVS_Init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {        
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }    
}

esp_err_t Dri_NVS_WriteI8(char *key,int8_t value)
{
    nvs_handle_t my_handle;
    esp_err_t err= nvs_open("pwd", NVS_READWRITE, &my_handle);
    if(err!=ESP_OK) return err;

    err=nvs_set_i8(my_handle,key,value);
    if(err!=ESP_OK) return err;

    err=nvs_commit(my_handle);
    if(err!=ESP_OK) return err;

    nvs_close(my_handle);

    return err;
}

esp_err_t Dri_NVS_Delkey(char *key)
{
    nvs_handle_t my_handle;
    esp_err_t err= nvs_open("pwd", NVS_READWRITE, &my_handle);
    if(err!=ESP_OK) return err;

    
    err=nvs_erase_key(my_handle,key);
    if(err!=ESP_OK) return err;

    nvs_close(my_handle);
    return err;
}

esp_err_t Dri_NVS_IskeyMatch(char *key)
{
    nvs_handle_t my_handle;
    esp_err_t err= nvs_open("pwd", NVS_READWRITE, &my_handle);
    if(err!=ESP_OK) return err;

    nvs_iterator_t it = NULL;
    err  = nvs_entry_find_in_handle(my_handle, NVS_TYPE_I8, &it);
    while(err == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL
        //printf("key '%s', type '%d' \n", info.key, info.type);
        //遍历出来的key是否是传入的key的一部分
        if(strstr(key,info.key))
        {
            break;
        }
        err = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);

    nvs_close(my_handle);

    return err;
}