#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <Ucglib.h>
constexpr uint16_t TIMER1_PERIOD_MILLIS = 100; // constexpr is better than const for compile-time constants
constexpr uint16_t TIMER2_PERIOD_MILLIS = 10;
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
    bool runTickGen(); // to be kept inside ISR. If tick generated during that ISR iteration, gives true, else gives false.
    bool tickUtilize(); // to be kept outside ISR. Gives true for onetime, after tick generated.
    void genTickNow();
    void rstTickGen();
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
    Buzzer(uint8_t buzzerPin);
    void begin();
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
    uint32_t elapsedMillis();
    uint32_t remainingMillis();
    uint32_t timeOutMillis();
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

class ButtonReadConventional{
  private:
  static void (*onPressCallback)(uint8_t);   // store function pointer
  public:
  static void setCallback(void (*funcPtr)(uint8_t));
  static void readButton(uint8_t pinNum);
  static void readButtonLongPress(uint8_t pinNum);
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
  private:
    static constexpr uint8_t NUM_SAMPLES = 6;
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
    void updateOutputs(const uint8_t* DO_StatusArr);
    void disableOutput();
    void enableOutput();

  private:
    uint8_t _OE_Pin, _latchPin, _dataPin, _clkPin;
    uint8_t _dataByteDO; bool _outputEnabled;
};

class TFTDisplay{
  private:
    static constexpr uint8_t CHAR_BUFF_SIZE = 17;
    Ucglib_ST7735_18x128x160_HWSPI ucg;
  public:
    char charBuffer[CHAR_BUFF_SIZE];
    TFTDisplay(uint8_t cdPin, uint8_t rstPin);          // constructor — wires up ucg pins
    void begin();
    void clearScreen();
    void setSmallFont();
    void setBigFont();
    void textPrintSmallFont(float linePos);   // linePos 1,2,3...
    void textPrintBigFont(float linePos);
    void clearCharBuffer();
};

// -------------- TimersFunctions class ---------------
class ArduinoTimersFunctions{
  public:
  // ─── Timer1 Setup (initializes but leaves disabled) ──────────────────────────
  // Period range: 1ms to 4194ms (prescaler fixed at 1024, 16MHz)
  static void setupTimer1();
  static void enableTimer1();
  static void disableTimer1();
  // ─── Timer2 Setup (initializes but leaves disabled) ─────────────────────────
  // Period range: 1ms to 16ms (prescaler fixed at 1024, 16MHz)
  static void setupTimer2();
  static void enableTimer2();
  static void disableTimer2();
};

class EEPROM_Functions{
  public:
  static bool erase_eeprom_if_req(int addrValStr);
};

class DecimalToCharFunctions{
  private:
  static constexpr uint8_t SIZE_TEMP_STR = 7; // must be > fullDcmlLen
  public:
  static void insertFloatVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, uint8_t numDcmlPts, float dcmlVal, bool dispPosSign);
  static void insertUintVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, uint32_t dcmlVal);
  static void insertIntVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, int32_t dcmlVal, bool dispPosSign);
  static float floatFromCharArray(const char *str);
};