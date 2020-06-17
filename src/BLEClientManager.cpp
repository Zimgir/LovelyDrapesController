#include <Arduino.h>

#include "BLEClientManager.hpp"

BLEService BLEClientManager::export_service(BLE_UUID_IMPORT_SERVICE);
BLEService BLEClientManager::import_service(BLE_UUID_EXPORT_SERVICE);

BLEClientManager::characteristic_t BLEClientManager::export_chars[EXPORT_NUM_SIGNALS] = {
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_EXPORT_CHAR_0, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_EXPORT_CHAR_1, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_EXPORT_CHAR_2, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_EXPORT_CHAR_3, BLERead | BLEWrite)}

};

BLEClientManager::characteristic_t BLEClientManager::import_chars[IMPORT_NUM_SIGNALS] = {
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_IMPORT_CHAR_0, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_IMPORT_CHAR_1, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_IMPORT_CHAR_2, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_IMPORT_CHAR_3, BLERead | BLEWrite)}

};

void BLEClientManager::begin(id_map_t *map)
{
    LOG("BLEServerManager::begin ...");

    if (!BLE.begin())
        BLINK_BUG(ERR_BLE_BEGIN);

    for (int i = 0; i < EXPORT_NUM_SIGNALS; ++i)
    {
        export_chars[i].id = map->exports[i];
        export_chars[i].c.writeValue(0);
        export_chars[i].c.writeValue(0);
        export_service.addCharacteristic(export_chars[i].c);
    }

    for (int i = 0; i < IMPORT_NUM_SIGNALS; ++i)
    {
        import_chars[i].id = map->imports[i];
        import_chars[i].c.writeValue(0);
        import_service.addCharacteristic(import_chars[i].c);
    }

    BLE.setLocalName(BLE_SERVER_NAME);

    BLE.addService(export_service);
    BLE.addService(import_service);

    BLE.setAdvertisedService(export_service);
    BLE.setAdvertisedService(import_service);

    BLE.advertise();

    LOGL("ok");
}

void BLEClientManager::setTriggers(trigger_list_t *triggers)
{

    for (int trig_idx = 0; trig_idx < triggers->size; ++trig_idx)
    {
        for (int sig_idx = 0; sig_idx < EXPORT_NUM_SIGNALS; ++sig_idx)
        {
            if (triggers->ids[trig_idx] == export_chars[sig_idx].id)
            {
#ifdef LOG_ENABLE
                export_chars[sig_idx].trig = export_chars[sig_idx].c.value();
                
                if (export_chars[sig_idx].trig == 0)
                {
                    LOG("[TRIG][BLEServerManager] id = ");
                    LOGL(export_chars[sig_idx].id);
                }
#endif
                export_chars[sig_idx].trig = 1;
                export_chars[sig_idx].c.writeValue(1);
            }
        }
    }
}
void BLEClientManager::getTriggers(trigger_list_t *triggers)
{
    for (int i = 0; i < IMPORT_NUM_SIGNALS; ++i)
    {
        import_chars[i].trig = import_chars[i].c.value();

        if (import_chars[i].trig)
        {
            InputManager::addTrigger(triggers, import_chars[i].id);
            import_chars[i].c.writeValue(0); // clear trigger in characteristic
        }
    }
}

void BLEClientManager::update()
{
    static BLEDevice central = BLE.central();
    static int connect_acknowledged = 0;

    if (central.connected())
    {
        if (!connect_acknowledged)
        {
            LOG("[CON][BLEServerManager] MAC = ");
            LOGL(central.address());
            connect_acknowledged = 1;
        }
    }
    else
    {
        connect_acknowledged = 0;
    }
}
