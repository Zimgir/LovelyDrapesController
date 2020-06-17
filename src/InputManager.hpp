#pragma once
#include <Arduino.h>

#include "system.hpp"
#include "utils.hpp"

class InputManager
{

public:
    virtual void getTriggers(trigger_list_t *triggers) = 0;
    virtual uint32_t getIRQCount() = 0;

protected:
    void addTrigger(trigger_list_t *triggers, uint8_t id)
    {
        if (triggers->size >= MAX_SIGNALS)
            BLINK_BUG(ERR_INDEX_OUT_OF_BOUNDS);

        triggers->ids[triggers->size++] = id;
    }
};
