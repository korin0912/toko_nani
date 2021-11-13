#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

/**
 * @brief ボイス
 * 
 * note: https://blog.hrendoh.com/how-to-use-dfplayer-mini/
 */
class Voice
{
private:
  SoftwareSerial *softweaSerial;
  DFRobotDFPlayerMini *dfPlayerMini;

public:
  /**
   * @brief 初期化
   * 
   * @param receivePin 
   * @param transmitPin 
   * @return true 
   * @return false 
   */
  bool Initialize(uint8_t receivePin, uint8_t transmitPin)
  {
    Serial.println("SoftwareSerial initialize.");
    softweaSerial = new SoftwareSerial(receivePin, transmitPin);
    softweaSerial->begin(9600);

    Serial.println("DFPlayer Mini initialize.");
    dfPlayerMini = new DFRobotDFPlayerMini();
    if (dfPlayerMini->begin(*softweaSerial))
    {
      Serial.println("ERR: initialize fail.");
      Serial.println("ERR: DFPlayer Mini setup fail.");
      return false;
    }

    return true;
  }

  /**
   * @brief 再生
   * 
   * @param index 
   * @param volume 
   */
  void Play(unsigned int index, uint8_t volume)
  {
    Serial.print("play voice: ");
    Serial.println(index);

    dfPlayerMini->volume(volume);
    dfPlayerMini->play(index);
  }
};
