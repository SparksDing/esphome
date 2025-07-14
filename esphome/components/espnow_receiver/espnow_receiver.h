#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/globals/globals_component.h"
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

namespace esphome {
namespace espnow_receiver {

// Structure matching the sender's data
struct SensorData {
  int pm1_0;
  int pm2_5;
  int pm10;
  int co2;
  int voc;
  float temperature;
  int humidity;
  float ch2o;
  float co;
  float o3;
  float no2;
  bool valid; // Sender indicates if its readings are valid
}; // Ensure struct packing matches sender


// Forward declaration
class EspnowReceiver;

// Static instance pointer to access component members from static callback
static EspnowReceiver *instance = nullptr;

// Static ESP-NOW receive callback
static void IRAM_ATTR on_data_recv_static(const uint8_t *mac, const uint8_t *incoming_data, int len);

class EspnowReceiver : public Component {
 public:
  // Pointers to the global variables defined in YAML (using GlobalsComponent<T>*)
  globals::GlobalsComponent<int> *pm1_0_global_{nullptr};
  globals::GlobalsComponent<int> *pm2_5_global_{nullptr};
  globals::GlobalsComponent<int> *pm10_global_{nullptr};
  globals::GlobalsComponent<int> *co2_global_{nullptr};
  globals::GlobalsComponent<int> *voc_global_{nullptr};
  globals::GlobalsComponent<float> *temp_global_{nullptr};
  globals::GlobalsComponent<int> *hum_global_{nullptr};
  globals::GlobalsComponent<float> *ch2o_global_{nullptr};
  globals::GlobalsComponent<float> *co_global_{nullptr};
  globals::GlobalsComponent<float> *o3_global_{nullptr};
  globals::GlobalsComponent<float> *no2_global_{nullptr};
  globals::GlobalsComponent<bool> *valid_global_{nullptr};

  // Setters called by generated code (using GlobalsComponent<T>*)
  void set_pm1_0_global(globals::GlobalsComponent<int> *ptr) { this->pm1_0_global_ = ptr; }
  void set_pm2_5_global(globals::GlobalsComponent<int> *ptr) { this->pm2_5_global_ = ptr; }
  void set_pm10_global(globals::GlobalsComponent<int> *ptr) { this->pm10_global_ = ptr; }
  void set_co2_global(globals::GlobalsComponent<int> *ptr) { this->co2_global_ = ptr; }
  void set_voc_global(globals::GlobalsComponent<int> *ptr) { this->voc_global_ = ptr; }
  void set_temp_global(globals::GlobalsComponent<float> *ptr) { this->temp_global_ = ptr; }
  void set_hum_global(globals::GlobalsComponent<int> *ptr) { this->hum_global_ = ptr; }
  void set_ch2o_global(globals::GlobalsComponent<float> *ptr) { this->ch2o_global_ = ptr; }
  void set_co_global(globals::GlobalsComponent<float> *ptr) { this->co_global_ = ptr; }
  void set_o3_global(globals::GlobalsComponent<float> *ptr) { this->o3_global_ = ptr; }
  void set_no2_global(globals::GlobalsComponent<float> *ptr) { this->no2_global_ = ptr; }
  void set_valid_global(globals::GlobalsComponent<bool> *ptr) { this->valid_global_ = ptr; }

  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void on_data_recv(const uint8_t *mac, const uint8_t *incoming_data, int len);

 protected:
  unsigned long last_data_received_ = 0;
  const unsigned long data_timeout_ = 10000; // 10 seconds timeout
};

} // namespace espnow_receiver
} // namespace esphome 