#include "xsPlatform.h"
#include "xsmc.h"
#include "mc.xs.h"
#include "xsHost.h"
#include "mc.defines.h"
#include "modGPIO.h"
#include "modI2C.h"
#include "../api/bme280.h"

#ifndef MODDEF_BME280_SDA_PIN
#define MODDEF_BME280_SDA_PIN MODDEF_I2C_SDA_PIN
#endif

#ifndef MODDEF_BME280_SCL_PIN
#define MODDEF_BME280_SCL_PIN MODDEF_I2C_SCL_PIN
#endif

#ifdef MODDEF_BME280_SECONDARY_ADDR
#define MODDEF_BME280_ADDR BME280_I2C_ADDR_SEC
#else
#define MODDEF_BME280_ADDR BME280_I2C_ADDR_PRIM
#endif

static modI2CConfigurationRecord i2cConfig;

typedef struct {
  struct bme280_dev dev;
  struct bme280_data data;
} BME280Record;

static int8_t readI2CBus(uint8_t id, uint8_t regAddr, uint8_t *data, uint16_t length);
static int8_t writeI2CBus(uint8_t id, uint8_t regAddr, uint8_t *data, uint16_t length);
static void delayMillisecconds(uint32_t ms);

void xs_BME280(xsMachine *the)
{
  int8_t result = BME280_OK;
  BME280Record bme280;

  i2cConfig.hz = 0;
  i2cConfig.sda = MODDEF_BME280_SDA_PIN;
  i2cConfig.scl = MODDEF_BME280_SCL_PIN;
  i2cConfig.address = MODDEF_BME280_ADDR;
  modI2CInit(&i2cConfig);

  bme280.dev.dev_id = MODDEF_BME280_ADDR;
  bme280.dev.intf = BME280_I2C_INTF;
  bme280.dev.read = readI2CBus;
  bme280.dev.write = writeI2CBus;
  bme280.dev.delay_ms = delayMillisecconds;

  result = bme280_init(&(bme280.dev));
  if (result != BME280_OK)
  {
    xsUnknownError("Failed to init sensor (%d)", result);
  }

  xsmcSetHostChunk(xsThis, &bme280, sizeof(bme280));
}

void xs_BME280_destructor(void *data)
{
  modI2CUninit(&i2cConfig);
}

void xs_BME280_setSensorSettings(xsMachine *the)
{
  BME280Record *bme280 = xsmcGetHostChunk(xsThis);
  int8_t result = BME280_OK;
  uint8_t settings_sel;

  xsmcVars(1);

  if (xsmcToInteger(xsArgc) == 0)
  {
    bme280->dev.settings.osr_t = BME280_NO_OVERSAMPLING;
    bme280->dev.settings.osr_p = BME280_NO_OVERSAMPLING;
    bme280->dev.settings.osr_h = BME280_NO_OVERSAMPLING;
    bme280->dev.settings.filter = BME280_FILTER_COEFF_OFF;
    bme280->dev.settings.standby_time = BME280_STANDBY_TIME_0_5_MS;
    settings_sel = 0;
  }
  else
  {
    xsmcGet(xsVar(0), xsArg(0), xsID_osrTemperature);
    bme280->dev.settings.osr_t = (xsUndefinedType == xsmcTypeOf(xsVar(0)))
                               ? BME280_NO_OVERSAMPLING
                               : xsmcToInteger(xsVar(0));

    xsmcGet(xsVar(0), xsArg(0), xsID_osrPressure);
    bme280->dev.settings.osr_t = (xsUndefinedType == xsmcTypeOf(xsVar(0)))
                               ? BME280_NO_OVERSAMPLING
                               : xsmcToInteger(xsVar(0));

    xsmcGet(xsVar(0), xsArg(0), xsID_osrHumidity);
    bme280->dev.settings.osr_t = (xsUndefinedType == xsmcTypeOf(xsVar(0)))
                               ? BME280_NO_OVERSAMPLING
                               : xsmcToInteger(xsVar(0));

    xsmcGet(xsVar(0), xsArg(0), xsID_filter);
    bme280->dev.settings.osr_t = (xsUndefinedType == xsmcTypeOf(xsVar(0)))
                               ? BME280_FILTER_COEFF_OFF
                               : xsmcToInteger(xsVar(0));

    xsmcGet(xsVar(0), xsArg(0), xsID_standbyTime);
    bme280->dev.settings.osr_t = (xsUndefinedType == xsmcTypeOf(xsVar(0)))
                               ? BME280_STANDBY_TIME_0_5_MS
                               : xsmcToInteger(xsVar(0));

    settings_sel = BME280_OSR_TEMP_SEL
                 | BME280_OSR_PRESS_SEL
                 | BME280_OSR_HUM_SEL
                 | BME280_FILTER_SEL;
  }

  result = bme280_set_sensor_settings(settings_sel, &(bme280->dev));
  if (result != BME280_OK)
  {
    xsUnknownError("Faild to set sensor settings (%d)", result);
  }
}

void xs_BME280_setSensorMode(xsMachine *the)
{
  BME280Record *bme280 = xsmcGetHostChunk(xsThis);
  int8_t result = BME280_OK;
  uint8_t sensorMode = BME280_SLEEP_MODE;

  if (xsmcToInteger(xsArgc) == 1)
  {
    sensorMode = xsmcToInteger(xsArg(0));
  }

  result = bme280_set_sensor_mode(sensorMode, &(bme280->dev));
  if (result != BME280_OK)
  {
    xsUnknownError("Failed to set sensor mode (%d)", result);
  }
}

void xs_BME280_getSensorMode(xsMachine *the)
{
  BME280Record *bme280 = xsmcGetHostChunk(xsThis);
  int8_t result = BME280_OK;
  uint8_t sensorMode;

  result = bme280_get_sensor_mode(&sensorMode, &(bme280->dev));
  if (result != BME280_OK)
  {
    xsUnknownError("Failed to get sensor mode (%d)", result);
  }

  xsResult = xsInteger(sensorMode);
}

void xs_BME280_update(xsMachine *the)
{
  BME280Record *bme280 = xsmcGetHostChunk(xsThis);
  int8_t result = BME280_OK;

  result = bme280_get_sensor_data(BME280_ALL, &(bme280->data), &(bme280->dev));
  if (result != BME280_OK)
  {
    xsUnknownError("Failed to get sensor data (%d)", result);
  }
}

void xs_BME280_get_temperature(xsMachine *the)
{
  BME280Record *bme280 = xsmcGetHostChunk(xsThis);
  xsResult = xsNumber(bme280->data.temperature);
}

void xs_BME280_get_pressure(xsMachine *the)
{
  BME280Record *bme280 = xsmcGetHostChunk(xsThis);
  xsResult = xsNumber(bme280->data.pressure / 100); // Pa => hPa
}

void xs_BME280_get_humidity(xsMachine *the)
{
  BME280Record *bme280 = xsmcGetHostChunk(xsThis);
  xsResult = xsNumber(bme280->data.humidity);
}

static int8_t readI2CBus(uint8_t id, uint8_t regAddr, uint8_t *data, uint16_t length)
{
  uint8_t result;

  result = modI2CWrite(&i2cConfig, &regAddr, 1, false);
  if (result)
  {
    return BME280_E_COMM_FAIL;
  }

  result = modI2CRead(&i2cConfig, data, length, true);
  if (result)
  {
    return BME280_E_COMM_FAIL;
  }

  return BME280_OK;
}

static int8_t writeI2CBus(uint8_t id, uint8_t regAddr, uint8_t *data, uint16_t length)
{
  /*
   * Data provided by the Bosch Sensor API
   *
   * single byte:
   * regAddr
   *  |  data[length = 1]
   *  |  +-------+
   *  +->| data1 |
   *     +-------+
   *
   * multiple byte:
   * regAddr
   *  |  data[length = 5]
   *  |  +-------+-------+-------+-------+-------+
   *  +->| data1 | addr2 | data2 | addr3 | data3 |
   *     +-------+-------+-------+-------+-------+
   */

  uint8_t result;

  result = modI2CWrite(&i2cConfig, &regAddr, 1, false);
  if (result)
  {
    return BME280_E_COMM_FAIL;
  }

  /*
   * When writing multiple bytes in I2C, bme280 does not
   * automatically increment the address pointer.
   */
  for (int i=0; i<length-1; i++) {
    result = modI2CWrite(&i2cConfig, &data[i], 1, false);
    if (result)
    {   
      return BME280_E_COMM_FAIL;
    }   
  }

  result = modI2CWrite(&i2cConfig, &data[length-1], 1, true);
  if (result)
  {
    return BME280_E_COMM_FAIL;
  }

  return BME280_OK;
}

static void delayMillisecconds(uint32_t ms)
{
  modDelayMilliseconds(ms);
}

