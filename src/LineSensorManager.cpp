#include <Arduino.h>
#include "LineSensorManager.hpp"

LineSensorManager::line_input_t LineSensorManager::inputs[LINE_NUM_SENSORS];
uint32_t LineSensorManager::irq_count = 0;

#define DEF_LINE_IRQ_HANDLER(ID)                             \
    void LineSensorManager::SYM_LINE_IRQ_HANDLER##ID(void) \
    {                                                        \
        inputs[ID].trig = 1;                                 \
        irq_count++;                                         \
    }

DEF_LINE_IRQ_HANDLER(0);
//DEF_LINE_IRQ_HANDLER(1);
//DEF_LINE_IRQ_HANDLER(2);

pin_irq_handler_t LineSensorManager::irq_handlers[LINE_NUM_SENSORS] = {
    LINE_IRQ_HANDLER(0),
    /*LINE_IRQ_HANDLER(1),*/
    /*LINE_IRQ_HANDLER(2)*/};

void LineSensorManager::begin(input_map_t *map)
{
    LOG("LineSensorManager::begin ...");

    for (int i = 0; i < LINE_NUM_SENSORS; ++i)
    {
        inputs[i].id = map->inputs[i].id;
        inputs[i].pin = map->inputs[i].pin;
        inputs[i].handler = irq_handlers[i];
        inputs[i].trig = 0;

        pinMode(inputs[i].pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(inputs[i].pin), inputs[i].handler, CHANGE);
    }

    LOGL("ok");
}

void LineSensorManager::getTriggers(trigger_list_t *triggers)
{

    for (int i = 0; i < LINE_NUM_SENSORS; ++i)
    {
        if (inputs[i].trig)
        {
            InputManager::addTrigger(triggers, inputs[i].id);
            inputs[i].trig = 0;
        }
    }
}