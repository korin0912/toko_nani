#include <Arduino.h>
#include <Wire.h>

#define MPU6050_ADDRESS 0x68            // MPU-6050 のアドレス
#define MPU6050_REG_WHO_AM_I 0x75       // デバイス識別情報読み込みレジスタ
#define MPU6050_REG_PWR_MGMT_1 0x6B     // パワーマネージメント設定レジスタ
#define MPU6050_REG_READ_SENSER 0x3B    // X軸加速度データの上位バイトレジスタ
#define MPU6050_ACC_RESOLUTION 16384.0; // 加速度の分解能
#define MPU6050_GYRO_RESOLUTION 131.0;  // 角速度の分解能

/**
 * @brief 傾き
 * 
 * note: https://shizenkarasuzon.hatenablog.com/entry/2019/02/16/162647
 */
class Tilt
{
public:
  // 加速度
  float acc_x;
  float acc_y;
  float acc_z;

  // 動作温度
  int16_t temperature;

  // 角速度
  float gyro_x;
  float gyro_y;
  float gyro_z;

public:
  /**
   * @brief 初期化
   * 
   * @return true 
   * @return false 
   */
  bool Initialize()
  {
    // MPU-6050 初期化
    Serial.println("MPU-6050 initialize.");
    Wire.begin();

    // デバイス生存確認
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_REG_WHO_AM_I);
    Wire.write(0x00);
    Wire.endTransmission();

    // パワーマネージメント設定
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_REG_PWR_MGMT_1);
    Wire.write(0x00);
    Wire.endTransmission();

    return true;
  }

  /**
   * @brief 読み込み
   * 
   */
  void Fetch()
  {
    // データ読み込み
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_REG_READ_SENSER);
    Wire.endTransmission(false);

    // データ読み込み待ち
    Wire.requestFrom(MPU6050_ADDRESS, 14, true);
    while (Wire.available() < 14)
      ;

    int16_t acx, acy, acz, gyx, gyy, gyz, tmp;

    // 加速度
    acx = Wire.read() << 8 | Wire.read();
    acy = Wire.read() << 8 | Wire.read();
    acz = Wire.read() << 8 | Wire.read();

    // 動作温度
    tmp = Wire.read() << 8 | Wire.read();

    // 各速度
    gyx = Wire.read() << 8 | Wire.read();
    gyy = Wire.read() << 8 | Wire.read();
    gyz = Wire.read() << 8 | Wire.read();

    // 分解能で割る
    acc_x = acx / MPU6050_ACC_RESOLUTION;
    acc_y = acy / MPU6050_ACC_RESOLUTION;
    acc_z = acz / MPU6050_ACC_RESOLUTION;

    gyro_x = gyx / MPU6050_GYRO_RESOLUTION;
    gyro_y = gyy / MPU6050_GYRO_RESOLUTION;
    gyro_z = gyz / MPU6050_GYRO_RESOLUTION;

    temperature = tmp;
  }

  /**
   * @brief 値出力
   * 
   */
  void Print()
  {
    Serial.print("Tilt: (");
    Serial.print(acc_x);
    Serial.print(",");
    Serial.print(acc_y);
    Serial.print(",");
    Serial.print(acc_z);
    Serial.print("),(");
    Serial.print(gyro_x);
    Serial.print(",");
    Serial.print(gyro_y);
    Serial.print(",");
    Serial.print(gyro_z);
    Serial.println(")");
  }
};
