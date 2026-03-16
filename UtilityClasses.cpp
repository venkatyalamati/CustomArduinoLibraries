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

// PreemptiveOnOff class implementations
  PreemptiveOnOff::PreemptiveOnOff(uint32_t cyclePeriodMillis, uint8_t dutCylPercent){
    _tikCntMax = cyclePeriodMillis/TIMER1_PERIOD_MILLSEC;
    _tikCntOn = static_cast<uint16_t>(static_cast<uint32_t>(dutCylPercent * _tikCntMax)/100);
    _enableActivity = false;
  }
  void PreemptiveOnOff::start(uint8_t maxRepeats){
    _cycleCntMax = maxRepeats;
    _cycleCnt = 0; _tikCnt = 0;
    _enableActivity = true; _onActDone = false; _offActDone = false;
  }
  void PreemptiveOnOff::executor(void (*task_On_Ptr)(), void (*task_Off_Ptr)(), void (*task_Final_Ptr)()){
    if(_enableActivity){
      _tikCnt++;
      if(!_onActDone){
        if(task_On_Ptr != nullptr) task_On_Ptr(); _onActDone = true;
      }
      else if(_tikCnt >= _tikCntOn){
        if(!_offActDone){
          if(task_Off_Ptr != nullptr) task_Off_Ptr(); _offActDone = true; _cycleCnt++;
        }
      }

      if(_cycleCnt >= _cycleCntMax){
        _enableActivity = false;
        if(task_Final_Ptr != nullptr) task_Final_Ptr();
      }
        
      if(_tikCnt >= _tikCntMax)
        _tikCnt = 0;
    }
  }
  bool PreemptiveOnOff::isRunning(){
    return _enableActivity;
  }
  void PreemptiveOnOff::stop(){
    _enableActivity = false;
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
// ButtonMillisBased class implementations
  ButtonMillisBased::ButtonMillisBased(uint8_t pin) {
    this->pin = pin;
    pinMode(pin, INPUT_PULLUP);

    lastState = HIGH;
    currentState = HIGH;
    stableState = HIGH;

    lastChangeMillis = 0;
    debounceTimeMilSec = 40;
  }

  bool ButtonMillisBased::scanButton() {
    bool retVal = false;
    unsigned long currentScanMillis = millis();

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

// ButtonTimer2Based class implementations
  ButtonTimer2Based::ButtonTimer2Based(uint8_t pin) {
    _pin = pin;
    
    _lastState = HIGH;
    _currentState = HIGH;
    _stableState = HIGH;

    _lastChangeTick = 0;
  }
  void ButtonTimer2Based::setPinMode(){
    pinMode(_pin, INPUT_PULLUP);
  }
  void ButtonTimer2Based::scanButton() {
    bool retVal = false;
      _currentState = digitalRead(_pin);

      if (_currentState != _lastState) {
          _lastChangeTick = timer2_isr_tick;
          _lastState = _currentState;
      }
      if ((timer2_isr_tick - _lastChangeTick) >= debounceTicks) {
          if (_stableState != _currentState) {
              _stableState = _currentState;
              if (_stableState == LOW)
                retVal = true;
          }
      }
      if(retVal)
        pinButtPressed = _pin;
  }
  // static keyword only in declaration inside class — never repeat it in definition outside class.
  void ButtonTimer2Based::setDebNormPress(){
    debounceTicks = 40/TIMER2_PERIOD_MILLSEC; // for 40ms debounce time
  }
  void ButtonTimer2Based::setDebLongPress(){
    debounceTicks = 5000/TIMER2_PERIOD_MILLSEC; // for 5s debounce time
  }

// LevelSensor class implementations
  LevelSensor::LevelSensor(uint8_t sensorPin, int16_t addrEEPROM){
    addrEmptyVal = addrEEPROM;
    addrFullVal = addrEEPROM + 2;
    this->sensorPin = sensorPin;
  }
  void LevelSensor::storeDfltCalParameters(){
    EEPROM.put(addrEmptyVal, 200); delay(10);
    EEPROM.put(addrFullVal, 900); delay(10);
  }
  void LevelSensor::loadCalParameters() {
    EEPROM.get(addrEmptyVal, emptyMarkVal);
    EEPROM.get(addrFullVal, fullMarkVal);
  }
  // Save current sensor value as 0% (empty)
  void LevelSensor::calibrateEmpty() {
    emptyMarkVal = sensorRead();
    EEPROM.put(addrEmptyVal, emptyMarkVal); delay(10);
  }
  // Save current sensor value as 100% (full)
  void LevelSensor::calibrateFull() {
    fullMarkVal = sensorRead();
    EEPROM.put(addrFullVal, fullMarkVal); delay(10);
  }
  // Calculate tank level in percent
  float LevelSensor::getTankLevelPercent() {
    int currSensorVal=0; float levelPercentage=0.0;
    currSensorVal = sensorRead();
    levelPercentage = (currSensorVal - emptyMarkVal) * 100.0 / (fullMarkVal - emptyMarkVal); // map function simplified for this case
    return levelPercentage;
  }
  int16_t LevelSensor::sensorRead(){
    #define numSamples 6
    int16_t sensorVal; uint16_t sumSensorVal;
    sensorVal = analogRead(sensorPin); // simply discard
    sumSensorVal = 0;
    for(uint8_t i=0; i<numSamples; i++){
      delay(200);
      sensorVal = analogRead(sensorPin);
      sumSensorVal += sensorVal;
    }
    sumSensorVal /= numSamples;
    sensorVal = sumSensorVal;
    return sensorVal;
  }
  // levelPercentage = floatMap(currSensorVal, emptyMarkVal, fullMarkVal, 0, 100);
  // float LevelSensor::floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  //   float retVal=0;
  //   retVal = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  //   return retVal;
  // }

// ShiftRegisterController class implementations
  ShiftRegisterController::ShiftRegisterController(uint8_t OE_Pin, uint8_t latchPin, uint8_t dataPin, uint8_t clkPin) {
    _OE_Pin = OE_Pin; _latchPin = latchPin; _dataPin = dataPin; _clkPin = clkPin;
    _outputEnabled = false;
  }
  void ShiftRegisterController::doStartUpActions(){
    pinMode(_OE_Pin, OUTPUT); enableOutput();
    pinMode(_latchPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);
    pinMode(_clkPin, OUTPUT);
  }
  void ShiftRegisterController::updateOutputs(const uint8_t DO_StatusArr[]){
    _dataByteDO = 0;
    for (uint8_t i = 0; i < 8; i++){
      if (DO_StatusArr[i] == 1){
        _dataByteDO |= (1 << i);
      }
    }
    
    digitalWrite(_latchPin, LOW);
    shiftOut(_dataPin, _clkPin, MSBFIRST, _dataByteDO);
    digitalWrite(_latchPin, HIGH);
  }
  void ShiftRegisterController::disableOutput(){
    if(_outputEnabled){
      digitalWrite(_OE_Pin, HIGH); // _OE_Pin is active low
      _outputEnabled = false;
    }
  }
  void ShiftRegisterController::enableOutput(){
    if(!_outputEnabled){
      digitalWrite(_OE_Pin, LOW); // _OE_Pin is active low
      _outputEnabled = true;
    }
  } 
  
