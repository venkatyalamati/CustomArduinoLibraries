 #include "UtilityFunctions.h"
// -------------- Timer related functions ---------------
  // ─── Timer1 Setup (initializes but leaves disabled) ──────────────────────────
  // Period range: 1ms to 4194ms (prescaler fixed at 1024, 16MHz)
  void setupTimer1() {
    /*
    Timer1 = 16-bit → max OCR1A = 65535
    At 16MHz, prescaler 1024:
    tick = 64µs
    Max period = 65536 × 64µs = 4,194,304µs = 4194ms ≈ 4.194 seconds
    OCR1A = (TIMER1_PERIOD_MILLSEC * 1000 / 64) - 1
          = (TIMER1_PERIOD_MILLSEC * 125 / 8) - 1
    */
      uint16_t ocr = (uint16_t)(((uint32_t)TIMER1_PERIOD_MILLSEC * 125u) / 8u) - 1u;
      uint8_t savedSREG = SREG;
      cli();
      TCCR1A = 0;
      TCCR1B = 0;        // prescaler bits cleared — timer stopped
      TCNT1  = 0;        // clear counter while stopped
      OCR1A  = ocr;
      TCCR1B |= (1 << WGM12);   // CTC mode
      // ── No prescaler bits written — timer remains stopped ──
      TIMSK1 &= ~(1 << OCIE1A); // disable compare match interrupt
      SREG = savedSREG; //sei();
  }

  // ─── Timer1 Enable ────────────────────────────────────────────────────────────
  void enableTimer1() {
      uint8_t savedSREG = SREG;
      cli();
      TCNT1  = 0;                                                // reset counter before starting
      TCCR1B |= (1 << CS12) | (1 << CS10);                      // prescaler 1024 — timer starts
      TIMSK1 |= (1 << OCIE1A);                                  // enable compare match interrupt
      SREG = savedSREG; //sei();
  }

  // ─── Timer1 Disable ───────────────────────────────────────────────────────────
  void disableTimer1() {
      uint8_t savedSREG = SREG;
      cli();
      TCCR1B &= ~((1 << CS12) | (1 << CS10));                   // clear prescaler — timer stops
      TIMSK1 &= ~(1 << OCIE1A);                                 // disable compare match interrupt
      TCNT1   = 0;                                              // clear counter while stopped
      SREG = savedSREG; //sei();
  }

  // ─── Timer2 Setup (initializes but leaves disabled) ─────────────────────────
  // Period range: 1ms to 16ms (prescaler fixed at 1024, 16MHz)
  // TIMER2_PERIOD_MILLSEC: uint8_t — whole milliseconds only (1 to 16)
  void setupTimer2() {
    // At 16MHz, prescaler 1024:
    // tick = 64µs
    // OCR2A = (TIMER2_PERIOD_MILLSEC * 1000µs / 64µs) - 1
    //       = (TIMER2_PERIOD_MILLSEC * 15.625) - 1
    // integer approximation: 1000/64 = 125/8
    // so: OCR2A = (TIMER2_PERIOD_MILLSEC * 125 / 8) - 1
      uint8_t ocr = (uint8_t)(((uint16_t)TIMER2_PERIOD_MILLSEC * 125u) / 8u) - 1u;
      uint8_t savedSREG = SREG;
      cli();
      TCCR2A = 0;
      TCCR2B = 0;        // prescaler bits cleared — timer stopped
      TCNT2  = 0;        // clear counter while stopped
      OCR2A  = ocr;
      TCCR2A |= (1 << WGM21);   // CTC mode
      // ── No prescaler bits written — timer remains stopped ──
      TIMSK2 &= ~(1 << OCIE2A); // disable compare match interrupt
      SREG = savedSREG; //sei();
  }

// ─── Timer2 Enable ────────────────────────────────────────────────────────────
void enableTimer2() {
    uint8_t savedSREG = SREG;
    cli();
    TCNT2  = 0;                                                // reset counter before starting
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);       // prescaler 1024 — timer starts
    TIMSK2 |= (1 << OCIE2A);                                  // enable compare match interrupt
    SREG = savedSREG; //sei();
}

// ─── Timer2 Disable ───────────────────────────────────────────────────────────
void disableTimer2() {
    uint8_t savedSREG = SREG;
    cli();
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));    // clear prescaler — timer stops
    TIMSK2 &= ~(1 << OCIE2A);                                 // disable compare match interrupt
    TCNT2   = 0;                                              // clear counter while stopped
    SREG = savedSREG; //sei();
}

  // -------------- EEPROM related functions ---------------
  bool erase_eeprom_if_req(int addrValStr){
    int checkResult = 0;
    char validationString[12]; char checkString[12];
    
    validationString[11]='\0';
    
    strcpy_P(validationString, (PGM_P)F("DATAISVALID"));
    
    for(int j = 0; j < 11; j++)
      checkString[j] = ' ';
    checkString[11] = '\0';

    for(int j = 0; j < 11; j++) {   
      checkString[j] = EEPROM.read(addrValStr + j);
      delay(1);
    }

    checkResult = strcmp(validationString, checkString);

    if(checkResult!=0){ // The strings are NOT equal, hence internal eeprom needs to be erased
      for(int j = 0; j < 1024; j++){ 
        EEPROM.write(j, 0);
        delay(10);
      }
      for(int j = 0; j < 11; j++) { // put validation string on internal eeprom to avoid erase on every startup
        EEPROM.write((addrValStr + j), validationString[j]);
        delay(10);
      }
      return true;
    }
    else{
      return false;
    }
  } // erase_eeprom_if_req ends

  #define SIZE_TEMP_STR 7 // must be > fullDcmlLen
  void insertFloatVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, uint8_t numDcmlPts, float dcmlVal, bool dispPosSign){
  //Note:- To get startPos, count 1,2,3.. & fullDcmlLen includes '.' and '-' signs also
    char tempStr[SIZE_TEMP_STR];
    for(uint8_t i=0; i<SIZE_TEMP_STR; i++)
      tempStr[i] = '\0';

    dtostrf(dcmlVal, fullDcmlLen, numDcmlPts, tempStr);
    if(dispPosSign) tempStr[0] = '+';

    for(uint8_t i=0; i<fullDcmlLen; i++)
      charArr[i+startPos-1] = tempStr[i];
  }

  void insertUintVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, uint32_t dcmlVal){
  //Note:- To get startPos, count 1,2,3..
    char tempStr[SIZE_TEMP_STR];
    for(uint8_t i=0; i<SIZE_TEMP_STR; i++)
      tempStr[i] = '\0';

    for(uint8_t i=fullDcmlLen; i>0; i--){
      tempStr[i-1] = dcmlVal % 10; dcmlVal /= 10;
      tempStr[i-1] += 48; // To ASCII
    }

    for(uint8_t i=0; i<fullDcmlLen; i++)
      charArr[i+startPos-1] = tempStr[i];
  }

  void insertIntVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, int32_t dcmlVal, bool dispPosSign){
  //Note:- To get startPos, count 1,2,3.. & fullDcmlLen includes '-' signs also
    char tempStr[SIZE_TEMP_STR]; bool isPositive;
    for(uint8_t i=0; i<SIZE_TEMP_STR; i++)
      tempStr[i] = '\0';

    if(dcmlVal > 0){
      isPositive = true;
    }
    else{
      dcmlVal = -dcmlVal;
      isPositive = false;
    }

    for(uint8_t i=fullDcmlLen; i>0; i--){
      tempStr[i] = dcmlVal % 10; dcmlVal /= 10;
      tempStr[i] += 48; // To ASCII
    }

    if(isPositive){
      tempStr[0] = (dispPosSign) ? '+' : ' ';
    }
    else{
      tempStr[0] = '-';
    }

    for(uint8_t i=0; i<fullDcmlLen; i++)
      charArr[i+startPos-1] = tempStr[i];
  }

  float floatFromCharArray(const char *str){
    float result = 0;
    float decimalPlace = 0.1;
    bool isNegative = false;
    bool decimalPart = false;

    // Skip leading spaces
    while(*str == ' '){
      str++;
    }

    if(*str == '-'){
      isNegative = true;
      str++;
    }

    while(*str != '\0'){
      if (*str == '.'){
        decimalPart = true;
      }
      else if(*str >= '0' && *str <= '9'){
        if(!decimalPart){
          result = result * 10 + (*str - '0');
        }
        else{
          result += (*str - '0') * decimalPlace;
          decimalPlace *= 0.1;
        }
      }
      else{
        // Stop at first invalid character
        break;
      }
      str++;
    } // while loop

    return isNegative ? -result : result; // will work perfectly
  }