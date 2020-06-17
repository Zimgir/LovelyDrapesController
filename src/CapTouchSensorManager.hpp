#pragma once
#include <Arduino.h>
#include <SparkFun_CAP1203.h>

#include "InputManager.hpp"

#define SYM_CAP_IRQ_HANDLER __cap_irq_handler_
#define CAP_IRQ_HANDLER(ID) SYM_CAP_IRQ_HANDLER##ID
#define DEC_CAP_IRQ_HANDLER(ID) static void CAP_IRQ_HANDLER(ID)(void)

class CapTouchSensorManager : public InputManager
{

public:
    enum input_position_t
    {
        LEFT,
        MID,
        RIGHT,
        MAX
    };

    struct input_t
    {
        uint8_t pin;
        uint8_t ids[input_position_t::MAX];
    };

    struct input_map_t
    {
        input_t inputs[CAP_NUM_SENSORS];
    };

private:
    struct pos_intput_t
    {
        uint8_t id;
        uint8_t trig;
    };

    struct cap_input_t
    {
        uint8_t pin;
        pos_intput_t pos[input_position_t::MAX];
        pin_irq_handler_t handler;
        CAP1203 sensor;
    };

public:
    CapTouchSensorManager() {}
    ~CapTouchSensorManager() {}

    void begin(input_map_t *map);
    virtual void getTriggers(trigger_list_t *triggers) override;
    virtual uint32_t getIRQCount() override { return irq_count; }

private:
    static cap_input_t inputs[CAP_NUM_SENSORS];
    static pin_irq_handler_t irq_handlers[CAP_NUM_SENSORS];
    static uint32_t irq_count;

    DEC_CAP_IRQ_HANDLER(0);
};
