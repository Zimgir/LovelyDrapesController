#include <Arduino.h>

#include "BLEServerManager.hpp"

BLEService BLEServerManager::signature_service(BLE_UUID_SIGNATURE_SERVICE);

BLEUnsignedLongCharacteristic BLEServerManager::signature_char(BLE_UUID_SIGNATURE_CHAR, BLEWrite);

BLEService BLEServerManager::export_service(BLE_UUID_IMPORT_SERVICE);

BLEServerManager::characteristic_t BLEServerManager::export_chars[EXPORT_NUM_SIGNALS] = {
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_EXPORT_CHAR_0, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_EXPORT_CHAR_1, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_EXPORT_CHAR_2, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_EXPORT_CHAR_3, BLERead | BLEWrite)}

};

BLEService BLEServerManager::import_service(BLE_UUID_EXPORT_SERVICE);

BLEServerManager::characteristic_t BLEServerManager::import_chars[IMPORT_NUM_SIGNALS] = {
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_IMPORT_CHAR_0, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_IMPORT_CHAR_1, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_IMPORT_CHAR_2, BLERead | BLEWrite)},
    {.id = 0xFF, .trig = 0, .c = BLEByteCharacteristic(BLE_UUID_IMPORT_CHAR_3, BLERead | BLEWrite)}

};

void BLEServerManager::begin(id_map_t *map)
{
    LOG("BLEServerManager::begin ...");

    if (!BLE.begin())
        BLINK_BUG(ERR_BLE_BEGIN);

    for (int i = 0; i < EXPORT_NUM_SIGNALS; ++i)
    {
        export_chars[i].id = map->exports[i];
        export_chars[i].c.writeValue(0);
        export_service.addCharacteristic(export_chars[i].c);
    }

    for (int i = 0; i < IMPORT_NUM_SIGNALS; ++i)
    {
        import_chars[i].id = map->imports[i];
        import_chars[i].c.writeValue(0);
        import_service.addCharacteristic(import_chars[i].c);
    }

    signature_char.writeValue(0);
    signature_service.addCharacteristic(signature_char);

    BLE.addService(signature_service);
    BLE.addService(export_service);
    BLE.addService(import_service);

    BLE.setAdvertisedService(export_service);
    BLE.setAdvertisedService(import_service);

    BLE.setLocalName(BLE_SERVER_NAME);

    BLE.advertise();

    LOGL("ok");
}

void BLEServerManager::setTriggers(trigger_list_t *triggers)
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
void BLEServerManager::getTriggers(trigger_list_t *triggers)
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

void BLEServerManager::update()
{
    static unsigned long connect_t0 = 0;
    static String saved_connected_mac;

    BLEDevice central = BLE.central();

    if (central.connected())
    {
        String connected_mac = central.address();

        if (connected_mac != saved_connected_mac)
        {
            saved_connected_mac = connected_mac;

            LOG("[CON][BLEServerManager] MAC = ");
            LOGL(connected_mac);
      
#ifdef BLE_NO_SIGN_DROP_ENABLE
            signature_char.writeValue(0);    
            connect_t0 = millis();
#endif

        }
#ifdef BLE_NO_SIGN_DROP_ENABLE
        else
        {
            unsigned long signature = signature_char.valueBE();

            if ((signature != BLE_CLIENT_SIGNATURE) && (millis() - connect_t0 >= BLE_NO_SIGNATURE_DROP_TIME_MS))
            {
                LOG("[DROP][BLEServerManager] MAC = ");
                LOG(connected_mac);
                LOG(" Signature: 0x");
                LOGX(signature);
                LOGL("");
                saved_connected_mac = "";
                central.disconnect();
            }
        }
#endif
    }
    else
    {
        saved_connected_mac = "";
    }
}
