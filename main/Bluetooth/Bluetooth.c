#include "Bluetooth.h"

esp_err_t f_bluetooth_create_device(const char* device_name, int mode)
{
    bluetooth_service_cfg_t bt_cfg = {
        .device_name = device_name,
        .mode = mode,
    };
    return bluetooth_service_start(&bt_cfg);
}

esp_err_t f_bluetooth_create_stream()
{
    return bluetooth_service_create_stream();
}

void f_bluetooth_call_command(int cmd, esp_periph_handle_t handle)
{
    switch (cmd)
    {
        case BT_NEXT:
            periph_bluetooth_next(handle);
        break;

        case BT_PAUSE:
            periph_bluetooth_pause(handle);
        break;

        case BT_PLAY:
            periph_bluetooth_play(handle);
        break;

        case BT_STOP:
            periph_bluetooth_stop(handle);
        break;

        case BT_PREV:
            periph_bluetooth_prev(handle);
        break;

        case BT_CANCEL_DISCOVER:
            periph_bluetooth_cancel_discover(handle);
        break;

        case BT_DISCOVER:
            periph_bluetooth_discover(handle);
        break;

        case BT_FF:
            periph_bluetooth_fast_forward(handle);
        break;

        case BT_REWIND:
            periph_bluetooth_rewind(handle);
        break;
    }
}

esp_periph_handle_t f_bluetooth_create_peripheral()
{
    return bluetooth_service_create_periph();
}