#include <Arduino.h>
#include <./tilt.h>
#include <./voice.h>

#define TILT_WAIT 50             // 傾き読み込み待ち時間
#define TILT_THRESOLD 0.35       // 傾き閾値
#define VOICE_VOLUME 10          // 音量
#define VOICE_NUM 4              // 音声ファイル数
#define VOICE_SPECIAL_INTERVAL 5 // 特別な音声を流す間隔
#define VOICE_PLAY_WAIT 1000     // ボイス再生待ち(ms)

/**
 * @brief フェーズ
 * 
 */
enum Phase
{
  PHASE_PROC_FETCH_TILT_WAIT, // 傾き読み込み待ち
  PHASE_PROC_FETCH_TILT,      // 傾き読み込み
  PHASE_PROC_PLAY_VOICE,      // ボイス再生
  PHASE_PROC_PLAY_VOICE_WAIT, // ボイス再生待ち
  PHASE_TERM,                 // 終了
  PHASE_ERROR,                // エラー
};

extern void SetPhase(Phase value);

Phase phase;                 // フェーズ
unsigned long phaseTime;     // フェーズ開始時間
unsigned int voicePlayCount; // ボイス再生回数
Tilt tilt;                   // 傾き
Voice voice;                 // ボイス
int tilt_x, tilt_y;          // XY傾き方向

/**
 * @brief セットアップ
 * 
 */
void setup()
{
  voicePlayCount = 0;
  tilt_x = 0;
  tilt_y = 0;

  // モニター初期化
  Serial.begin(115200);
  // // モニター通信が確率するまで or 最大三秒待ち
  // unsigned long waitStartTime = millis();
  // while (!Serial || millis() >= waitStartTime + 3000)
  // {
  //   delay(10);
  // }

  // 初期化
  Serial.println("initialize start.");
  if (tilt.Initialize() &&      // 傾き
      voice.Initialize(A3, A2)) // ボイス
  {
    Serial.println("initialize success.");
    SetPhase(PHASE_PROC_FETCH_TILT_WAIT);
  }
  else
  {
    Serial.println("initialize error.");
    SetPhase(PHASE_ERROR);
  }
}

/**
 * @brief メインループ
 * 
 */
void loop()
{
  unsigned long nowTime = millis();

  switch (phase)
  {
  // 傾き読み込み待ち
  case PHASE_PROC_FETCH_TILT_WAIT:
    if (nowTime >= phaseTime + TILT_WAIT)
    {
      SetPhase(PHASE_PROC_FETCH_TILT);
      break;
    }
    break;

  // 傾き読み込み
  case PHASE_PROC_FETCH_TILT:
    tilt.Fetch();

    {
      // X/Y の傾きが一定以上であればボイスを再生
      // そうでなければ、傾き読み込み待ちへ
      float accx = tilt.acc_x;
      if (accx < 0)
      {
        accx *= -1;
      }
      int tx = (accx > TILT_THRESOLD) ? (tilt.acc_x > 0 ? 1 : -1) : 0;
      float accy = tilt.acc_y;
      if (accy < 0)
      {
        accy *= -1;
      }
      int ty = (accy > TILT_THRESOLD) ? (tilt.acc_y > 0 ? 1 : -1) : 0;
      if (tilt_x != tx || tilt_y != ty)
      {
        // 傾きが変わっていればボイス再生へ
        tilt_x = tx;
        tilt_y = ty;
        SetPhase(PHASE_PROC_PLAY_VOICE);
      }
      else
      {
        // 傾きが変わってなければ、傾き読み込み待ちへ
        SetPhase(PHASE_PROC_FETCH_TILT_WAIT);
      }
    }
    break;

  // ボイス再生
  case PHASE_PROC_PLAY_VOICE:
    voicePlayCount++;
    if ((voicePlayCount % VOICE_SPECIAL_INTERVAL) == 0)
    {
      // n回に1回特別なボイスを再生
      voice.Play(VOICE_NUM, VOICE_VOLUME);
    }
    else
    {
      // ランダム再生
      voice.Play(random(1, VOICE_NUM), VOICE_VOLUME);
    }

    // ボイス再生待ちへ
    SetPhase(PHASE_PROC_PLAY_VOICE_WAIT);
    break;

  // ボイス再生待ち
  case PHASE_PROC_PLAY_VOICE_WAIT:
    delay(10);
    if (nowTime >= phaseTime + VOICE_PLAY_WAIT)
    {
      // 傾き読み取り待ちへ
      SetPhase(PHASE_PROC_FETCH_TILT_WAIT);
    }
    break;

  case PHASE_TERM:
  case PHASE_ERROR:
    delay(100);
    break;
  }
}

/**
 * @brief フェーズ設定
 * 
 * @param value 
 */
void SetPhase(Phase value)
{
  phase = value;
  phaseTime = millis();
}
