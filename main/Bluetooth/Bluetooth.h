#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#include "..\Globals.h"

//Definities
#define BLUETOOTH_RECV BLUETOOTH_A2DP_SINK
#define BLUETOOTH_SEND BLUETOOTH_A2DP_SOURCE

#define BT_NEXT     1
#define BT_PREV     2
//#define BT_CONNECT  4
#define BT_PAUSE    8
#define BT_PLAY     16
#define BT_STOP     32
#define BT_CANCEL_DISCOVER 64
#define BT_DISCOVER 128
#define BT_FF       256
#define BT_REWIND   512

//Create and start bluetooth service
esp_err_t f_bluetooth_create_device(const char* device_name, int mode);

//Create bluetooth stream
esp_err_t f_bluetooth_create_stream();

void f_bluetooth_call_command(int cmd, esp_periph_handle_t handle);

//Create peripheral
esp_periph_handle_t f_bluetooth_create_peripheral();

#endif // !_BLUETOOTH_H