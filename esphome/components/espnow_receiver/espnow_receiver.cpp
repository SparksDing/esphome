#include "espnow_receiver.h"
#include "esphome/core/log.h"

namespace esphome {
namespace espnow_receiver {

static const char *const TAG = "espnow_receiver";

// Static callback function wrapper
static void IRAM_ATTR on_data_recv_static(const uint8_t *mac, const uint8_t *incoming_data, int len) {
  if (instance != nullptr) {
    instance->on_data_recv(mac, incoming_data, len);
  }
}

void EspnowReceiver::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ESP-NOW Receiver...");
  instance = this; // Set the static instance pointer

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Set wifi channel to 13
  esp_wifi_set_channel(13, WIFI_SECOND_CHAN_NONE);
  ESP_LOGCONFIG(TAG, "WiFi channel set to 13");

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    ESP_LOGE(TAG, "Error initializing ESP-NOW");
    this->mark_failed();
    return;
  }

  // Register the static callback function
  if (esp_now_register_recv_cb(on_data_recv_static) != ESP_OK) {
      ESP_LOGE(TAG, "Failed to register ESP-NOW receive callback");
      this->mark_failed();
      return;
  }

  ESP_LOGCONFIG(TAG, "ESP-NOW Receiver Initialized.");

  // Print MAC address for debugging
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  ESP_LOGCONFIG(TAG, "MAC Address: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void EspnowReceiver::loop() {
  // Check pointer validity before dereferencing and call value() method
  if (this->valid_global_ != nullptr && this->valid_global_->value() && (millis() - this->last_data_received_ > this->data_timeout_)) {
    ESP_LOGW(TAG, "ESP-NOW data timed out.");
    this->valid_global_->value() = false; // Update the global variable by assigning to value()
    // No need to trigger display update here, the display lambda should check the valid flag
  }
}

// Member function to handle received data
void EspnowReceiver::on_data_recv(const uint8_t *mac, const uint8_t *incoming_data, int len) {
  if (len != sizeof(SensorData)) {
    ESP_LOGW(TAG, "Received packet with wrong size: %d, expected %d", len, sizeof(SensorData));
    return;
  }

  SensorData received_data;
  memcpy(&received_data, incoming_data, sizeof(received_data));

  // Check if sender marked data as valid
  if (!received_data.valid) {
     ESP_LOGD(TAG, "Received data marked as invalid by sender.");
     // Check pointer validity before dereferencing and call value() method
     if (this->valid_global_ != nullptr && this->valid_global_->value()) {
         this->valid_global_->value() = false; // Update global if it was previously true
     }
     // Even if sender says invalid, reset our timeout timer because we got *something*
     this->last_data_received_ = millis(); 
     return; // Don't update sensor values
  }

  ESP_LOGD(TAG, "Received valid data from %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Update global variables only if pointers are valid, by assigning to value()
  if (this->pm1_0_global_) this->pm1_0_global_->value() = received_data.pm1_0;
  if (this->pm2_5_global_) this->pm2_5_global_->value() = received_data.pm2_5;
  if (this->pm10_global_) this->pm10_global_->value() = received_data.pm10;
  if (this->co2_global_) this->co2_global_->value() = received_data.co2;
  if (this->voc_global_) this->voc_global_->value() = received_data.voc;
  if (this->temp_global_) this->temp_global_->value() = received_data.temperature;
  if (this->hum_global_) this->hum_global_->value() = received_data.humidity;
  if (this->ch2o_global_) this->ch2o_global_->value() = received_data.ch2o;
  if (this->co_global_) this->co_global_->value() = received_data.co;
  if (this->o3_global_) this->o3_global_->value() = received_data.o3;
  if (this->no2_global_) this->no2_global_->value() = received_data.no2;
  
  // Update validity flag and timestamp
  if (this->valid_global_) this->valid_global_->value() = true;
  this->last_data_received_ = millis();

  // Note: We don't explicitly trigger a display update here.
  // The display component's update_interval will handle redraws,
  // and its lambda should check the 'valid_global_' flag.
}

} // namespace espnow_receiver
} // namespace esphome 