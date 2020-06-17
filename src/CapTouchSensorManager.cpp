#include <Arduino.h>
#include <Wire.h>

#include "CapTouchSensorManager.hpp"

CapTouchSensorManager::cap_input_t CapTouchSensorManager::inputs[CAP_NUM_SENSORS];
uint32_t CapTouchSensorManager::irq_count = 0;

#define CAP_INTERRUPT_DISABLE

#define DEF_CAP_IRQ_HANDLER(ID)                               \
    void CapTouchSensorManager::SYM_CAP_IRQ_HANDLER##ID(void) \
    {                                                         \
        irq_count++;                                          \
    }

DEF_CAP_IRQ_HANDLER(0);

pin_irq_handler_t CapTouchSensorManager::irq_handlers[CAP_NUM_SENSORS] = {
    CAP_IRQ_HANDLER(0)};

void CapTouchSensorManager::begin(input_map_t *map)
{
    LOG("CapTouchSensorManager::begin ...");

    Wire.begin(); // Join I2C bus

    for (int i = 0; i < CAP_NUM_SENSORS; ++i)
    {
        inputs[i].pin = map->inputs[i].pin;
        inputs[i].handler = irq_handlers[i];

        for (int j = 0; j < input_position_t::MAX; ++j)
        {
            inputs[i].pos[j].id = map->inputs[i].ids[j];
            inputs[i].pos[j].trig = 0;
        }

        if (!inputs[i].sensor.begin())
            BLINK_BUG(ERR_CAP_SENSOR_BEGIN);

        inputs[i].sensor.setSensitivity(SENSITIVITY_64X);

#ifdef CAP_INTERRUPT_DISABLE
        inputs[i].sensor.setInterruptDisabled();
#else
        pinMode(inputs[i].pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(inputs[i].pin), inputs[i].handler, FALLING);
#endif
    }

    LOGL("ok");
}

void CapTouchSensorManager::getTriggers(trigger_list_t *triggers)
{

    for (int i = 0; i < CAP_NUM_SENSORS; ++i)
    {
        if (inputs[i].sensor.isLeftTouched())
            InputManager::addTrigger(triggers, inputs[i].pos[LEFT].id);

        if (inputs[i].sensor.isMiddleTouched())
            InputManager::addTrigger(triggers, inputs[i].pos[MID].id);

        if (inputs[i].sensor.isRightTouched())
            InputManager::addTrigger(triggers, inputs[i].pos[RIGHT].id);
    }
}