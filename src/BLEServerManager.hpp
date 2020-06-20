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
    struct trig_bit_t
    {
        uint8_t id;
        uint8_t trig;
        uint8_t bit;
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

    static void reset();
    
    static BLEService signature_service;
    static BLEUnsignedLongCharacteristic client_signature_c;
    static BLEUnsignedLongCharacteristic server_signature_c;

    static BLEService export_service;
    static BLEUnsignedLongCharacteristic export_set_c;
    static BLEUnsignedLongCharacteristic export_clear_c;

    static BLEService import_service;
    static BLEUnsignedLongCharacteristic import_set_c;
    static BLEUnsignedLongCharacteristic import_clear_c;

    static trig_bit_t export_triggers[EXPORT_NUM_SIGNALS];
    static trig_bit_t import_triggers[IMPORT_NUM_SIGNALS];
};
