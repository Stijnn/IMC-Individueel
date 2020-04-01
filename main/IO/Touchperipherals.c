#include "Touchperipherals.h"

//Set play-button callback
void f_setup_event_touch_periph_play(PERIPH_BUTTON_EVENT event)
{
    on_btn_play_callback = event;
}

//Set set-button callback
void f_setup_event_touch_periph_set(PERIPH_BUTTON_EVENT event)
{
    on_btn_set_callback = event;
}

//Set volumeup-button callback
void f_setup_event_touch_periph_volup(PERIPH_BUTTON_EVENT event)
{
    on_btn_volup_callback = event;
}

//Set volumedown-button callback
void f_setup_event_touch_periph_voldown(PERIPH_BUTTON_EVENT event)
{
    on_btn_voldown_callback = event;
}

//Register the callback onboard
void f_add_callback(PERIPH_EVENT_HANDLE callback)
{
    esp_periph_set_register_callback(touch_periph_set, callback, NULL);
}

//Custom callback function
esp_err_t on_touch(audio_event_iface_msg_t* event, void* context)
{
    if (event->data == get_input_play_id() && on_btn_play_callback != NULL)
    {
        (*on_btn_play_callback)(event->cmd, event->data);
    }
    else if (event->data == get_input_set_id() && on_btn_set_callback != NULL)
    {
        (*on_btn_set_callback)(event->cmd, event->data);
    }
    else if (event->data == get_input_volup_id() && on_btn_volup_callback != NULL)
    {
        (*on_btn_volup_callback)(event->cmd, event->data);
    }
    else if (event->data == get_input_voldown_id() && on_btn_voldown_callback != NULL)
    {
        (*on_btn_voldown_callback)(event->cmd, event->data);
    }

    return ESP_OK;
}

//Initialize the onboard buttons
void f_setup_touch_periph()
{
    //Load default peripherals set and initialize it
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    touch_periph_set = esp_periph_set_init(&periph_cfg);

    //Setup the button config
    periph_touch_cfg_t touch_cfg = {
        .touch_mask = TOUCH_PAD_SEL4 | TOUCH_PAD_SEL7 | TOUCH_PAD_SEL8 | TOUCH_PAD_SEL9,
        .tap_threshold_percent = 70,
    };
    esp_periph_handle_t touch_handle = periph_touch_init(&touch_cfg);

    //Start the peripherals
    esp_periph_start(touch_periph_set, touch_handle);

    //Register the custom callback for the ESP
    f_add_callback(&on_touch);
}

//Unload the peripherals onboard
void f_destroy_touch_periph()
{
    esp_periph_set_destroy(touch_periph_set);
}