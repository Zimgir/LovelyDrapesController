#include <Arduino.h>

#include "LineSensorManager.hpp"
#include "CapTouchSensorManager.hpp"
#include "LedManager.hpp"

#if defined(BLE_BUILD_SERVER)
#include "BLEServerManager.hpp"
#define BLEManager BLEServerManager
#elif defined(BLE_BUILD_CLIENT)
#include "BLEClientManager.hpp"
#define BLEManager BLEClientManager
#else
#error "BLE manager not defined!"
#endif

LineSensorManager::input_map_t line_input_map = {.inputs = {
                                                     {.id = 0, .pin = A7},
                                                     {.id = 1, .pin = 10},
                                                     {.id = 2, .pin = A1}}};

CapTouchSensorManager::input_map_t cap_input_map = {.inputs = {
                                                        {.pin = 11,
                                                         .ids = {3, 4, 5}}}};

LedManager::output_map_t led_output_map = {.outputs = {
                                               {.id = 0, .pin = 2},
                                               {.id = 1, .pin = 3},
                                               {.id = 2, .pin = 4},
                                               {.id = 3, .pin = 5},
                                               {.id = 4, .pin = 6},
                                               {.id = 5, .pin = 12}}};

BLEManager::id_map_t ble_id_map = {.exports = {0, 3, 4, 5},
                                   .imports = {0, 3, 4, 5}};

BLEManager ble_manager;
LineSensorManager line_sensor_manager;
CapTouchSensorManager cap_touch_sensor_manager;
LedManager led_manager;

trigger_list_t export_triggers;
trigger_list_t import_triggers;

void setup()
{
  BLINK_RGB(3, 50);
  LOG_BEGIN();

  ble_manager.begin(&ble_id_map);

#ifdef BLE_BUILD_SERVER
  line_sensor_manager.begin(&line_input_map);
  cap_touch_sensor_manager.begin(&cap_input_map);
  led_manager.begin(&led_output_map);
#endif

  LOGL("setup: ok");
}

void loop()
{
  export_triggers.size = 0;
  import_triggers.size = 0;

#ifdef BLE_BUILD_SERVER
  line_sensor_manager.getTriggers(&export_triggers);
  cap_touch_sensor_manager.getTriggers(&export_triggers);
#endif

  ble_manager.getTriggers(&import_triggers);

#ifdef BLE_BUILD_SERVER
  ble_manager.setTriggers(&export_triggers);
#else
  ble_manager.setTriggers(&import_triggers);
#endif

#ifdef BLE_BUILD_SERVER
#ifdef LOOPBACK_ENABLE
  led_manager.setTriggers(&export_triggers);
#else
  led_manager.setTriggers(&import_triggers);
#endif
  led_manager.update();
#endif

  ble_manager.update();

#if defined(LOG_ENABLE) && defined(LOG_DELAY_MAIN)
  delay(500);
#endif
}
