 #include "UtilityFunctions.h"
 
// -------------- Timer related functions ---------------
  void setupTimer1(){
    unsigned long OCR1A_Value;
    // must be < 65536 -> '15625' is for ISR interval of 1000ms @16MHz 
    OCR1A_Value = ((16000000UL / 1024) * (TIMER1_PERIOD_MILLSEC / 1000.0)) - 1; // for 16.0 MHz
    //set timer1 interrupt
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register increments
    OCR1A = OCR1A_Value;
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR1B |= (1 << CS12) | (1 << CS10);  
    TCNT1  = 0;//initialize counter value to 0
    TIMSK1 |= (1 << OCIE1A); // enable timer1 compare interrupt
  }

  void enableTimer1_Int(){
    TCNT1  = 0; //initialize counter value to 0
    TIMSK1 |= (1 << OCIE1A); // enable timer1 compare interrupt
  }

  void disableTimer1_Int(){
    TIMSK1 &= ~(1 << OCIE1A); // disable timer1 interrupt
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