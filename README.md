# BME280 センサーを制御するための Moddable SDK ライブラリ

## 目次

- [概要](#introduction)
- [対応デバイス](#target-device)
- [参考資料](#reference-materials)
- [利用方法](#usage)
- [詳細](#details)
- [ライセンスと免責事項](#license)

<a id="introduction"></a>

## 概要

BME280 は、Bosch Sensortec が提供している湿度、気圧、気温を測るセンサーです。それぞれの計測範囲は、気温が-40℃ から+85℃、湿度が 0 から 100%、気圧が 300 から 1100hPa です。

BME280 は、I2C と SPI の両方のインターフェースが利用できますが、このライブラリは I2C のみに対応しています。

Bosch Sensortec が github で公開している API を利用して、制御と計測値の較正を行っています。

- [Bosch Sensortec - BME280_driver](https://github.com/BoschSensortec/BME280_driver)

<a id="target-device"></a>

## 対応デバイス

現バージョンは、ESP32 ベースのデバイスにのみに対応しています。

<a id="reference-materials"></a>

## 参考資料

データシートのバージョン 1.6 をベースに実装しています。

- [Bosch Sensortec - BME280](https://www.bosch-sensortec.com/bst/products/all_products/bme280)

<a id="usage"></a>

## 利用方法

### 準備

始めに、自身のプロジェクトのサブディレクトリとして、このリポジトリをコピーします。

`manifest.json`が上書きされてしまうので、プロジェクト直下にはコピーしないように注意してください。

サブディレクトリ名は`manifest.json`の`include`プロパティに指定するパスになるので、任意の名前が使えます。

```sh
$ mkdir your_project
$ cd your_project
$ git clone https://github.com/jpzukin/moddalbe-bme280-driver bme280
```

次に、`manifest.json`の`include`プロパティに、このライブラリの`manifest.json`のパスを追加します。

実装例(`example-bme280`の`manifest.json`):

```json
{
  "include": {
    "$(MODDABLE)/examples/manifest_base.json",
    "$(MODDABLE)/examples/manifest_piu.json",
    "./bme280/manifest.json"
  },
  "modules": {
    "*": "./main"
  },
  "resources": {
    "$(MODDABLE)/examples/assets/fonts/OpenSans-Semibold-28"
  }
}
```

### 実装例

実装例は`example-bme280`ディレクトリにあります。

1. モジュールをインポートする
2. インスタンスを生成する
3. センサーのパラメータを設定する
4. センサーモードを設定する
5. センサーから計測値を読み出す
6. それぞれのプロパティから計測値を取得する
7. 5. 6.を一定間隔で繰り返し実行する

定数、プロパティ、メソッドについては、詳細を参照してしてください。

```javascript
import {} from "piu/MC";
import Timer from "timer";

// 1) モジュールをインポートする
import BME280 from "bme280";

const Font = "OpenSans-Semibold-28";

const LableTemplate = Label.template($ => ({
  name: $,
  left: 0,
  height: 80
}));

export default application = new Application(null, {
  style: new Style({ font: Font, color: "#000000" }),
  skin: new Skin({ fill: "#FFFFFF" }),
  contents: [new LableTemplate("T", { top: 0 }), new LableTemplate("P", { top: 80 }), new LableTemplate("H", { top: 160 })]
});

// 2) インスタンスの生成する
const bme280 = new BME280();

// 3) センサーのパラメータを設定する
bme280.setSensorSettings({
  osrTemperature: BME280.OVERSAMPLING_X2,
  osrPressure: BME280.OVERSAMPLING_X16,
  osrHumidity: BME280.OVERSAMPLING_X1,
  filter: BME280.FILTER_COEFF_16,
  standbyTime: BME280.STANDBY_TIME_0_5_MS
});

// 4) センサーモードを設定する
bme280.setSensorMode(BME280.NORMAL_MODE);

// 一定間隔で計測を取得して表示を更新する
Timer.repeat(id => {
  // 5) センサーから計測値を読み出す
  bme280.update();

  // 6) それぞれのプロパティから気温、気圧、湿度を取得する
  application.content("T").string = "Temperature = " + bme280.temperature.toFixed(2) + "C";
  application.content("P").string = "Pressure = " + bme280.pressure.toFixed(2) + "hPa";
  application.content("H").string = "Humidity = " + bme280.humidity.toFixed(2) + "%";
}, 1000);
```

<a id="details"></a>

## 詳細

### class BME280

このクラスは、BME280 を制御する機能を提供します。このライブラリの唯一のクラスなので、次のようにインポートします。

```javascript
import BME280 from "bme280";
```

#### `constructor()`

コンストラクタは、I2C による通信を有効にして、BME280 のセットアップを行います。

```javascript
let bme280 = new BME280();
```

引数はありません。I2C の SDA と SCL のピン番号と、BME280 の I2C アドレスは、`manifest.json`の`defines`プロパティにより設定します。

I2C の SDA と SCL のピン番号を省略すると、デバイスのデフォルト値が設定されます。

BME280 は２つの I2C アドレスを持っています。プライマリーアドレス(0x76)ならばデフォルト値なので何も設定しません。セカンダリーアドレス(0x77)を利用するならば、`secondary_addr`を指定します。

```json
  ...
  "defines": {
    "bme280": {
      "secondary_addr": true,
      "sda_pin": 21,
      "scl_pin": 22
    }
  }
  ...
```

#### `setSensorSettings([dictionary])`

このメソッドは、BME280 の次のパラメータを設定します。

設定の変更後は、センサーモードが`SLEEP_MODE`となります。新しく計測を行うにはセンサーモードを、`setSensorMode()`により`NORMAL_MODE`か`FORCED_MODE`に設定する必要があります。

- 気圧のオーバーサンプリング設定
- 湿度のオーバーサンプリング設定
- 気温のオーバーサンプリング設定
- IIR フィルター係数
- NORMAL モード時の非アクティブ時間(スタンバイタイム)

オーバーサンプリングに設定できる値
|定数|説明|
|---|---|
|NO_OVERSAMPLING|計測しない|
|OVERSAMPLING_X1|1 倍|
|OVERSAMPLING_X2|2 倍|
|OVERSAMPLING_X4|4 倍|
|OVERSAMPLING_X16|16 倍|

IIR フィルターに設定できる値
|定数|説明|
|---|---|
|FILTER_COEFF_OFF|フィルターなし|
|FILTER_COEFF_2|2|
|FILTER_COEFF_4|4|
|FILTER_COEFF_8|8|
|FILTER_COEFF_16|16|

スタンバイタイムに設定できる値
|定数|説明|
|---|---|
|STANDBY_TIME_0_5_MS|0.5ms|
|STANDBY_TIME_10_MS|10ms|
|STANDBY_TIME_20_MS|20ms|
|STANDBY_TIME_62_5_MS|62.5ms|
|STANDBY_TIME_125_MS|127ms|
|STANDBY_TIME_250_MS|250ms|
|STANDBY_TIME_500_MS|500ms|
|STANDBY_TIME_1000_MS|1000ms|

オーバーサンプリングと IIR フィルターの詳細はデータシートを参照してください。

#### `getSensorMode()`

このメソッドは、現在のセンサーモードを BME280 から読み出します。ライブラリ内部にバッファした値ではありません。

戻り値は、 `SLEEP_MODE`、`NORMAL_MODE`、`FORCED_MODE`のいづれかです。

#### `setSensorMode([sensorMode])`

このメソッドは、センサーモードを設定します。`sensorMode`を省略した場合のデフォルト値は`SLEEP_MODE`です。

設定できるセンサーモードは、`SLEEP_MODE`、`NORMAL_MODE`、`FORCED_MODE`のいづれかです。

センサーモードに設定できる値
|定数|説明|
|---|---|
|SLEEP_MODE|計測を停止して待機する|
|NORMAL_MODE|一定間隔で計測を続ける|
|FORCED_MODE|一度だけ計測する(計測後は SLEEP_MODE も遷移する)|

センサーモードの詳細はデータシートを参照してください。

#### `update()`

このメソッドは、BME280 から計測値を読み出し、ライブラリが保持している計測値を更新します。

#### `temperature`プロパティ

このプロパティは、ライブラリが保持している気温を返します。最新の値を取得するには`update()`により計測値の更新が必要です。

このプロパティは、読み取り専用です。

#### `pressure`プロパティ

このプロパティは、ライブラリが保持している気圧を返します。最新の値を取得するには`update()`により計測値の更新が必要です。

このプロパティは、読み取り専用です。

#### `humidity`プロパティ

このプロパティは、ライブラリが保持している湿度を返します。最新の値を取得するには`update()`により計測値の更新が必要です。

このプロパティは、読み取り専用です。

<a id="license"></a>

### ライセンスと免責事項

Bosch Sensortec の API については`./api/LICENCE`の通りです。その他は、Moddalbe SDK と同様に、GNU Lesser General Public License version 3 以降です。

