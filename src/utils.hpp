#pragma once
#include "system.hpp"

typedef enum
{
    RET_OK,
    ERR_INDEX_OUT_OF_BOUNDS,
    ERR_CAP_SENSOR_BEGIN,
    ERR_BLE_BEGIN
} ret_t;

#define WDT_ENABLE(timeout)                                                               \
    do                                                                                    \
    {                                                                                     \
        NRF_WDT->CONFIG = 0x9;             /* Configure WDT to run when CPU is asleep */ \
        NRF_WDT->CRV = timeout * 32768 + 1; /* timeout (sec) = (CRV-1)/32768 */           \
        NRF_WDT->RREN = 0x01;               /* Enable the RR[0] reload register */        \
        NRF_WDT->TASKS_START = 1;           /* Start WDT */                               \
    } while (0)

#define WDT_RESET(timeout)                 \
    do                                     \
    {                                      \
        NRF_WDT->RR[0] = WDT_RR_RR_Reload; \
    } while (0)

#define LOG_SERIAL_BAUD 115200

#ifdef LOG_ENABLE
#define LOG_BEGIN()                          \
    do                                       \
    {                                        \
        Serial1.begin(LOG_SERIAL_BAUD);      \
        BLINK_WAIT(!Serial1);                \
        Serial1.println("Log::begin ...ok"); \
    } while (0)

#define LOG(m)                \
    do                        \
    {                         \
        if (Serial1)          \
            Serial1.print(m); \
    } while (0)

#define LOGL(m)                 \
    do                          \
    {                           \
        if (Serial1)            \
            Serial1.println(m); \
    } while (0)

#define LOGF(m, f)               \
    do                           \
    {                            \
        if (Serial1)             \
            Serial1.print(m, f); \
    } while (0)

#define LOGFL(m, f)                \
    do                             \
    {                              \
        if (Serial1)               \
            Serial1.println(m, f); \
    } while (0)

#define LOGV(val, f)                      \
    do                                    \
    {                                     \
        if (Serial1)                      \
        {                                 \
            Serial1.print(" " #val ": "); \
            Serial1.print(val, f);        \
            Serial1.print(" ");           \
        }                                 \
    } while (0)

#define LOGVL(val, f)                     \
    do                                    \
    {                                     \
        if (Serial1)                      \
        {                                 \
            Serial1.print(" " #val ": "); \
            Serial1.println(val, f);      \
        }                                 \
    } while (0)

#else
#define LOG_BEGIN()
#define LOG(m)
#define LOGL(m)
#define LOGF(m, f)
#define LOGFL(m, f)
#define LOGV(val, f)
#define LOGVL(val, f)
#endif

#define BLINK_FIXED(count, delay_ms)         \
    do                                       \
    {                                        \
                                             \
        for (int i = 0; i < count; ++i)      \
        {                                    \
            digitalWrite(LED_BUILTIN, HIGH); \
            delay(delay_ms);                 \
            digitalWrite(LED_BUILTIN, LOW);  \
            delay(delay_ms);                 \
        }                                    \
                                             \
    } while (0)

#define BLINK_RGB(count, delay_ms)      \
    do                                  \
    {                                   \
                                        \
        for (int i = 0; i < count; ++i) \
        {                               \
            digitalWrite(LEDR, LOW);    \
            delay(delay_ms);            \
            digitalWrite(LEDG, LOW);    \
            delay(delay_ms);            \
            digitalWrite(LEDR, HIGH);   \
            delay(delay_ms);            \
            digitalWrite(LEDB, LOW);    \
            delay(delay_ms);            \
            digitalWrite(LEDG, HIGH);   \
            delay(delay_ms);            \
            digitalWrite(LEDR, LOW);    \
            delay(delay_ms);            \
            digitalWrite(LEDB, HIGH);   \
            delay(delay_ms);            \
            digitalWrite(LEDR, HIGH);   \
            delay(delay_ms);            \
        }                               \
    } while (0)

#define BLINK_WAIT_DELAY 200

#define BLINK_WAIT(condition)         \
    do                                \
    {                                 \
                                      \
        while (condition)             \
        {                             \
            digitalWrite(LEDG, LOW);  \
            delay(BLINK_WAIT_DELAY);  \
            digitalWrite(LEDG, HIGH); \
            delay(BLINK_WAIT_DELAY);  \
        }                             \
                                      \
    } while (0)

#define BLINK_BUG_ACTIVE_DELAY 500
#define BLINK_BUG_SILENT_DELAY 3000

#define BLINK_BUG(code)                        \
    do                                         \
    {                                          \
        LOG("BUG at ");                        \
        LOG(__func__);                         \
        LOG(":");                              \
        LOGL(__LINE__);                        \
                                               \
        while (1)                              \
        {                                      \
            for (uint8_t i = 0; i < code; ++i) \
            {                                  \
                digitalWrite(LEDR, LOW);       \
                delay(BLINK_BUG_ACTIVE_DELAY); \
                digitalWrite(LEDR, HIGH);      \
                delay(BLINK_BUG_ACTIVE_DELAY); \
            }                                  \
            delay(BLINK_BUG_SILENT_DELAY);     \
        }                                      \
    } while (0)
