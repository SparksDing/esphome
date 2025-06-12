#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace awesome_servo {

struct JointStatus_t {
  float angleMin;
  float angleMax;
  float angle;
  float modelAngelMin;
  float modelAngelMax;
  bool inverted = false;
};

/// This class implements support for the i2c-based BH1750 ambient light sensor.
class AwesomeServo : public PollingComponent, public sensor::Sensor, public i2c::I2CDevice {
 public:
  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;

  void dump_config() override;

  void update() override;

  void setAngle(float value);

  float get_setup_priority() const override;

  void update_servo_angle(float _angleSetPoint);

  void update_joint_angle(float _angleSetPoint);

 protected:
 private:
  uint8_t _i2cRxData[8];
  uint8_t _i2cTxData[8];
  JointStatus_t _joint;

  float _target_angle = 0;
  uint32_t start_millis_ = 0;
  ESPPreferenceObject rtc_;
  bool restore_{false};
};

template<typename... Ts> class AwesomeServoWriteAction : public Action<Ts...> {
 public:
  AwesomeServoWriteAction(AwesomeServo *servo) : servo_(servo) {}
  TEMPLATABLE_VALUE(float, value)

  void play(Ts... x) override { this->servo_->setAngle(this->value_.value(x...)); }

 protected:
  AwesomeServo *servo_;
};

// template<typename... Ts> class AwesomeServoDetachAction : public Action<Ts...> {
//  public:
//   AwesomeServoDetachAction(Servo *servo) : servo_(servo) {}

//   void play(Ts... x) override { this->servo_->detach(); }

//  protected:
//   AwesomeServo *servo_;
// };

}  // namespace awesome_servo
}  // namespace esphome
