#pragma once
#include <Arduino.h>
#include "GlobalConstants.h" // include here only to use this in other projects

class BlinkActivity{ // Doesn't depends on Timer1 ISR. It is designed to be used with Ticks class
  private:
    unsigned int _cnt, _cntMax;
    bool _statusOn;
  public:
    bool enableActivity;
    BlinkActivity();
    void srtActivity(unsigned int maxRepeats);
    bool switchOn();
    bool switchOff();
};

class Ticks // depends on Timer1 ISR
{
  private:
    uint16_t _cnt, _cntMax;
    bool _tickGenerated;

  public:
    Ticks(uint32_t tikPeriodMilSec);
    bool tick_Gen_Run(); // to be kept inside ISR. If tick generated during that ISR iteration, gives true, else gives false.
    bool tick_Utilize(); // to be kept outside ISR. Gives true for onetime, after tick generated.
    void force_Gen_Tick();
};

class CircularCounter // Doesn't depends on Timer1 ISR. It can be used with or without Ticks class 
{
  private:
    byte _cntMax; bool _enableCnt, _justIncr;
  public:
    byte count;
    CircularCounter(byte cntMax);
    void enableCnt();
    void incrCnt();
    void disableCnt();
    bool checkJustIncr();
};

class Buzzer{ // depends on Timer1 ISR
  private:
    uint16_t _onTimeCntMax, _onTimeCnt;
    bool _isOn; byte _buzzerPin;
  public:
    Buzzer(byte buzzerPin);
    void turnOn(byte beepLength);
    void timedTurnOff();
};

enum class TimerModes : uint8_t {infRunning, finOneShot, finPeriodic};
enum class TimerStates : uint8_t {stopped, running, expired};

class NonBlockingTimer{ // Works based on Timer1 ISR. Doesn't use millis(). Max input for 'timeOutMillis' is 65,00,000 millis
  private:
    uint16_t _timeOutCnt, _timeOutCntMax;
    TimerModes _timerMode; TimerStates _timerState;
  public:
    NonBlockingTimer();
    void start_infRunning();
    void start_finOneShot(uint32_t timeOutMillis);
    void start_finPeriodic(uint32_t timeOutMillis);
    void update();
    bool event();
    void restart();
    bool isModeInfRunning();
    bool isModeFinOneShot();
    bool isModeFinPeriodic();
    bool isRunning();
    bool isExpired();
    bool isStopped();
    void stop();
    void forceExpire();
    uint32_t elapsed();
    uint32_t remaining();
    uint32_t timeOutVal();
    uint8_t percentComplete();
};

class BinSemaphore{
  private:
    bool _status;
  public:
    BinSemaphore();
    void give();
    bool take();
};

class Button {
public:
    uint8_t pin;

    bool lastState;
    bool currentState;
    bool stableState;
    
    unsigned long lastChangeMillis;
    unsigned long currentScanMillis;
    unsigned long debounceTimeMilSec;

    Button(uint8_t buttonPin);
    bool scanButton();
};