import Timer from "timer";

// 1) モジュールをインポートする
import BME280 from "bme280";

// 2) インスタンスの生成する
const bme280 = new BME280();

// 3) センサーのパラメータを設定する
// Indoor navigation
bme280.setSensorSettings({
  osrTemperature: BME280.OVERSAMPLING_2X,
  osrPressure: BME280.OVERSAMPLING_16X,
  osrHumidity: BME280.OVERSAMPLING_1X,
  filter: BME280.FILTER_COEFF_16,
  standbyTime: BME280.STANDBY_TIME_0_5_MS
}); 

// 4) センサーモードを設定する
bme280.setSensorMode(BME280.NORMAL_MODE);

// 一定間隔で測定値を取得して表示を更新する
Timer.repeat(id => {

  // 5) センサーから測定値を読み出す
  bme280.update();

  // 6) 各プロパティから気温、気圧、湿度を取得する
  trace(`Temperature = ${bme280.temperature.toFixed(2)}C,`);
  trace(`Pressure = ${bme280.pressure.toFixed(2)}hPa,`);
  trace(`Humidity = ${bme280.humidity.toFixed(2)}%\n`);

}, 1000);

