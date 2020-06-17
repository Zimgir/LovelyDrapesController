#pragma once

#define LOG_ENABLE
//#define BLE_NO_SIGN_DROP_ENABLE

#define BLE_SERVER_NAME "LovelyDrapesServer"
#define BLE_CLIENT_SIGNATURE 0xC0DEB0DE

#define BLE_UUID_SIGNATURE_SERVICE "b6147353-98ab-4672-bc32-a771ab12620b"

#define BLE_UUID_SIGNATURE_CHAR "d60eb52e-d5bc-40ab-9d07-8c0c54ddeccc"

#define BLE_UUID_EXPORT_SERVICE "3fec2db4-7eb0-4f17-adc6-4b6db26bef86"

#define BLE_UUID_EXPORT_CHAR_0 "0a42e0d3-355f-42e7-8ac6-700861572387"
#define BLE_UUID_EXPORT_CHAR_1 "1d10172f-94f9-47ba-8591-4a59537d6048"
#define BLE_UUID_EXPORT_CHAR_2 "82dd72b2-5861-412c-96e1-68d773de2231"
#define BLE_UUID_EXPORT_CHAR_3 "add8cdae-9715-49df-a451-77dfdbb01531"
#define BLE_UUID_EXPORT_CHAR_4 "8727451b-5ba6-45f8-8dbb-f2bd990dd9ae"
#define BLE_UUID_EXPORT_CHAR_5 "989510a4-ecd3-4588-8031-4445643add1c"

#define BLE_UUID_IMPORT_SERVICE "5c90ed27-49c4-4e25-91ca-f88ab7006067"

#define BLE_UUID_IMPORT_CHAR_0 "839d7f05-f0ad-4121-a22c-552a2d4d416a"
#define BLE_UUID_IMPORT_CHAR_1 "707474e2-4954-42c8-865e-73f2f33cdab9"
#define BLE_UUID_IMPORT_CHAR_2 "94b482eb-392c-49af-bab3-10016047a1a8"
#define BLE_UUID_IMPORT_CHAR_3 "2e504e2f-878f-4098-b95b-ac50eaa96f0b"
#define BLE_UUID_IMPORT_CHAR_4 "8dc98a41-4c73-4d3c-8be9-240618b277a6"
#define BLE_UUID_IMPORT_CHAR_5 "e380107d-7748-4c71-86a0-5d02cc5ec6dc"

#define BLE_NO_SIGNATURE_DROP_TIME_MS 2000


#define BLE_BUILD_SERVER
//#define BLE_BUILD_CLIENT

#if defined(BLE_BUILD_SERVER)
#ifdef BLE_BUILD_CLIENT
#errror "Can't build both server and client"
#endif
#elif defined(BLE_BUILD_CLIENT)
#ifdef BLE_BUILD_SERVER
#errror "Can't build both client and server"
#endif
#else
#error "Must define to build either server or client"
#endif

#define LEDS_NUM 4
#define LED_FADE_MS 1000

#define LINE_NUM_SENSORS 1
#define LINE_SIGNALS_PER_SENSOR 1

#define CAP_NUM_SENSORS 1
#define CAP_NUM_SIGNALS_PER_SENSOR 3

#define PWM_RES_BITS 10
#define PWM_RES (1UL << PWM_RES_BITS)
#define PWM_MAX_NUM 4 // HW constraint on Arduino nano BLE 33

#define IMPORT_NUM_SIGNALS LEDS_NUM
#define EXPORT_NUM_SIGNALS (LINE_NUM_SENSORS * LINE_SIGNALS_PER_SENSOR + CAP_NUM_SENSORS * CAP_NUM_SIGNALS_PER_SENSOR)

#if (IMPORT_NUM_SIGNALS >= EXPORT_NUM_SIGNALS)
#define MAX_SIGNALS IMPORT_NUM_SIGNALS
#else
#define MAX_SIGNALS EXPORT_NUM_SIGNALS
#endif

typedef void (*pin_irq_handler_t)(void);

typedef struct
{
    uint8_t size;
    uint8_t ids[MAX_SIGNALS];
} trigger_list_t;
