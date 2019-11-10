# 実装例

## デバイス

- [ESP-WROOM-02](https://www.espressif.com/en/products/hardware/esp-wroom-02/overview)
- [BME280 モジュール](https://www.amazon.co.jp/gp/product/B01M98R905)

## 接続方法

| ESP-WROOM-02 | pin | BME280         |
| ------------ | --- | -------------- |
| SDA          | 4   | SDA            |
| SCL          | 5   | SCL            |
|              | -   | CSB(pull-up)   |
|              | -   | SDO(pull-down) |

VCCとGNDは、適切な電源へ接続されていること。

## 実行方法

```bash
$ mcconfig -d -m -p esp
```

## 備考

`manifest.json`の`include`プロパティが相対パス指定なので、ファイルの配置を変更するとパスの変更が必要になります。

ESP-WROOM-02は、表示装置を持っていないので、結果は`xsbug`のLOGへ出力しています。

