#include <Arduino.h>
//#include <nrfx_timer.h>
#include "utils.hpp"
#include "timer.hpp"



#define TIMER_1MHZ_PRESCALER 4
#define TIMER_1MHZ_1MS_COMPARE 1000

/*#define NRFX_TIMER4_INST_IDX 4
nrfx_timer_t led_timer = NRFX_TIMER_INSTANCE(4);
nrfx_timer_config_t led_timer_config = NRFX_TIMER_DEFAULT_CONFIG;

void led_timer_callback(nrf_timer_event_t event_type, void* p_context){
  //LOG("timer_callback: ");
  int_counter++;
  led_state = ~led_state;
  //digitalWrite(LED_BUILTIN, led_state);
  //LOG("ok\n");
}

void led_timer_init(void){
    LOG("led_timer_init: ");
    led_timer_config.frequency = NRF_TIMER_FREQ_1MHz;
    nrfx_timer_init(&led_timer,&led_timer_config,led_timer_callback);
    // set timer interrupt every 1000 ticks (1ms) and clear ticks on interrupt
    nrfx_timer_extended_compare(&led_timer,NRF_TIMER_CC_CHANNEL0,1000,NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,true);
    nrfx_timer_compare_int_enable(&led_timer,NRF_TIMER_CC_CHANNEL0);
    nrfx_timer_enable(&led_timer);   
    LOG("ok\n");
}*/

/*void timer_callback(void){
  //LOG("timer_callback: ");
  int_counter++;
  led_state = ~led_state;
  //digitalWrite(LED_BUILTIN, led_state);
  //LOG("ok\n");
}*/

#ifdef __cplusplus
extern "C" {
#endif

volatile static struct {
  uint32_t timeout_ms;
  uint32_t cur_ms;
  timer_callback_t timeout_callback;
} TIMER;

	
void timer_init(uint32_t timeout_ms, timer_callback_t callback)
{	

  LOG("timer_init: ");

  if(!callback){
    LOG("error(!callback)\n");
    BLINK_BUG(ERR_INDEX_OUT_OF_BOUNDS);
  }
    
  /* stop timer if it was somehow running (timer must be stopped for configuration) */
  NRF_TIMER4->TASKS_STOP  = 1;

  delay(1);

  /* clear timer to zero count */
  NRF_TIMER4->TASKS_CLEAR = 1;

  /* Clear out and enable TIMER4 interrupt at system level. */
  NRF_TIMER4->INTENCLR = 0xFFFFFFFF;

  NRF_TIMER4->EVENTS_COMPARE[0] = 0;
  NRF_TIMER4->EVENTS_COMPARE[1] = 0;
  NRF_TIMER4->EVENTS_COMPARE[2] = 0;
  NRF_TIMER4->EVENTS_COMPARE[3] = 0;
  NRF_TIMER4->EVENTS_COMPARE[4] = 0;
  NRF_TIMER4->EVENTS_COMPARE[5] = 0;

  /* configure timer */
  NRF_TIMER4->MODE      = TIMER_MODE_MODE_Timer;
  NRF_TIMER4->BITMODE   = TIMER_BITMODE_BITMODE_16Bit;
  NRF_TIMER4->PRESCALER = TIMER_1MHZ_PRESCALER;  /* f = 16MHz / (2 ^ TIMER_PRESCALER) */

  /* Set compare 0 value. */
  NRF_TIMER4->CC[0] =  TIMER_1MHZ_1MS_COMPARE;
 // NRF_TIMER4->CC[1] = 2 * TIMER_1MHZ_1MS_COMPARE;
  //NRF_TIMER4->CC[2] = 3 * TIMER_1MHZ_1MS_COMPARE;
  //NRF_TIMER4->CC[3] = 4 * TIMER_1MHZ_1MS_COMPARE;
  //NRF_TIMER4->CC[4] = 5 * TIMER_1MHZ_1MS_COMPARE;
 // NRF_TIMER4->CC[5] = 6 * TIMER_1MHZ_1MS_COMPARE;


  NVIC_SetPriority(TIMER4_IRQn, 1);
  NVIC_ClearPendingIRQ(TIMER4_IRQn);
  NVIC_EnableIRQ(TIMER4_IRQn);

  TIMER.timeout_ms = timeout_ms;
  TIMER.timeout_callback = callback;
  TIMER.cur_ms = 0;

  /* Enable TIMER4 interrupt source for CC[0].  */
  NRF_TIMER4->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  /* TIMER4 is a free running clock. */
  NRF_TIMER4->TASKS_START = 1;

  LOG("ok\n");
}

void timer_start(void) {

  LOG("timer_start: ");

  /* stop timer if it was somehow running (timer must be stopped for configuration) */
  //NRF_TIMER4->TASKS_STOP = 1;

  /* Clear pending events. */
  //NRF_TIMER4->EVENTS_COMPARE[0] = 0;

  //TIMER.cur_ms = 0;



  LOG("ok\n");
}

void timer_stop(void) {

    LOG("timer_stop: ");

    /* stop timer if it was somehow running (timer must be stopped for configuration) */
    NRF_TIMER4->TASKS_STOP = 1;

    /* Clear pending events. */
    //NRF_TIMER4->EVENTS_COMPARE[0] = 0;

    //TIMER.cur_ms = 0;

    /* Disable this interrupt */
    //NRF_TIMER4->INTENCLR = TIMER_INTENSET_COMPARE0_Msk;

    LOG("ok\n");
}  

/** TIMTER3 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER4 interrupt
 */
void nrfx_timer_4_irq_handler_v(void)
{
  //int_counter++;	

  //LOG("TIMER4_IRQHandler_v: ");

  /*if (NRF_TIMER4->EVENTS_COMPARE[0] == 0) {
    // event is not set
    //LOG("error(!event)\n");
    return;
  }*/

  /*if((NRF_TIMER4->INTENSET & TIMER_INTENSET_COMPARE0_Msk) == 0) {
    // interrupt not enabled
    //LOG("error(!enable)\n");
    return;
  }*/

  /* Disable this interrupt */
  NRF_TIMER4->INTENCLR = TIMER_INTENSET_COMPARE0_Msk;

  /* Clear event again just in case. */
  NRF_TIMER4->EVENTS_COMPARE[0] = 0; 

  /* clear timer to zero count */
  NRF_TIMER4->TASKS_CLEAR = 1;

  NVIC_ClearPendingIRQ(TIMER4_IRQn);

  /* Enable TIMER4 interrupt source for CC[0].  */
  NRF_TIMER4->INTENSET = TIMER_INTENSET_COMPARE0_Msk;


  /*TIMER.cur_ms++;

  if (TIMER.cur_ms == TIMER.timeout_ms){
    TIMER.cur_ms = 0;
    TIMER.timeout_callback();
  }*/

  //NRF_TIMER4->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  //LOG("ok\n");
}

#ifdef __cplusplus
}
#endif