#pragma once

#define LOG_ENABLE
//#define LOG_DELAY_MAIN
#define LOOPBACK_ENABLE
#define BLE_NO_SIGN_DROP_ENABLE

#define BLE_BUILD_SERVER
//#define BLE_BUILD_CLIENT

#define LED_FADE_MS 1000
#define BLE_NO_SIGNATURE_DROP_TIME_MS 8000
#define BLE_SIGNATURE_WRITE_DELAY_MS (BLE_NO_SIGNATURE_DROP_TIME_MS / 4)
#define BLE_BLINK_WAIT_MS 100
#define BLE_SCAN_WAIT_MS 100
#define BLE_ADEVRTISE_WAIT_MS 500
#define BLE_CLIENT_WDT_TIMEOUT_SEC 30
#define BLE_SERVER_WDT_TIMEOUT_SEC 60

#define BLE_SERVER_NAME "LovelyDrapesServer"
#define BLE_CLIENT_SIGNATURE 0xC0DEB0DE
#define BLE_SERVER_SIGNATURE 0xC0FED0FE

#define BLE_UUID_SIGNATURE_SERVICE "b6147353-98ab-4672-bc32-a771ab12620b"
#define BLE_UUID_CLIENT_SIGNATURE "d60eb52e-d5bc-40ab-9d07-8c0c54ddeccc"
#define BLE_UUID_SERVER_SIGNATURE "2e504e2f-878f-4098-b95b-ac50eaa96f0b"

#define BLE_UUID_EXPORT_SERVICE "3fec2db4-7eb0-4f17-adc6-4b6db26bef86"
#define BLE_UUID_EXPORT_SET "0a42e0d3-355f-42e7-8ac6-700861572387"
#define BLE_UUID_EXPORT_CLEAR "8727451b-5ba6-45f8-8dbb-f2bd990dd9ae"

#define BLE_UUID_IMPORT_SERVICE "5c90ed27-49c4-4e25-91ca-f88ab7006067"
#define BLE_UUID_IMPORT_SET "707474e2-4954-42c8-865e-73f2f33cdab9"
#define BLE_UUID_IMPORT_CLEAR "e380107d-7748-4c71-86a0-5d02cc5ec6dc"

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

#ifdef LOOPBACK_ENABLE
#if NUM_IMPORT_SIGNALS != NUM_EXPORT_SIGNALS
#error "NUM_IMPORT_SIGNALS != NUM_EXPORT_SIGNALS"
#endif
#endif

#define LED_NUM 6

#define LINE_NUM_SENSORS 3
#define LINE_SIGNALS_PER_SENSOR 1

#define CAP_NUM_SENSORS 1
#define CAP_NUM_SIGNALS_PER_SENSOR 3

#define PWM_RES_BITS 10
#define PWM_RES (1UL << PWM_RES_BITS)
#define PWM_MAX_NUM 4 // HW constraint on Arduino nano BLE 33

#define IMPORT_NUM_SIGNALS LED_NUM
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

template <typename T>
inline uint8_t getBit(T *src, uint8_t bit)
{
    return (*src >> bit) & 0x1;
}

template <typename T>
inline void setBit(T *dst, uint8_t bit)
{
    *dst |= (0x1 << bit);
}

template <typename T>
inline void clearBit(T *dst, uint8_t bit)
{
    *dst &= ~(0x1 << bit);
}
