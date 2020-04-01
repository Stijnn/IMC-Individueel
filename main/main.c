#include "Globals.h"
#include "IO/Touchperipherals.h"
#include "Bluetooth/Bluetooth.h"

static const char* TAG = "BLUETOOTH ESP-ADF WRAPPER";

static bool running = false;
static bool playing = false;

static audio_pipeline_handle_t pipeline;
static audio_element_handle_t bt_stream_reader, i2s_stream_writer;
static esp_periph_handle_t bluetooth_periph_handle;

static void on_next_command(int cmd, void* data)
{
    if (cmd == TOUCH_UP)
    {
        f_bluetooth_call_command(BT_NEXT, bluetooth_periph_handle);
    }
}

static void on_previous_command(int cmd, void* data)
{
    if (cmd == TOUCH_UP)
    {
        f_bluetooth_call_command(BT_PREV, bluetooth_periph_handle);
    }
}

//Our callback for when we press the button
static void on_play_button_updown(int cmd, void* data)
{
    if (cmd == TOUCH_UP)
    {
        f_bluetooth_call_command((playing ? BT_PAUSE : BT_PLAY), bluetooth_periph_handle);
        playing = !playing;
    }
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    f_setup_touch_periph();
    f_setup_event_touch_periph_play(on_play_button_updown);
    f_setup_event_touch_periph_volup(on_next_command);
    f_setup_event_touch_periph_voldown(on_previous_command);

    if (f_bluetooth_create_device("Dit is het ESP bord", BLUETOOTH_RECV) == ESP_OK)
    {
        //-------------------------
        //Source: Examples
        //-------------------------

        audio_board_handle_t board_handle = audio_board_init();
        audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

        audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
        pipeline = audio_pipeline_init(&pipeline_cfg);

        i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
        i2s_cfg.type = AUDIO_STREAM_WRITER;
        i2s_stream_writer = i2s_stream_init(&i2s_cfg);

        bt_stream_reader = f_bluetooth_create_stream();

        audio_pipeline_register(pipeline, bt_stream_reader, "bt");
        audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

        #if (CONFIG_ESP_LYRATD_MSC_V2_1_BOARD || CONFIG_ESP_LYRATD_MSC_V2_2_BOARD)
            rsp_filter_cfg_t rsp_cfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
            rsp_cfg.src_rate = 44100;
            rsp_cfg.src_ch = 2;
            rsp_cfg.dest_rate = 48000;
            rsp_cfg.dest_ch = 2;
            audio_element_handle_t filter = rsp_filter_init(&rsp_cfg);
            audio_pipeline_register(pipeline, filter, "filter");
            i2s_stream_set_clk(i2s_stream_writer, 48000, 16, 2);
            audio_pipeline_link(pipeline, (const char *[]) {"bt", "filter", "i2s"}, 3);
        #else
            audio_pipeline_link(pipeline, (const char *[]) {"bt", "i2s"}, 2);
        #endif

        esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
        esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
        bluetooth_periph_handle = f_bluetooth_create_peripheral();
        esp_periph_start(set, bluetooth_periph_handle);

        audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
        audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

        audio_pipeline_set_listener(pipeline, evt);

        audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

        audio_pipeline_run(pipeline);

        //----------------------
        //Einde source examples
        //----------------------

        running = true;
        while (running)
        {
            audio_event_iface_msg_t msg;
            esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
                continue;
            }

            if (msg.cmd == AEL_MSG_CMD_ERROR) {
                ESP_LOGE(TAG, "[ * ] Action command error: src_type:%d, source:%p cmd:%d, data:%p, data_len:%d",
                     msg.source_type, msg.source, msg.cmd, msg.data, msg.data_len);
            }

            if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) bt_stream_reader
                && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) 
            {
                audio_element_info_t music_info = {0};
                audio_element_getinfo(bt_stream_reader, &music_info);

                ESP_LOGI(TAG, "[ * ] Receive music info from Bluetooth, sample_rates=%d, bits=%d, ch=%d",
                        music_info.sample_rates, music_info.bits, music_info.channels);

                audio_element_setinfo(i2s_stream_writer, &music_info);
                #if (CONFIG_ESP_LYRATD_MSC_V2_1_BOARD || CONFIG_ESP_LYRATD_MSC_V2_2_BOARD)
                #else
                    i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
                #endif
                    continue;
            }
        }

        //-------------------------
        //Source: Examples
        //-------------------------
        audio_pipeline_terminate(pipeline);

        audio_pipeline_unregister(pipeline, bt_stream_reader);
        audio_pipeline_unregister(pipeline, i2s_stream_writer);

        /* Terminate the pipeline before removing the listener */
        audio_pipeline_remove_listener(pipeline);

        /* Stop all peripherals before removing the listener */
        esp_periph_set_stop_all(set);
        audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);

        /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
        audio_event_iface_destroy(evt);

        /* Release all resources */

        #if (CONFIG_ESP_LYRATD_MSC_V2_1_BOARD || CONFIG_ESP_LYRATD_MSC_V2_2_BOARD)
            audio_pipeline_unregister(pipeline, filter);
            audio_element_deinit(filter);
        #endif
        audio_pipeline_deinit(pipeline);
        audio_element_deinit(bt_stream_reader);
        audio_element_deinit(i2s_stream_writer);
        esp_periph_set_destroy(set);
        bluetooth_service_destroy();

        //-------------------------
        //Einde source examples
        //-------------------------
    }

    f_destroy_touch_periph(); 
}
