# 実装例

## デバイス

- [M5Stack](https://m5stack.com/)
- [BME280 モジュール](https://www.amazon.co.jp/gp/product/B01M98R905)

## 接続方法

| M5Stack | pin | BME280 |
| ------- | --- | ------ |
| 3V3     | -   | VCC    |
| G       | -   | GND    |
| SDA     | 21  | SDA    |
| SCL     | 22  | SCL    |
| 3V3     | -   | CSB    |
| GND     | -   | SDO    |

## 実行方法

```bash
$ mcconfig -d -m -p esp32/m5stack
```

## 備考

`manifest.json`の`include`プロパティが相対パス指定なので、ファイルの配置を変更するとパスの変更が必要になります。

