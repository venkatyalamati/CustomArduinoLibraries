#include "UtilityClasses.h"

// Note:- Do not use delay(), Serial.print() etc. inside the constructor
// Ticks class implementations
  Ticks::Ticks(unsigned long tikPeriodMilSec){
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
  NonBlockingTimer::NonBlockingTimer():_timeOutMillis(0),_startTime(0),_timerMode(TimerModes::infRunning),_timerState(TimerStates::stopped)
  {} // Using initializer list is better on Arduino (more efficient)
  void NonBlockingTimer::start(TimerModes timerMode, unsigned long timeOutMillis = 0){
    _timeOutMillis = timeOutMillis;
    _timerMode = timerMode;
    _timerState = TimerStates::running;
    _startTime = millis();
  }
  void NonBlockingTimer::update(){
    if(_timerState == TimerStates::running && _timerMode != TimerModes::infRunning){
      if((millis()-_startTime) >= _timeOutMillis){
        if(_timerMode == TimerModes::finPeriodic){
          _startTime = millis();  // auto reload
          _timerState = TimerStates::expired;
        }
        else{ // finite one shot mode
          _timerState = TimerStates::expired;
        }
      }
    }
  }
  bool NonBlockingTimer::event(){
    if(_timerState == TimerStates::expired){
      if(_timerMode == TimerModes::finOneShot){
        _timerState = TimerStates::stopped;
        _timeOutMillis = 0;
      }
      else{ // finite periodic mode
        _timerState = TimerStates::running;
      } 
      return true;
    }
    return false;
  }
  void NonBlockingTimer::restart(){
    _startTime = millis();
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
    _timeOutMillis = 0;
  }
  void NonBlockingTimer::forceExpire(){
    _timerState = TimerStates::expired;
  }
  unsigned long NonBlockingTimer::elapsed(){
    if(_timerState == TimerStates::running)
      return (millis() - _startTime);
    else
      return 0;
  }
  unsigned long NonBlockingTimer::remaining(){
    unsigned long e = elapsed();
    if(_timerMode != TimerModes::infRunning && _timerState == TimerStates::running){
      return (_timeOutMillis - e);
    }
    else{
      return 0;
    }
  }
  unsigned long NonBlockingTimer::timeOutVal(){
    if(_timerMode != TimerModes::infRunning && _timerState != TimerStates::stopped)
      return _timeOutMillis;
    else
      return 0;
  }
  uint8_t NonBlockingTimer::percentComplete(){ // max output is 200%
    unsigned long e = elapsed();
    if(_timerMode != TimerModes::infRunning && _timerState == TimerStates::running){
      if (e >= 2*_timeOutMillis)
        return 200;
      else
        return (uint8_t)((e * 100UL) / _timeOutMillis);
    }
    else{
      return 0;
    }
  }
  
  // void NonBlockingTimer::runTimer(){
  //   if(_timerIsOn){
  //     _timeOutCnt++;
  //     if(_timeOutCnt >= _timeOutCntMax){
  //       _timerIsOn = false;
  //       _timeOut = true;
  //     }
  //     else{
  //       _timeOut = false;
  //     }
  //   }
  // }

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
