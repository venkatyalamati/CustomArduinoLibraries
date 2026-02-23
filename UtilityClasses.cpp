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
  
// TimeOutCnt class implementations
  ActUponTimeOut::ActUponTimeOut(){
    _timerIsOn = false; _forceTimeOut = false;
  }
  void ActUponTimeOut::startTimer(unsigned long timeOutMilSec){
    _timeOutMilSec = timeOutMilSec;
    _timerIsOn = true;
    _startTime = millis();
  }
  // void ActUponTimeOut::runTimer(){
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
  bool ActUponTimeOut::checkTimeOut(){
    if(_timerIsOn){
      if((millis()-_startTime) > _timeOutMilSec){
        _timerIsOn = false;
        return true;
      }
    }
    else if(_forceTimeOut){
      _forceTimeOut = false;
      return true;
    }
    else{
      return false;
    }
  }
  bool ActUponTimeOut::isTimerRunning(){
    if(_timerIsOn)
      return true;
    else
      return false;
  }
  void ActUponTimeOut::stopTimer(){
    _timerIsOn = false;
  }
  void ActUponTimeOut::forceTimeOut(){
    _timerIsOn = false;
    _forceTimeOut = true;
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

// CheckTimeElapsed class implementations
  CheckTimeElapsed::CheckTimeElapsed(){
    _startTime = 0;
  }
  void CheckTimeElapsed::startTimer(){
    _startTime = millis();
  }
  bool CheckTimeElapsed::isTimeElapsed(unsigned long checkTimeDur){
    if((millis()-_startTime) > checkTimeDur){
      return true;
    }
    else{
      return false;
    }
  }
  unsigned long CheckTimeElapsed::getTimeElapsedMilSec(){
    return (millis() - _startTime);
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
