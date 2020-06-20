#pragma once

#if 0
#ifdef __cplusplus
extern "C" {
#endif

//extern volatile uint32_t int_counter;

typedef void (*timer_callback_t)(void);

void timer_init(uint32_t timeout_ms, timer_callback_t callback);
void timer_start(void);
void timer_stop(void);

#ifdef __cplusplus
}
#endif
#endif