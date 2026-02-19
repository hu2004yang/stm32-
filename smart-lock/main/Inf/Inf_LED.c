#include "Inf_LED.h"
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"

/* 定义几种常见颜色 */
uint8_t black[3]  = {0, 0, 0};
uint8_t white[3]  = {255, 255, 255};
uint8_t red[3]    = {0, 255, 0};
uint8_t green[3]  = {255, 0, 0};
uint8_t blue[3]   = {0, 0, 255};
uint8_t cyan[3]   = {255, 0, 255}; /* 青色 */
uint8_t purple[3] = {0, 255, 255}; /* 紫色 */


#define RMT_LED_STRIP_RESOLUTION_HZ 10000000

rmt_channel_handle_t led_chan = NULL;
rmt_encoder_handle_t simple_encoder = NULL;
rmt_transmit_config_t tx_config = {
        .loop_count = 0, // no transfer loop
    };

static const rmt_symbol_word_t ws2812_zero = {
    .level0 = 1,
    .duration0 = 0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T0H=0.3us
    .level1 = 0,
    .duration1 = 0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T0L=0.9us
};

static const rmt_symbol_word_t ws2812_one = {
    .level0 = 1,
    .duration0 = 0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T1H=0.9us
    .level1 = 0,
    .duration1 = 0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T1L=0.3us
};

//reset defaults to 50uS
static const rmt_symbol_word_t ws2812_reset = {
    .level0 = 1,
    .duration0 = RMT_LED_STRIP_RESOLUTION_HZ / 1000000 * 50 / 2,
    .level1 = 0,
    .duration1 = RMT_LED_STRIP_RESOLUTION_HZ / 1000000 * 50 / 2,
};

static size_t encoder_callback(const void *data, size_t data_size,
                               size_t symbols_written, size_t symbols_free,
                               rmt_symbol_word_t *symbols, bool *done, void *arg)
{
    // We need a minimum of 8 symbol spaces to encode a byte. We only
    // need one to encode a reset, but it's simpler to simply demand that
    // there are 8 symbol spaces free to write anything.
    if (symbols_free < 8) {
        return 0;
    }

    // We can calculate where in the data we are from the symbol pos.
    // Alternatively, we could use some counter referenced by the arg
    // parameter to keep track of this.
    size_t data_pos = symbols_written / 8;
    uint8_t *data_bytes = (uint8_t*)data;
    if (data_pos < data_size) {
        // Encode a byte
        size_t symbol_pos = 0;
        for (int bitmask = 0x80; bitmask != 0; bitmask >>= 1) {
            if (data_bytes[data_pos]&bitmask) {
                symbols[symbol_pos++] = ws2812_one;
            } else {
                symbols[symbol_pos++] = ws2812_zero;
            }
        }
        // We're done; we should have written 8 symbols.
        return symbol_pos;
    } else {
        //All bytes already are encoded.
        //Encode the reset, and we're done.
        symbols[0] = ws2812_reset;
        *done = 1; //Indicate end of the transaction.
        return 1; //we only wrote one symbol
    }
}

void Inf_LED_Init(void)
{
    //1.创建一个发送通道
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = 6,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    rmt_new_tx_channel(&tx_chan_config, &led_chan);

    //2.创建编码器    
    const rmt_simple_encoder_config_t simple_encoder_cfg = {
        .callback = encoder_callback
        //Note we don't set min_chunk_size here as the default of 64 is good enough.
    };
    rmt_new_simple_encoder(&simple_encoder_cfg, &simple_encoder);

    //3.使能发送通道
    rmt_enable(led_chan);
}

uint8_t led_colors[36]={0};
void Inf_LED_LightLeds(void)
{
    rmt_transmit(led_chan, simple_encoder, led_colors, sizeof(led_colors), &tx_config);

    rmt_tx_wait_all_done(led_chan, portMAX_DELAY);

}

void Inf_LED_Lightkeyled(Touch_Key_t key,uint8_t color[])
{
    memset(led_colors,0,sizeof(led_colors));
    memcpy(&led_colors[key*3],color,3);

    Inf_LED_LightLeds();
}

void Inf_LED_LightAll(uint8_t color[])
{
    memset(led_colors,0,sizeof(led_colors));
    for(size_t i=0;i<12;i++)
    {
        memcpy(&led_colors[i*3],color,3);
    }
    Inf_LED_LightLeds();
}