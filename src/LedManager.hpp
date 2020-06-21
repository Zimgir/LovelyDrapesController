#pragma once
#include <Arduino.h>

#include "OutputManager.hpp"

class LedManager : public OutputManager
{

public:
    struct output_t
    {
        uint8_t id;
        uint8_t pin;
    };

    struct output_map_t
    {
        output_t outputs[LED_NUM];
    };

private:
    struct led_t : public output_t
    {
        uint8_t trig;
        unsigned long trig_t0;
    };

#ifdef LED_PWM_RESET_WORKAROUND
    struct pwm_t
    {
        uint8_t pin;
        unsigned long last_tt;
        mbed::PwmOut *pwm;
    };
#endif

public:
    LedManager() {}
    ~LedManager() {}

    static void begin(output_map_t *map);
    virtual void setTriggers(trigger_list_t *triggers) override;
    void update(void) override;

private:
    static led_t leds[LED_NUM];

#ifdef LED_PWM_RESET_WORKAROUND
    static pwm_t pwms[PWM_MAX_NUM];
    static void pwmWrite(pin_size_t pin, int val);
#endif
};
