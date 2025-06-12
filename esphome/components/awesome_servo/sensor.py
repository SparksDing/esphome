from esphome import automation
import esphome.codegen as cg
from esphome.components import i2c, sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_LEVEL,
    DEVICE_CLASS_ILLUMINANCE,
    STATE_CLASS_MEASUREMENT,
    UNIT_LUX,
)

awesome_servo_ns = cg.esphome_ns.namespace("awesome_servo")
AwesomeServo = awesome_servo_ns.class_(
    "AwesomeServo", cg.PollingComponent, sensor.Sensor, i2c.I2CDevice
)
AwesomeServoWriteAction = awesome_servo_ns.class_(
    "AwesomeServoWriteAction", automation.Action
)
# ServoDetachAction = servo_ns.class_("AwesomeServoDetachAction", automation.Action)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        AwesomeServo,
        unit_of_measurement=UNIT_LUX,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ILLUMINANCE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.Optional("resolution"): cv.invalid(
                "The 'resolution' option has been removed. The optimal value is now dynamically calculated."
            ),
            cv.Optional("measurement_duration"): cv.invalid(
                "The 'measurement_duration' option has been removed. The optimal value is now dynamically calculated."
            ),
        }
    )
    .extend(cv.polling_component_schema("1ms"))
    .extend(i2c.i2c_device_schema(0x23))
)


async def to_code(config):
    # var = cg.new_Pvariable(config[CONF_ID])
    # await cg.register_component(var, config)

    # cg.add(var.set_restore(config[CONF_RESTORE]))

    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


@automation.register_action(
    "awesome_servo.write",
    AwesomeServoWriteAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(AwesomeServo),
            cv.Required(CONF_LEVEL): cv.templatable(cv.possibly_negative_percentage),
        }
    ),
)
async def awesome_servo_write_to_code(config, action_id, template_arg, args):
    print("[register_action]: awesome_servo.write")
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_LEVEL], args, float)
    cg.add(var.set_value(template_))
    return var


# @automation.register_action(
#     "servo.detach",
#     ServoDetachAction,
#     maybe_simple_id(
#         {
#             cv.Required(CONF_ID): cv.use_id(Servo),
#         }
#     ),
# )
# async def servo_detach_to_code(config, action_id, template_arg, args):
#     paren = await cg.get_variable(config[CONF_ID])
#     return cg.new_Pvariable(action_id, template_arg, paren)
