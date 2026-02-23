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
    unsigned int _cnt, _cntMax;
    bool _tickGenerated;

  public:
    Ticks(unsigned long tikPeriodMilSec);
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
    unsigned int _onTimeCntMax, _onTimeCnt;
    bool _isOn; byte _buzzerPin;
  public:
    Buzzer(byte buzzerPin);
    void turnOn(byte beepLength);
    void timedTurnOff();
};

class ActUponTimeOut{ // Works based on millis(). Do not depends on Timer1 ISR
  private:
    unsigned long _timeOutMilSec, _startTime;
    bool _timerIsOn, _forceTimeOut;
  public:
    ActUponTimeOut();
    void startTimer(unsigned long timeOutMilSec);
    bool checkTimeOut();
    bool isTimerRunning();
    void stopTimer();
    void forceTimeOut();
};

class CheckTimeElapsed{ // Doesn't depends on Timer1 ISR. Works based on millis()
  private:
    unsigned long _startTime;
  public:
    CheckTimeElapsed();
    void startTimer();
    bool isTimeElapsed(unsigned long checkTimeDur);
    unsigned long getTimeElapsedMilSec();
};

class BinSemaphore{
  private:
    bool _status;
  public:
    BinSemaphore();
    void give();
    bool take();
};