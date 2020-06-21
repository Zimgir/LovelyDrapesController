#pragma once

#include <Arduino.h>
#include <ArduinoBLE.h>

#include "OutputManager.hpp"
#include "InputManager.hpp"

#ifdef BLE_BUILD_CLIENT

class BLEClientManager : public OutputManager, public InputManager
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
    BLEClientManager() {}
    ~BLEClientManager() {}

    static void begin(id_map_t *map);
    virtual void update() override;
    virtual void setTriggers(trigger_list_t *triggers) override;
    virtual void getTriggers(trigger_list_t *triggers) override;
    virtual uint32_t getIRQCount() override { return 0; };

private:
    bool connect();
    bool is_connected();
    bool update_export();
    bool update_import();

    BLEDevice peripheral;

    static trig_bit_t export_triggers[EXPORT_NUM_SIGNALS];
    static trig_bit_t import_triggers[IMPORT_NUM_SIGNALS];
};

#endif
