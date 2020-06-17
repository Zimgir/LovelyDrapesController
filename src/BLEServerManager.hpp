#pragma once
#include <Arduino.h>
#include <ArduinoBLE.h>

#include "OutputManager.hpp"
#include "InputManager.hpp"

class BLEServerManager : public OutputManager, public InputManager
{

public:
    struct id_map_t
    {
        uint8_t exports[EXPORT_NUM_SIGNALS];
        uint8_t imports[IMPORT_NUM_SIGNALS];
    };

private:
    struct characteristic_t
    {
        uint8_t id;
        uint8_t trig;
        BLEByteCharacteristic c;
    };

public:
    BLEServerManager() {}
    ~BLEServerManager() {}

    static void begin(id_map_t *map);
    virtual void update() override;
    virtual void setTriggers(trigger_list_t *triggers) override;
    virtual void getTriggers(trigger_list_t *triggers) override;
    virtual uint32_t getIRQCount() override { return 0; };

private:
    static BLEService signature_service;
    static BLEUnsignedLongCharacteristic signature_char;

    static BLEService export_service;
    static characteristic_t export_chars[EXPORT_NUM_SIGNALS];

    static BLEService import_service;
    static characteristic_t import_chars[IMPORT_NUM_SIGNALS];
};
