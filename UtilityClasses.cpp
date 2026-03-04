#include "UtilityClasses.h"

// Note:- Do not use delay(), Serial.print() etc. inside the constructor
// Ticks class implementations
  Ticks::Ticks(uint32_t tikPeriodMilSec){
    _cntMax = tikPeriodMilSec/TIMER1_PERIOD_MILLSEC;
    _cnt = 0; _tickGenerated = false;
  }
  bool Ticks::tick_Gen_Run(){
    _cnt++;
    if(_cnt >= _cntMax){
      _cnt = 0;
      _tickGenerated = true;
      return true;
    }
    else{
      return false;
    }
  }
  bool Ticks::tick_Utilize(){
    if(_tickGenerated){
      _tickGenerated = false;
      return true;
    }
    else{
      return false;
    }
  }
  void Ticks::force_Gen_Tick(){
    _tickGenerated = true;
    _cnt = 0;
  }

  // Buzzer class implementations
  Buzzer::Buzzer(byte pin){
    _buzzerPin = pin; _isOn = false;
    pinMode(_buzzerPin, OUTPUT); digitalWrite(_buzzerPin, LOW);
  }
  void Buzzer::turnOn(byte beepLength){ //beepLength as number of ISR cycles
    if(beepLength!=0){
      _onTimeCntMax = beepLength;
      _isOn = true;
      _onTimeCnt = 0;
      digitalWrite(_buzzerPin, HIGH);
    }
  }
  void Buzzer::timedTurnOff(){
    if(_isOn){ // check for beep duration
      _onTimeCnt++;
      if(_onTimeCnt >= _onTimeCntMax){
        digitalWrite(_buzzerPin, LOW);
        _isOn = false;      
      }
    }
  }

// BlinkActivity class implementations
  BlinkActivity::BlinkActivity(){
    enableActivity = false; _statusOn = false;
  }
  void BlinkActivity::srtActivity(unsigned int maxRepeats){
    _cntMax = maxRepeats;
    _cnt = 0;
    enableActivity = true;
  }
  bool BlinkActivity::switchOn(){
    if(enableActivity){
      _statusOn = true;
      return true;
    }
    else{
      return false;
    }
  }
  bool BlinkActivity::switchOff(){
    if(_statusOn){
      _statusOn = false;
      _cnt++;
      if(_cnt >= _cntMax)
        enableActivity = false;
      return true;
    }
    else{
      return false;
    }
  }
  
// NonBlockingTimer class implementations
  NonBlockingTimer::NonBlockingTimer():_timeOutCnt(0),_timeOutCntMax(0),_timerMode(TimerModes::infRunning),_timerState(TimerStates::stopped)
  {} // Using initializer list is better on Arduino (more efficient)
  void NonBlockingTimer::start_infRunning(){
    _timeOutCnt = 0;
    _timerState = TimerStates::running;
    _timerMode = TimerModes::infRunning;
  }
  void NonBlockingTimer::start_finOneShot(uint32_t timeOutMillis){
    _timeOutCntMax = static_cast<uint16_t>(timeOutMillis/static_cast<uint32_t>(TIMER1_PERIOD_MILLSEC));
    _timeOutCnt = 0;
    _timerState = TimerStates::running;
    _timerMode = TimerModes::finOneShot;
  }
  void NonBlockingTimer::start_finPeriodic(uint32_t timeOutMillis){
    _timeOutCntMax = static_cast<uint16_t>(timeOutMillis/static_cast<uint32_t>(TIMER1_PERIOD_MILLSEC));
    _timeOutCnt = 0;
    _timerState = TimerStates::running;
    _timerMode = TimerModes::finPeriodic;
  }
  void NonBlockingTimer::update(){
    if(_timerState == TimerStates::running && _timerMode != TimerModes::infRunning){
      _timeOutCnt++;
      if( _timeOutCnt >= _timeOutCntMax){
        if(_timerMode == TimerModes::finPeriodic){
          _timeOutCnt = 0; // auto reload
          _timerState = TimerStates::expired;
        }
        else{ // finOneShot mode
          _timerState = TimerStates::expired;
        }
      }
    }
  }
  bool NonBlockingTimer::event(){
    if(_timerState == TimerStates::expired){
      if(_timerMode == TimerModes::finOneShot){
        _timerState = TimerStates::stopped;
        _timeOutCntMax = 0;
      }
      else{ // finite periodic mode
        _timerState = TimerStates::running;
      } 
      return true;
    }
    return false;
  }
  void NonBlockingTimer::restart(){
    _timeOutCnt = 0;
    _timerState = TimerStates::running;
  }
  bool NonBlockingTimer::isModeInfRunning(){
    if(_timerMode == TimerModes::infRunning)
      return true;
    return false;
  }
  bool NonBlockingTimer::isModeFinOneShot(){
    if(_timerMode == TimerModes::finOneShot)
      return true;
    return false;
  }
  bool NonBlockingTimer::isModeFinPeriodic(){
    if(_timerMode == TimerModes::finPeriodic)
      return true;
    return false;
  }
  bool NonBlockingTimer::isRunning(){
    if(_timerState == TimerStates::running)
      return true;
    return false;
  }
  bool NonBlockingTimer::isExpired(){
    if(_timerState == TimerStates::expired)
      return true;
    return false;
  }
  bool NonBlockingTimer::isStopped(){
    if(_timerState == TimerStates::stopped)
      return true;
    return false;
  }
  void NonBlockingTimer::stop(){
    _timerState = TimerStates::stopped;
    _timeOutCntMax = 0;
  }
  void NonBlockingTimer::forceExpire(){
    _timerState = TimerStates::expired;
  }
  uint32_t NonBlockingTimer::elapsed(){
    if(_timerState == TimerStates::running)
      return static_cast<uint32_t>(_timeOutCnt) * static_cast<uint32_t>(TIMER1_PERIOD_MILLSEC);
    else
      return 0;
  }
  uint32_t NonBlockingTimer::remaining(){
    if(_timerMode != TimerModes::infRunning && _timerState == TimerStates::running){
      return static_cast<uint32_t>(_timeOutCntMax - _timeOutCnt) * static_cast<uint32_t>(TIMER1_PERIOD_MILLSEC);
    }
    else{
      return 0;
    }
  }
  uint32_t NonBlockingTimer::timeOutVal(){
    if(_timerMode != TimerModes::infRunning && _timerState != TimerStates::stopped)
      return static_cast<uint32_t>(_timeOutCntMax) * static_cast<uint32_t>(TIMER1_PERIOD_MILLSEC);
    else
      return 0;
  }
  uint8_t NonBlockingTimer::percentComplete(){ // max output is 200%
    if(_timerMode != TimerModes::infRunning && _timerState == TimerStates::running){
      if (_timeOutCnt >= 2*_timeOutCntMax)
        return 200;
      else
        return static_cast<uint8_t>((_timeOutCnt * 100) / _timeOutCntMax);
    }
    else{
      return 0;
    }
  }

// CircularCounter class implementations
  CircularCounter::CircularCounter(byte numCounts){
    _cntMax = numCounts-1;
    _enableCnt = false;
    _justIncr = false;
    count = 0;
  }
  void CircularCounter::enableCnt(){
    if(!_enableCnt){
      _enableCnt = true; count = _cntMax;
    }
  }
  void CircularCounter::incrCnt(){
    if(_enableCnt){
      _justIncr = true;
      if(count < _cntMax)
        count++;
      else
        count = 0;
    }
  }
  void CircularCounter::disableCnt(){
    _enableCnt = false;
  }
  bool CircularCounter::checkJustIncr(){
    if(_justIncr){
      _justIncr = false;
      return true;
    }
    else{
      return false;
    }
  }

// BinSemaphore class implementations
  BinSemaphore::BinSemaphore(){
    _status = false;
  }
  void BinSemaphore::give(){
    _status = true;
  }
  bool BinSemaphore::take(){
    if(_status){
      _status = false;
      return true;
    }
    else{
      return false;
    }
  }
// Button class implementations
  Button::Button(uint8_t buttonPin) {
    pin = buttonPin;
    pinMode(pin, INPUT_PULLUP);

    lastState = HIGH;
    currentState = HIGH;
    stableState = HIGH;

    lastChangeMillis = 0;
    currentScanMillis = 0;
    debounceTimeMilSec = 40;
  }

  bool Button::scanButton() {
    bool retVal = false;
      currentScanMillis = millis();
      currentState = digitalRead(pin);

      if (currentState != lastState) {
          lastChangeMillis = currentScanMillis;
          lastState = currentState;
      }
      if ((currentScanMillis - lastChangeMillis) >= debounceTimeMilSec) {
          if (stableState != currentState) {
              stableState = currentState;
              if (stableState == LOW)
                retVal = true;
          }
      }
      return retVal;
  }
