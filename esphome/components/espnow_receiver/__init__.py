import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import wifi
from esphome.const import (
    CONF_ID,
    CONF_PM_1_0,
    CONF_PM_2_5,
    CONF_PM_10_0,
    CONF_CO2,
    # VOC doesn't have a standard constant, use custom
    CONF_TEMPERATURE,
    CONF_HUMIDITY,
    # CH2O doesn't have a standard constant, use custom
    # CONF_OZONE, # O3
    # NO2 doesn't have a standard constant, use custom
    # CONF_VALID doesn't exist, use custom
)

# Ensure WiFi is set up
DEPENDENCIES = ['wifi']

CONF_VOC = 'voc'
CONF_CH2O = 'ch2o'
CONF_NO2 = 'no2'
CONF_VALID = 'valid'
CONF_CO = 'co'
CONF_OZONE = 'ozone' # Add custom definition for O3

espnow_receiver_ns = cg.esphome_ns.namespace('espnow_receiver')
EspnowReceiver = espnow_receiver_ns.class_('EspnowReceiver', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(EspnowReceiver),
    # Require IDs for the globals that will store the received data
    cv.Required(CONF_PM_1_0): cv.use_id(cg.global_ns.int_),
    cv.Required(CONF_PM_2_5): cv.use_id(cg.global_ns.int_),
    cv.Required(CONF_PM_10_0): cv.use_id(cg.global_ns.int_),
    cv.Required(CONF_CO2): cv.use_id(cg.global_ns.int_),
    cv.Required(CONF_VOC): cv.use_id(cg.global_ns.int_),
    cv.Required(CONF_TEMPERATURE): cv.use_id(cg.global_ns.float_),
    cv.Required(CONF_HUMIDITY): cv.use_id(cg.global_ns.int_),
    cv.Required(CONF_CH2O): cv.use_id(cg.global_ns.float_),
    cv.Required(CONF_CO): cv.use_id(cg.global_ns.float_),
    cv.Required(CONF_OZONE): cv.use_id(cg.global_ns.float_),
    cv.Required(CONF_NO2): cv.use_id(cg.global_ns.float_),
    cv.Required(CONF_VALID): cv.use_id(cg.global_ns.bool_),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Get the global variable instances and pass them to the C++ component
    pm1_0_global = await cg.get_variable(config[CONF_PM_1_0])
    cg.add(var.set_pm1_0_global(pm1_0_global))
    pm2_5_global = await cg.get_variable(config[CONF_PM_2_5])
    cg.add(var.set_pm2_5_global(pm2_5_global))
    pm10_global = await cg.get_variable(config[CONF_PM_10_0])
    cg.add(var.set_pm10_global(pm10_global))
    co2_global = await cg.get_variable(config[CONF_CO2])
    cg.add(var.set_co2_global(co2_global))
    voc_global = await cg.get_variable(config[CONF_VOC])
    cg.add(var.set_voc_global(voc_global))
    temp_global = await cg.get_variable(config[CONF_TEMPERATURE])
    cg.add(var.set_temp_global(temp_global))
    hum_global = await cg.get_variable(config[CONF_HUMIDITY])
    cg.add(var.set_hum_global(hum_global))
    ch2o_global = await cg.get_variable(config[CONF_CH2O])
    cg.add(var.set_ch2o_global(ch2o_global))
    co_global = await cg.get_variable(config[CONF_CO])
    cg.add(var.set_co_global(co_global))
    o3_global = await cg.get_variable(config[CONF_OZONE])
    cg.add(var.set_o3_global(o3_global))
    no2_global = await cg.get_variable(config[CONF_NO2])
    cg.add(var.set_no2_global(no2_global))
    valid_global = await cg.get_variable(config[CONF_VALID])
    cg.add(var.set_valid_global(valid_global)) 