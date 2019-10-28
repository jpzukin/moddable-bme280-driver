
export default class BME280 @ "xs_BME280_destructor" {
  get temperature() @ "xs_BME280_get_temperature";
  get pressure() @ "xs_BME280_get_pressure";
  get humidity() @ "xs_BME280_get_humidity";

  constructor() @ "xs_BME280";

  setSensorSettings(dictionary) @ "xs_BME280_setSensorSettings";

  getSensorMode() @ "xs_BME280_getSensorMode";
  setSensorMode(sensorMode) @ "xs_BME280_setSensorMode";

  update() @ "xs_BME280_update";
};

/* Sensor power modes */
BME280.SLEEP_MODE  = 0x00;
BME280.FORCED_MODE = 0x01;
BME280.NORMAL_MODE = 0x03;

/* Oversampling */
BME280.NO_OVERSAMPLING  = 0x00;
BME280.OVERSAMPLING_1X  = 0x01;
BME280.OVERSAMPLING_2X  = 0x02;
BME280.OVERSAMPLING_4X  = 0x03;
BME280.OVERSAMPLING_8X  = 0x04;
BME280.OVERSAMPLING_16X = 0x05;

/* Standby duration */
BME280.STANDBY_TIME_0_5_MS  = 0x00;
BME280.STANDBY_TIME_62_5_MS = 0x01;
BME280.STANDBY_TIME_125_MS  = 0x02;
BME280.STANDBY_TIME_250_MS  = 0x03;
BME280.STANDBY_TIME_500_MS  = 0x04;
BME280.STANDBY_TIME_1000_MS = 0x05;
BME280.STANDBY_TIME_10_MS   = 0x06;
BME280.STANDBY_TIME_20_MS   = 0x07;

/* IIR filter coefficient */
BME280.FILTER_COEFF_OFF = 0x00;
BME280.FILTER_COEFF_2   = 0x01;
BME280.FILTER_COEFF_4   = 0x02;
BME280.FILTER_COEFF_8   = 0x03;
BME280.FILTER_COEFF_16  = 0x04;

Object.freeze(BME280);

