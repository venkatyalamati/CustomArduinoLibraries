#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "GlobalConstants.h" // include here only to use this in other projects

class PreemptiveOnOff{ // Depends on Timer1 ISR. It is designed to use without Ticks class
  private:
    uint16_t _tikCnt, _tikCntOn, _tikCntMax;
    uint8_t _cycleCnt, _cycleCntMax;
    bool _enableActivity, _onActDone, _offActDone;
  public:
    PreemptiveOnOff(uint32_t cyclePeriodMillis, uint8_t dutCylPercent);
    void start(uint8_t maxRepeats);
    void executor(void (*task_On_Ptr)(), void (*task_Off_Ptr)(), void (*task_Final_Ptr)());
    bool isRunning();
    void stop();
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

class ButtonMillisBased {
private:
    uint8_t pin;
    bool lastState;
    bool currentState;
    bool stableState;
    unsigned long lastChangeMillis;
    unsigned long debounceTimeMilSec;
public:
    ButtonMillisBased(uint8_t buttonPin);
    bool scanButton();
};

class ButtonTimer2Based {
private:
    uint8_t _pin;
    bool _lastState;
    bool _currentState;
    bool _stableState;
    uint16_t _lastChangeTick;
public:
    static volatile uint16_t timer2_isr_tick;
    static volatile uint16_t debounceTicks;
    static volatile uint8_t pinButtPressed;
    
    ButtonTimer2Based(uint8_t pin);
    static void setDebNormPress();
    static void setDebLongPress();
    void setPinMode();
    void scanButton();
};

class LevelSensor{
  public:
    int16_t addrEmptyVal, addrFullVal, emptyMarkVal, fullMarkVal; uint8_t sensorPin;
    LevelSensor(uint8_t sensorPin, int16_t addrEEPROM);
    void storeDfltCalParameters();
    void loadCalParameters();
    void calibrateEmpty();
    void calibrateFull();
    float getTankLevelPercent();
    int16_t sensorRead();
};

class ShiftRegisterController {
  public:
    ShiftRegisterController(uint8_t OE_Pin, uint8_t latchPin, uint8_t dataPin, uint8_t clkPin);
    void doStartUpActions();
    void updateOutputs(const uint8_t DO_StatusArr[]);
    void disableOutput();
    void enableOutput();

  private:
    uint8_t _OE_Pin, _latchPin, _dataPin, _clkPin;
    uint8_t _dataByteDO; bool _outputEnabled;
};