#include "awesome_servo.h"
#include "esphome/core/log.h"

namespace esphome {
namespace awesome_servo {

static const char *const TAG = "awesome_servo.sensor";

uint32_t global_servo_id = 1911044085ULL;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void AwesomeServo::setup() {
  ESP_LOGCONFIG(TAG, "Setting up AwesomeServo '%s'...", this->name_.c_str());

  _i2cTxData[0] = 0xff;
  _i2cTxData[1] = 1;

  if (this->write(_i2cTxData, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Starting measurement for BH1750 failed");
    return;
  }

  float v;
  if (this->restore_) {
    this->rtc_ = global_preferences->make_preference<float>(global_servo_id);

    if (this->rtc_.load(&v)) {
      this->_target_angle = v;
      this->update_joint_angle(v);
      this->start_millis_ = millis();
      return;
    }
  }

  this->set_timeout("read", 1000, [this]() {
    if (this->read(reinterpret_cast<uint8_t *>(_i2cRxData), 2) != i2c::ERROR_OK) {
      ESP_LOGW(TAG, "Reading awesome servo angle data failed");
      return;
    }

    _joint.angle = *(float *) (_i2cRxData + 1);
  });
}

void AwesomeServo::update() { update_servo_angle(_target_angle); }

void AwesomeServo::update_servo_angle(float _angleSetPoint) {
  if (_angleSetPoint >= _joint.angleMin && _angleSetPoint <= _joint.angleMax) {
    auto *b = (unsigned char *) (&_angleSetPoint);

    _i2cTxData[0] = 0x01;
    for (int i = 0; i < 4; i++)
      _i2cTxData[i + 1] = *(b + i);

    if (this->write(_i2cTxData, 1) != i2c::ERROR_OK) {
      ESP_LOGW(TAG, "Starting measurement for BH1750 failed");
      return;
    }

    this->set_timeout("read", 1000, [this]() {
      if (this->read(reinterpret_cast<uint8_t *>(_i2cRxData), 2) != i2c::ERROR_OK) {
        ESP_LOGW(TAG, "Reading awesome servo angle data failed");
        return;
      }

      _joint.angle = *(float *) (_i2cRxData + 1);
    });
  }
}

void AwesomeServo::update_joint_angle(float _angleSetPoint) {
  float sAngle = _joint.inverted
                     ? (_angleSetPoint - _joint.modelAngelMin) / (_joint.modelAngelMax - _joint.modelAngelMin) *
                               (_joint.angleMin - _joint.angleMax) +
                           _joint.angleMax
                     : (_angleSetPoint - _joint.modelAngelMin) / (_joint.modelAngelMax - _joint.modelAngelMin) *
                               (_joint.angleMax - _joint.angleMin) +
                           _joint.angleMin;

  this->update_servo_angle(sAngle);

  float jAngle = _joint.inverted ? (_joint.angleMax - _joint.angle) / (_joint.angleMax - _joint.angleMin) *
                                           (_joint.modelAngelMax - _joint.modelAngelMin) +
                                       _joint.modelAngelMin
                                 : (_joint.angle - _joint.angleMin) / (_joint.angleMax - _joint.angleMin) *
                                           (_joint.modelAngelMax - _joint.modelAngelMin) +
                                       _joint.modelAngelMin;

  _joint.angle = jAngle;

  set_accuracy_decimals(jAngle);
}

void AwesomeServo::setAngle(float value) {
  value = clamp(value, -1.0f, 1.0f);
  this->_target_angle = value;
  ESP_LOGD(TAG, "Servo new target: %f", value);
}

float AwesomeServo::get_setup_priority() const { return setup_priority::DATA; }

void AwesomeServo::dump_config() {
  LOG_SENSOR("", "AwesomeServo", this);
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL_FOR, this->get_name().c_str());
  }

  LOG_UPDATE_INTERVAL(this);
}

}  // namespace awesome_servo
}  // namespace esphome