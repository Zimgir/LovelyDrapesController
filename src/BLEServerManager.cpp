#include <Arduino.h>

#include "BLEServerManager.hpp"
#ifdef BLE_BUILD_SERVER

BLEService BLEServerManager::signature_service(BLE_UUID_SIGNATURE_SERVICE);

BLEUnsignedLongCharacteristic BLEServerManager::client_signature_c(BLE_UUID_CLIENT_SIGNATURE, BLEWrite);
BLEUnsignedLongCharacteristic BLEServerManager::server_signature_c(BLE_UUID_SERVER_SIGNATURE, BLERead | BLEWrite);

BLEService BLEServerManager::export_service(BLE_UUID_EXPORT_SERVICE);

BLEUnsignedLongCharacteristic BLEServerManager::export_set_c(BLE_UUID_EXPORT_SET, BLERead);
BLEUnsignedLongCharacteristic BLEServerManager::export_clear_c(BLE_UUID_EXPORT_CLEAR, BLERead | BLEWrite);

BLEService BLEServerManager::import_service(BLE_UUID_IMPORT_SERVICE);

BLEUnsignedLongCharacteristic BLEServerManager::import_set_c(BLE_UUID_IMPORT_SET, BLERead | BLEWrite);
BLEUnsignedLongCharacteristic BLEServerManager::import_clear_c(BLE_UUID_IMPORT_CLEAR, BLERead);

BLEServerManager::trig_bit_t BLEServerManager::export_triggers[EXPORT_NUM_SIGNALS];
BLEServerManager::trig_bit_t BLEServerManager::import_triggers[IMPORT_NUM_SIGNALS];

void BLEServerManager::begin(id_map_t *map)
{
    LOG("BLEServerManager::begin ...");

    if (!BLE.begin())
        BLINK_BUG(ERR_BLE_BEGIN);

    for (int i = 0; i < EXPORT_NUM_SIGNALS; ++i)
    {
        export_triggers[i].id = map->exports[i];
        export_triggers[i].trig = 0;
        export_triggers[i].bit = i;
    }

    for (int i = 0; i < IMPORT_NUM_SIGNALS; ++i)
    {
        import_triggers[i].id = map->imports[i];
        import_triggers[i].trig = 0;
        import_triggers[i].bit = i;
    }

    export_set_c.writeValue(0xC0DE0000);
    export_clear_c.writeValue(0xB0DE0000);

    import_set_c.writeValue(0xC0FE0000);
    import_clear_c.writeValue(0xD0FE0000);

    export_service.addCharacteristic(export_set_c);
    export_service.addCharacteristic(export_clear_c);

    import_service.addCharacteristic(import_set_c);
    import_service.addCharacteristic(import_clear_c);

    client_signature_c.writeValue(0);
    server_signature_c.writeValue(BLE_SERVER_SIGNATURE);

    signature_service.addCharacteristic(client_signature_c);
    signature_service.addCharacteristic(server_signature_c);

    BLE.addService(signature_service);
    BLE.addService(export_service);
    BLE.addService(import_service);

    BLE.setAdvertisedService(signature_service);

    BLE.setLocalName(BLE_SERVER_NAME);

    BLE.advertise();

    LOGL("ok");
}

void BLEServerManager::setTriggers(trigger_list_t *triggers)
{
    unsigned long export_bits_set = export_set_c.value();
    unsigned long export_bits_clear = export_clear_c.value();

    // first of all clear all previous triggers
    for (int i = 0; i < EXPORT_NUM_SIGNALS; ++i)
    {
        export_triggers[i].trig = 0;
    }

    // check for any new triggers and update export bitmask
    for (int trig_idx = 0; trig_idx < triggers->size; ++trig_idx)
    {
        for (int exp_idx = 0; exp_idx < EXPORT_NUM_SIGNALS; ++exp_idx)
        {
            if (triggers->ids[trig_idx] == export_triggers[exp_idx].id)
            {
#ifdef LOG_ENABLE
                if (getBit(&export_bits_set, export_triggers[exp_idx].bit) == 0)
                {
                    LOG("[TRIG][BLEClientManager] id = ");
                    LOG(export_triggers[exp_idx].id);
                    LOGV(export_bits_set, HEX);
                    LOGVL(export_bits_clear, HEX);
                }
#endif
                export_triggers[exp_idx].trig = 1;
                setBit(&export_bits_set, export_triggers[exp_idx].bit);
            }
        }
    }

    // clear bits that were requested to be cleared if not triggered
    for (int i = 0; i < EXPORT_NUM_SIGNALS; ++i)
    {
        if (export_triggers[i].trig)
            continue;

        if (getBit(&export_bits_clear, export_triggers[i].bit))
        {
            clearBit(&export_bits_set, export_triggers[i].bit);
        }
    }

    // set the export bitmask
    export_set_c.writeValue(export_bits_set);
}
void BLEServerManager::getTriggers(trigger_list_t *triggers)
{
    unsigned long import_bits_set = import_set_c.value();
    unsigned long import_bits_clear = import_clear_c.value();

    for (int i = 0; i < IMPORT_NUM_SIGNALS; ++i)
    {
        import_triggers[i].trig = getBit(&import_bits_set, import_triggers[i].bit);

        if (import_triggers[i].trig)
        {
            InputManager::addTrigger(triggers, import_triggers[i].id);

            //clear the import by setting the clear bit
            setBit(&import_bits_clear, import_triggers[i].bit);
        }
        else
        {
            // clear the clear bitmask to stop clearing the import
            clearBit(&import_bits_clear, import_triggers[i].bit);
        }
    }

    // clear the import bitmask
    import_clear_c.writeValue(import_bits_clear);
}

void BLEServerManager::update()
{
    static int wait_led_state = 0;
    static unsigned long led_t0 = 0;

#ifdef BLE_NO_SIGN_DROP_ENABLE
    static unsigned long sig_read_t0 = 0;
    static unsigned long sig_write_t0 = 0;
#endif

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

            wait_led_state = 0;
            digitalWrite(LEDB, wait_led_state);

#ifdef BLE_NO_SIGN_DROP_ENABLE
            client_signature_c.writeValue(0);
            server_signature_c.writeValue(BLE_SERVER_SIGNATURE);
            sig_read_t0 = millis();
            sig_write_t0 = sig_read_t0;
#endif
        }
#ifdef BLE_NO_SIGN_DROP_ENABLE
        else
        {
            unsigned long client_signature = client_signature_c.value();

            if (client_signature != BLE_CLIENT_SIGNATURE)
            {
                if (millis() - sig_read_t0 >= BLE_NO_SIGNATURE_DROP_TIME_MS)
                {
                    LOG("[DROP][BLEServerManager] MAC = ");
                    LOG(connected_mac);
                    LOG(" Client signature: 0x");
                    LOGFL(client_signature, HEX);
                    saved_connected_mac = "";
                    central.disconnect();

                    // reset 3 sec after dropping connection because client can cause a bad disconnet bug
                    digitalWrite(LEDB, HIGH); // turn off connetion LED
                    WDT_ENABLE(3);
                    BLINK_WAIT(1);
                }
            }
            else
            {
                client_signature_c.writeValue(0);
                sig_read_t0 = millis();
            }

            if (millis() - sig_write_t0 > BLE_SIGNATURE_WRITE_DELAY_MS)
            {
                server_signature_c.writeValue(BLE_SERVER_SIGNATURE);
                sig_write_t0 = millis();
            }
        }
#endif
    }
    else
    {
        saved_connected_mac = "";
        if (millis() - led_t0 > BLE_BLINK_WAIT_MS)
        {
            wait_led_state = ~wait_led_state;
            led_t0 = millis();
            digitalWrite(LEDB, wait_led_state);
        }
    }
}

#endif