#include <Arduino.h>

#include "BLEClientManager.hpp"

#ifdef BLE_BUILD_CLIENT

BLEClientManager::trig_bit_t BLEClientManager::export_triggers[EXPORT_NUM_SIGNALS];
BLEClientManager::trig_bit_t BLEClientManager::import_triggers[IMPORT_NUM_SIGNALS];

void BLEClientManager::begin(id_map_t *map)
{
    LOG("BLEClientManager::begin ...");

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

    WDT_ENABLE(BLE_CLIENT_WDT_TIMEOUT_SEC);

    LOGL("ok");
}

void BLEClientManager::setTriggers(trigger_list_t *triggers)
{
    unsigned long export_bits_set;
    unsigned long export_bits_clear;

    // client exports to server import and visa versa
    BLECharacteristic import_set_c = peripheral.characteristic(BLE_UUID_IMPORT_SET);
    BLECharacteristic import_clear_c = peripheral.characteristic(BLE_UUID_IMPORT_CLEAR);

    import_set_c.readValue(export_bits_set);
    import_clear_c.readValue(export_bits_clear);

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
    import_set_c.writeValue(export_bits_set);
}

void BLEClientManager::getTriggers(trigger_list_t *triggers)
{
    unsigned long import_bits_set;
    unsigned long import_bits_clear;

    // client exports to server import and visa versa
    BLECharacteristic import_set_c = peripheral.characteristic(BLE_UUID_EXPORT_SET);
    BLECharacteristic import_clear_c = peripheral.characteristic(BLE_UUID_EXPORT_CLEAR);

    import_set_c.readValue(import_bits_set);
    import_clear_c.readValue(import_bits_clear);

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

void BLEClientManager::update()
{
    static int wait_led_state = 0;
    static unsigned long led_t0 = 0;

#ifdef BLE_NO_SIGN_DROP_ENABLE
    static unsigned long sig_read_t0 = 0;
    static unsigned long sig_write_t0 = 0;
    unsigned long server_signature;
    BLECharacteristic c;
#endif

    if (!is_connected())
    {
        if (connect())
        {
            LOG("[CON][BLEClientManager] Name = ");
            LOGL(peripheral.localName());

            wait_led_state = 0;
            digitalWrite(LEDB, wait_led_state);

#ifdef BLE_NO_SIGN_DROP_ENABLE
            c = peripheral.characteristic(BLE_UUID_SERVER_SIGNATURE);
            c.writeValue((uint32_t)0);
            c = peripheral.characteristic(BLE_UUID_CLIENT_SIGNATURE);
            c.writeValue((uint32_t)BLE_CLIENT_SIGNATURE);
            sig_read_t0 = millis();
            sig_write_t0 = sig_read_t0;
#endif
        }
        else
        {
            if (millis() - led_t0 > BLE_BLINK_WAIT_MS)
            {
                wait_led_state = ~wait_led_state;
                led_t0 = millis();
                digitalWrite(LEDB, wait_led_state);
            }
        }
    }
#ifdef BLE_NO_SIGN_DROP_ENABLE
    else
    {
        c = peripheral.characteristic(BLE_UUID_SERVER_SIGNATURE);
        c.readValue(server_signature);

        if (server_signature != BLE_SERVER_SIGNATURE)
        {
            if (millis() - sig_read_t0 >= BLE_NO_SIGNATURE_DROP_TIME_MS)
            {
                LOG("[DROP][BLEClientManager] Name = ");
                LOG(peripheral.localName());
                LOG(" Server signature: 0x");
                LOGFL(server_signature, HEX);
                peripheral.disconnect();
                return;
            }
        }
        else
        {
            c.writeValue((uint32_t)0);
            sig_read_t0 = millis();
        }

        if (millis() - sig_write_t0 > BLE_SIGNATURE_WRITE_DELAY_MS)
        {
            WDT_RESET(); // reset WDT when writing signature
            BLECharacteristic c = peripheral.characteristic(BLE_UUID_CLIENT_SIGNATURE);
            c.writeValue((uint32_t)BLE_CLIENT_SIGNATURE);
            sig_write_t0 = millis();
        }
    }
#endif
}

bool BLEClientManager::is_connected()
{
    return peripheral.connected();
}

bool BLEClientManager::connect()
{
    unsigned long val;

    BLECharacteristic c;

    peripheral.disconnect();

    BLE.scanForUuid(BLE_UUID_SIGNATURE_SERVICE);

    delay(BLE_SCAN_WAIT_MS); 

    peripheral = BLE.available();

    if (!peripheral.connect())
    {
        LOGL("[DROP][BLEClientManager] Failed to connect");
        goto err;
    }

    if (!peripheral.localName().equals(BLE_SERVER_NAME))
    {
        LOGL("[DROP][BLEClientManager] Wrong server name");
        goto err;
    }

    if (!peripheral.discoverAttributes())
    {
        LOGL("[DROP][BLEClientManager] Failed to discover attributes");
        goto err;
    }

    c = peripheral.characteristic(BLE_UUID_SERVER_SIGNATURE);

    if (!c.canWrite() || !c.canRead())
    {
        LOGL("[DROP][BLEClientManager] No read and write permission for server signature");
        goto err;
    }

    c.readValue(val);
    LOG("SERVER SIGNATURE: ");
    LOGVL(val, HEX);

    if (!c.canWrite())
    {
        LOGL("[DROP][BLEClientManager] No write permission for client signature");
        goto err;
    }

    c = peripheral.characteristic(BLE_UUID_EXPORT_SET);

    if (!c.canRead())
    {
        LOGL("[DROP][BLEClientManager] no read permission for export set");
        goto err;
    }

    c.readValue(val);
    LOG("EXPORT SET: ");
    LOGVL(val, HEX);

    c = peripheral.characteristic(BLE_UUID_EXPORT_CLEAR);

    if (!c.canWrite() || !c.canRead())
    {
        LOGL("[DROP][BLEClientManager] no read and write permission for export clear");
        goto err;
    }

    c.readValue(val);
    LOG("EXPORT CLEAR: ");
    LOGVL(val, HEX);

    c = peripheral.characteristic(BLE_UUID_IMPORT_SET);

    if (!c.canWrite() || !c.canRead())
    {
        LOGL("[DROP][BLEClientManager] no read and write permission for import set");
        goto err;
    }

    c.readValue(val);
    LOG("IMPORT SET: ");
    LOGVL(val, HEX);

    c = peripheral.characteristic(BLE_UUID_IMPORT_CLEAR);

    if (!c.canRead())
    {
        LOGL("[DROP][BLEClientManager] no read permission for import clear");
        goto err;
    }

    c.readValue(val);
    LOG("IMPORT CLEAR: ");
    LOGVL(val, HEX);

    BLE.stopScan();

    return true;

err:
    peripheral.disconnect();
    BLINK_WAIT(peripheral.connected());
    BLE.stopScan();

    return false;
}

#endif
