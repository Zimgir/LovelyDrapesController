#pragma once
#include <Arduino.h>

#include "InputManager.hpp"

#define SYM_LINE_IRQ_HANDLER __line_irq_handler_
#define LINE_IRQ_HANDLER(ID) SYM_LINE_IRQ_HANDLER##ID
#define DEC_LINE_IRQ_HANDLER(ID) static void LINE_IRQ_HANDLER(ID)(void)

class LineSensorManager : public InputManager
{

public:
    struct input_t
    {
        uint8_t id;
        uint8_t pin;
    };

    struct input_map_t
    {
        input_t inputs[LINE_NUM_SENSORS];
    };

private:
    struct line_input_t : public input_t
    {
        volatile uint8_t trig;
        pin_irq_handler_t handler;
    };

public:
    LineSensorManager() {}
    ~LineSensorManager() {}

    static void begin(input_map_t *map);
    virtual void getTriggers(trigger_list_t *triggers) override;
    virtual uint32_t getIRQCount() override { return irq_count; }

private:
    static line_input_t inputs[LINE_NUM_SENSORS];
    static pin_irq_handler_t irq_handlers[LINE_NUM_SENSORS];
    static uint32_t irq_count;

    DEC_LINE_IRQ_HANDLER(0);
    DEC_LINE_IRQ_HANDLER(1);
    DEC_LINE_IRQ_HANDLER(2);
};