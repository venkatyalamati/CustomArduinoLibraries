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
  NonBlockingTimer::NonBlockingTimer():_timerIsOn(false),_forceTimeOut(false),_timeOutMillis(0),_startTime(0)
  {} // Using initializer list is better on Arduino (more efficient)
  void NonBlockingTimer::startTimer(unsigned long timeOutMillis = INFINITE){
    _timeOutMillis = timeOutMillis;
    _timerIsOn = true;
    _startTime = millis();
  }
  bool NonBlockingTimer::checkTimeOut(bool autoReload){
    if(_forceTimeOut){
      if(autoReload){
        _timerIsOn = true;
        _startTime = millis();
      }
      else{
        _timerIsOn = false;
        _timeOutMillis = 0;
      }
      _forceTimeOut = false;
      return true;
    }
    else if(_timerIsOn && _timeOutMillis != INFINITE){
      if((millis()-_startTime) >= _timeOutMillis){
        if(autoReload){
          _timerIsOn = true;
          _startTime = millis();
        }
        else{
          _timerIsOn = false;
          _timeOutMillis = 0;
        }
        return true;
      }
      return false;
    }
    else{
      return false;
    }
  }
  bool NonBlockingTimer::isTimerRunning(){
    return _timerIsOn;
  }
  void NonBlockingTimer::stopTimer(){
    _timerIsOn = false;
    _timeOutMillis = 0;
  }
  void NonBlockingTimer::forceTimeOut(){
    _forceTimeOut = true;
  }
  bool NonBlockingTimer::isTimeElapsed(unsigned long checkTimeMillis){
    if( _timerIsOn && ((millis()-_startTime) >= checkTimeMillis) ){
      return true;
    }
    else{
      return false;
    }
  }
  unsigned long NonBlockingTimer::elapsedMillis(){
    if(_timerIsOn)
      return (millis() - _startTime);
    else
      return 0;
  }
  unsigned long NonBlockingTimer::millisRemaining(){
    unsigned long e = elapsedMillis();
    if (_timerIsOn) {
      return (e >= _timeOutMillis) ? 0 : (_timeOutMillis - e); // inherently checks that _timeOutMillis != INFINITE
    }
    else{
      return 0;
    }
  }
  uint8_t NonBlockingTimer::percentComplete(){
    unsigned long e = elapsedMillis();
    if(_timerIsOn && (_timeOutMillis != INFINITE)){
      if (e >= _timeOutMillis)
        return 100;
      else
        return (uint8_t)((e * 100UL) / _timeOutMillis);
    }
    else{
      return 0;
    }
  }
  unsigned long NonBlockingTimer::timeOutMillis(){
    if(_timerIsOn)
      return _timeOutMillis;
    else
      return 0;
  }
  void NonBlockingTimer::restartTimer(){
    _startTime = millis();
    _timerIsOn = true;
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
