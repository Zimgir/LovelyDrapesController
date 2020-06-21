#include <Arduino.h>
#include <pins_arduino.h>

#include "LedManager.hpp"

LedManager::led_t LedManager::leds[LED_NUM];

#ifdef LED_PWM_RESET_WORKAROUND
LedManager::pwm_t LedManager::pwms[PWM_MAX_NUM];
#endif

void LedManager::begin(output_map_t *map)
{
    LOG("LedManager::begin ...");

    for (int i = 0; i < LED_NUM; ++i)
    {
        leds[i].id = map->outputs[i].id;
        leds[i].pin = map->outputs[i].pin;
        leds[i].trig = 0;
        leds[i].trig_t0 = 0;

        pinMode(leds[i].pin, OUTPUT);
    }

#ifdef LED_PWM_RESET_WORKAROUND
    for (int i = 0; i < PWM_MAX_NUM; ++i)
    {
        pwms[i].pin = 0xFF;
        pwms[i].last_tt = 0;
        pwms[i].pwm = NULL;
    }
#endif

    LOGL("ok");
}

void LedManager::setTriggers(trigger_list_t *triggers)
{
    for (int trig_idx = 0; trig_idx < triggers->size; ++trig_idx)
    {
        for (int sig_idx = 0; sig_idx < LED_NUM; ++sig_idx)
        {
            if (triggers->ids[trig_idx] == leds[sig_idx].id)
            {
#ifdef LOG_ENABLE
                if (leds[sig_idx].trig == 0)
                {
                    LOG("[TRIG][LedManager] id = ");
                    LOGL(leds[sig_idx].id);
                }
#endif
                leds[sig_idx].trig = 1;
                leds[sig_idx].trig_t0 = millis();
            }
        }
    }
}

#ifdef LED_PWM_RESET_WORKAROUND
/* This is attempted workaround to allocate and delte pwm driver to reset it on a another pin
   Unfortunately this doesn't work and some state is left which still triggers
   a BUG when used on more than 4 different pins */
void LedManager::pwmWrite(pin_size_t pin, int val)
{
    float percent = (float)val / (float)(1 << PWM_RES_BITS);

    unsigned long min_last_tt = pwms[0].last_tt;

    int selectd_index = 0;

    for (int i = 0; i < PWM_MAX_NUM; ++i)
    {
        if (pwms[i].last_tt < min_last_tt)
        {
            selectd_index = i;
            min_last_tt = pwms[i].last_tt;
        }

        if (pwms[i].pin == pin)
        {
            selectd_index = i;
            break;
        }

        if (pwms[i].pwm == NULL)
        {
            selectd_index = i;
            break;
        }
    }

    bool is_new_pwm = false;

    if (pwms[selectd_index].pwm == NULL)
    {
        LOGVL(selectd_index, DEC);
        is_new_pwm = true;
    }
    else if (pwms[selectd_index].pin != pin)
    {
        LOGVL(selectd_index, DEC);
        LOGL("[DELETE]");
        delete pwms[selectd_index].pwm;
        pwms[selectd_index].pwm = NULL;
        delay(10);
        is_new_pwm = true;
    }

    if (is_new_pwm)
    {
        LOGVL(is_new_pwm, DEC);
        pwms[selectd_index].pin = pin;
        pwms[selectd_index].pwm = new mbed::PwmOut(digitalPinToPinName(pin));
        pwms[selectd_index].pwm->period_ms(2); //500Hz
    }

    pwms[selectd_index].last_tt = millis();
    pwms[selectd_index].pwm->write(percent);
}
#endif

void LedManager::update(void)
{
    unsigned long t1, dt, fade_time;
    uint32_t pwm_val;

    for (int i = 0; i < LED_NUM; ++i)
    {

        if (!leds[i].trig)
            continue;

        t1 = millis();

        if (t1 >= leds[i].trig_t0)
        {
            dt = t1 - leds[i].trig_t0;
        }
        else
        {
            dt = (unsigned long)(-1) - leds[i].trig_t0 + t1;
        }

        if (dt < LED_FADE_MS)
        {
            fade_time = LED_FADE_MS - dt;
        }
        else
        {
            fade_time = 0;
            leds[i].trig = 0;
        }

        pwm_val = map(fade_time, 0, LED_FADE_MS, 0, PWM_RES);

        if (i < 4)
        {
            analogWrite(leds[i].pin, pwm_val); // this crashes if used on more than 4 different pins
        }
        else
        {
            if (pwm_val > 0)
                digitalWrite(leds[i].pin, HIGH);
            else
                digitalWrite(leds[i].pin, LOW);
        }
    }
}