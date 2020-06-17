#pragma once
#include <Arduino.h>

#include "system.hpp"
#include "utils.hpp"

class OutputManager
{

public:
    virtual void setTriggers(trigger_list_t *triggers) = 0;
    virtual void update(void) = 0;

};