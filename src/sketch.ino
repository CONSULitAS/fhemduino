/*
 * EOF preprocessor bug prevent
*/
/// @dir FHEMduino (2013-11-07)
/// FHEMduino communicator
//
// authors: mdorenka + jowiemann + sidey, mick6300
// see http://forum.fhem.de/index.php/topic,17196.0.html
//
// History of changes:
// 2013-11-07 - started working on PCA 301
// 2013-12-06 - second version
// 2013-12-15 - KW9010ISR
// 2013-12-15 - fixed a bug where readings did not get submitted due to wrong error
// 2013-12-18 - major upgrade to new receiver system (might be extendable to other protocols
// 2013-12-18 - fixed typo that prevented compilation of code
// 2014-02-27 - remove magic numbers for IO Pins, #define constants instead / Make the code Leonardo compatible / This has not been tested on a atmega328 (all my devices are atmega32u4 based)
// 2014-04-09 - add support for pearl NC7159 Code from http://forum.fhem.de/index.php/topic,17196.msg147168.html#msg147168 Sensor from: http://www.pearl.de/a-NC7159-3041.shtml
// 2014-06-04 - add support for EUROCHRON modified Code from http://forum.arduino.cc/index.php/topic,136836.0.html / Prevent receive the same message continius
// 2014-06-11 - add support for LogiLink NC_WS
// 2014-06-13 - EUROCHRON bugfix for neg temp 
// 2014-06-16 - Added TX70DTH (Aldi)
// 2014-06-16 - Added DCF77 ( http://www.arduinoclub.de/2013/11/15/dcf77-dcf1-arduino-pollin/)
// 2014-06-18 - Two loops for different duration timings
// 2014-06-21 - Added basic Support to dectect the follwoing codecs: Oregon Scientific v2, Oregon Scientific v3,Cresta,Kaku,XRF,Home Easy
//            - Implemented Decoding for OSV2 Protocol
//            - Added some compiler switches for DCF-77, but they are currently not working
//            - Optimized duration calculation and saved variable 'time'.
// 2014-06-22 - Added Compiler Switch for __AVR_ATmega32U4__ DCF Pin#2
// 2014-06-24 - Capsulatet all decoder with #defines
// 2014-06-24 - Added send / activate support for smoke detectors FA20RF / RM150RF / Brennenstuhl BR 102-F / (KD101 not verified)
// 2014-06-24 - Integrated mick6300 developments LIFETEC support
// 2014-06-24 - Integrated mick6300 developments TX70DTH (Aldi) support
// 2014-07-04 - Integrated Intertechno TX2/3/4 support, see CUL_TX and http://www.f6fbb.org/domo/sensors/tx3_th.php
// 2014-07-15 - Changed Stop-Bit interpretaion for smoke detectors FA20RF / RM150RF / Brennenstuhl BR 102-F / KD101
// 2014-07-16 - Return value for Lifetec changed to distribute to 14_FEHMduino_Lifetec.pm
// 2014-07-16 - Added Tchibo TCM234759 door bell
// 2014-07-17 - Added  * Heidemann HX Pocket (70283). May work also with other Heidemann HX series door bells
// 2014-07-31 - Fixed bug in KW9010 / crcValid not needed any more
// 2014-08-05 - Added temperature sensor AURIOL (Lidl Version: 09/2013)
// 2014-08-06 - Implemented uptime
// 2014-08-08 - Started outsourcing of devices in modules

// --- Configuration ---------------------------------------------------------
#define PROGNAME               "FHEMduino"
#define PROGVERS               "2.2d"

#if defined(__AVR_ATmega32U4__)          //on the leonardo and other ATmega32U4 devices interrupt 0 is on dpin 3
#define PIN_RECEIVE            3
#else
#define PIN_RECEIVE            2
#endif

#define PIN_LED                13

//#define DEBUG           // Compile sketch witdh Debug informations
#ifdef DEBUG
#define BAUDRATE               115200
#else
#define BAUDRATE               9600
#endif

#define COMP_DCF77      // Compile sketch with DCF-77 Support (currently disableling this is not working, has still to be done)

#define COMP_PT2262     // Compile sketch with PT2262 (IT / ELRO switches)

#define COMP_DOORBELL   // Compile sketch with door bell support: Tchibo / Heidemann HX Pocket (70283)

#define COMP_FA20RF     // Compile sketch with smoke detector Flamingo FA20RF / ELRO RM150RF

#define COMP_KW9010     // Compile sketch with KW9010 support
#define COMP_NC_WS      // Compile sketch with PEARL NC7159, LogiLink WS0002 support
#define COMP_EUROCHRON  // Compile sketch with EUROCHRON / Tchibo support
#define COMP_LIFETEC    // Compile sketch with LIFETEC support
#define COMP_TX70DTH    // Compile sketch with TX70DTH (Aldi) support
#define COMP_AURIOL     // Compile sketch with AURIOL (Lidl Version: 09/2013); only temperature
#define COMP_IT_TX      // Compile sketch with Intertechno TX2/3/4 support
#define USE_IT_TX       // Use 14_CUL_TX.pm Module which is already included in fhem. If not defined, the 14_fhemduino_Env module will be used.

#define COMP_OSV2       // Compile sketch with OSV2 Support
//#define COMP_Cresta     // Compile sketch with Cresta Support (currently not implemented, just for future use)
#define USE_OREGON_41   // Use oregon_41 Module which is already included in fhem. If not defined, the 14_fhemduino_oregon module will be used.

// Future enhancement
//#define COMP_OSV3     // Compile sketch with OSV3 Support (currently not implemented, just for future use)
//#define COMP_Kaku     // Compile sketch with Kaku  Support (currently not implemented, just for future use)
//#define COMP_HEZ      // Compile sketch with Homeeasy Support (currently not implemented, just for future use)
//#define COMP_XRF      // Compile sketch with XTF Support (currently not implemented, just for future use)


/*
 * Modified code to fit info fhemduino - Sidey
 * Oregon V2 decoder modfied - Olivier Lebrun
 * Oregon V2 decoder added - Dominique Pierre
 * New code to decode OOK signals from weather sensors, etc.
 * 2010-04-11 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
 *
*/

/*-----------------------------------------------------------------------------------------------
/* Devices with sending / receiving functionality => PT2262
-----------------------------------------------------------------------------------------------*/
#ifdef COMP_PT2262
  #include "PT2262.h"
#endif

/*-----------------------------------------------------------------------------------------------
/* door bell support: Tchibo / Heidemann HX Pocket (70283)
-----------------------------------------------------------------------------------------------*/
#ifdef COMP_DOORBELL
  #include "doorbell.h"
#endif

/*-----------------------------------------------------------------------------------------------
/* Smoke dector FA20RF
-----------------------------------------------------------------------------------------------*/
#ifdef COMP_FA20RF
  #include "FA20RF.h"
#endif

/*-----------------------------------------------------------------------------------------------
/* include oregon class
-----------------------------------------------------------------------------------------------*/
#include "decoders.h";

#ifdef COMP_OSV2     // Compile sketch with OSV3 Support (currently not implemented, just for future use)
OregonDecoderV2 orscV2;
#endif

#ifdef COMP_Cresta     // Compile sketch with Cresta Support (currently not implemened, just for future use)
CrestaDecoder cres;
#endif

#ifdef COMP_Kaku     // Compile sketch with Kaku  Support (currently not implemented, just for future use)
KakuDecoder kaku;
#endif

#ifdef COMP_HEZ     // Compile sketch with Homeeasy Support (currently not implemented, just for future use)
HezDecoder hez;
#endif

#ifdef COMP_XRF
XrfDecoder xrf;
#endif

/*-----------------------------------------------------------------------------------------------
/* Globals for message handling
-----------------------------------------------------------------------------------------------*/
String cmdstring;
volatile bool available = false;
String message = "";

/*-----------------------------------------------------------------------------------------------
/* Globals for bitstream handling
-----------------------------------------------------------------------------------------------*/
#define MAX_CHANGES            90
unsigned int timings[MAX_CHANGES];
unsigned int timings2500[MAX_CHANGES];

/*-----------------------------------------------------------------------------------------------
/* DCF77 stuff
-----------------------------------------------------------------------------------------------*/
/*
 * DCF77_SerialTimeOutput
 * Ralf Bohnen, 2013
 * This example code is in the public domain.
 */
#ifdef COMP_DCF77
#include "Time.h"        // Unterstuetzung für Datum/Zeit-Funktionen
#include "DCF77.h"
 
char time_s[9];
char date_s[11];
 
#if defined(__AVR_ATmega32U4__)          //on the leonardo and other ATmega32U4 devices interrupt 1 is on dpin 2
  #define DCF_PIN 2            // Connection pin to DCF 77 device
#else
  #define DCF_PIN 3            // Connection pin to DCF 77 device
#endif

#define DCF_INTERRUPT 1      // Interrupt number associated with pin
 
time_t time;
DCF77 DCF = DCF77(DCF_PIN, DCF_INTERRUPT);

char* sprintTime() {
    snprintf(time_s,sizeof(time_s),"%02d%02d%02d" , hour(), minute(), second());
    time_s[strlen(time_s)] = '\0';
    return time_s;
}
 
char* sprintDate() {
    snprintf(date_s,sizeof(date_s),"%02d%02d%04d" , day(), month(), year());
    date_s[strlen(date_s)] = '\0';
    return date_s;
}
#endif

/*-----------------------------------------------------------------------------------------------
/* Initializing aof Arduino
-----------------------------------------------------------------------------------------------*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUDRATE);
  enableReceive();
  pinMode(PIN_RECEIVE,INPUT);
  pinMode(PIN_SEND,OUTPUT);

#ifdef DEBUG
    delay(3000);
    Serial.println(" -------------------------------------- ");
    Serial.print("    ");
    Serial.print(PROGNAME);
    Serial.print(" ");
    Serial.println(PROGVERS);
    Serial.print(" "); 
    Serial.print("Free Ram: "); 
    Serial.println(freeRam()); 
    Serial.println(" -------------------------------------- ");
#endif

#ifdef COMP_DCF77
    DCF.Start();

#ifdef DEBUG
    Serial.println("Warte auf Zeitsignal ... ");
    Serial.println("Dies kann 2 oder mehr Minuten dauern.");
#endif

#endif // COMP_DCF77

}

/*-----------------------------------------------------------------------------------------------
/* Main loop
-----------------------------------------------------------------------------------------------*/
void loop() {

  // put your main code here, to run repeatedly: 

  if (messageAvailable()) {
    Serial.println(message);
    resetAvailable();
  }

#ifdef COMP_DCF77
    time_t DCFtime = DCF.getTime(); // Nachschauen ob eine neue DCF77 Zeit vorhanden ist
    if (DCFtime!=0)
    {
      setTime(DCFtime); //Neue Systemzeit setzen
      // Serial.print("Neue Zeit erhalten : "); //Ausgabe an seriell
      Serial.print("D"); 
      Serial.print(sprintTime()); 
      Serial.print("-"); 
      Serial.println(sprintDate());   
    }
#endif

//serialEvent does not work on ATmega32U4 devices like the Leonardo, so we do the handling ourselves
#if defined(__AVR_ATmega32U4__)
  if (Serial.available()) {
    serialEvent();
  }
#endif
}

/*-----------------------------------------------------------------------------------------------
/* Interrupt system
-----------------------------------------------------------------------------------------------*/

void enableReceive() {
  attachInterrupt(0,handleInterrupt,CHANGE);
}

void disableReceive() {
  detachInterrupt(0);
}

void handleInterrupt() {
  static unsigned int duration;
  static unsigned long lastTime;

  duration = micros() - lastTime;
  
#ifdef COMP_FA20RF
  FA20RF(duration);
#endif
#ifdef COMP_IT_TX
  IT_TX(duration);
#endif

  decoders(duration);
  decoders2500(duration);

#ifdef COMP_OSV2
  COMP_OSV2_HANDLER (duration);
#endif

#ifdef COMP_Cresta
  COMP_Cresta_HANDLER (duration);
#endif

  lastTime += duration;
}

/*
 * call Oregon decoder when valid timings sequence available
 */
#ifdef COMP_OSV2
void COMP_OSV2_HANDLER (unsigned int duration) {
  if (orscV2.nextPulse(duration) )
  {
    uint8_t len;
    const byte* data = orscV2.getData(len);

    char tmp[36]="";
    char len_field[2]="";
    uint8_t tmp_len = 0;
    //strcat(tmp, "OSV2:");
    //tmp_len = 5;
#ifdef DEBUG
    Serial.print("HEXStream");
#endif

    for (byte i = 0; i < len; ++i) {
#ifdef DEBUG
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0x0F, HEX);
        Serial.print(",");
#endif
      tmp_len += snprintf(tmp + tmp_len, 36, "%02X", data[i]);
    }

#ifdef DEBUG
    Serial.println(" ");
    Serial.println("Length:");
#endif


#ifdef USE_OREGON_41
    message=(String(len*8, HEX));
    message.concat(tmp);
//    message.concat("\n");
//    Dirty hack, just to test the 41_Oregon Module
//    Serial.println(" ");
//    Serial.print(len*8, HEX);
//    Serial.println(tmp);
#else
   message.concat("OSV2:");
   message.concat(tmp);
#endif
   orscV2.resetDecoder();

   available = true;
  }
}
#endif

#ifdef COMP_Cresta
void COMP_Cresta_HANDLER (unsigned int duration) {
  if (cres.nextPulse(duration))
  {
    byte len;
    const byte* data = orscV2.getData(len) + 5;
    char tmp[36]="";
    uint8_t tmp_len = 0;
    strcat(tmp, "CRESTA:");
    tmp_len = 7;

#ifdef DEBUG
      Serial.print("HEXStream");
#endif

    for (byte i = 0; i < len; ++i) {
#ifdef DEBUG
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0x0F, HEX);
        Serial.print(",");
#endif
      tmp_len += snprintf(tmp + tmp_len, 36, "%02X", data[i]);
    }
    
#ifdef DEBUG
      Serial.println(" ");
      Serial.print("Length:");
      Serial.println(len,HEX);
#endif

    message = tmp;
    available = true;
  }
  cres.resetDecoder();
}
#endif

/*-----------------------------------------------------------------------------------------------
/* Generation bitstreams with sync bits greater than 5000 us
-----------------------------------------------------------------------------------------------*/
void decoders(unsigned int duration) {
#define STARTBIT_TIME   5000
#define STARTBIT_OFFSET 200

  static unsigned int changeCount;
  static unsigned int repeatCount;
  bool rc = false;

  if (duration > STARTBIT_TIME && duration > timings[0] - STARTBIT_OFFSET && duration < timings[0] + STARTBIT_OFFSET) {
    repeatCount++;
    changeCount--;
    if (repeatCount == 2) {
      // update uptime. Could be every where, but still put here
      uptime(millis(), false);

#ifdef COMP_KW9010
      if (rc == false) {
        rc = receiveProtocolKW9010(changeCount);
      }
#endif
#ifdef COMP_NC_WS
      if (rc == false) {
        rc = receiveProtocolNC_WS(changeCount);
      }
#endif
#ifdef COMP_EUROCHRON
      if (rc == false) {
        rc = receiveProtocolEuroChron(changeCount);
      }
#endif
#ifdef COMP_PT2262
      if (rc == false) {
        rc = receiveProtocolPT2262(changeCount);
      }
#endif
#ifdef COMP_LIFETEC
      if (rc == false) {
        rc = receiveProtocolLIFETEC(changeCount);
      }
#endif
#ifdef COMP_DOORBELL
      if (rc == false) {
        rc = receiveProtocolTCM(changeCount);
      }
      if (rc == false) {
        rc = receiveProtocolHX(changeCount);
      }
#endif
#ifdef COMP_AURIOL
      if (rc == false) {
        rc = receiveProtocolAURIOL(changeCount);
      }
#endif
      if (rc == false) {
        // rc = next decoder;
      }
      repeatCount = 0;
    }
    changeCount = 0;
  } 
  else if (duration > STARTBIT_TIME) {
    changeCount = 0;
  }

  if (changeCount >= MAX_CHANGES) {
    changeCount = 0;
    repeatCount = 0;
  }
  timings[changeCount++] = duration;
}

/*-----------------------------------------------------------------------------------------------
/* Generation bitstreams with sync bits greater than 2500 us and less than 5000 us
-----------------------------------------------------------------------------------------------*/
void decoders2500(unsigned int duration) {
#define LOW_STARTBIT_TIME  2500
#define HIGH_STARTBIT_TIME 2500
#define STARTBIT_OFFSET 200

  static unsigned int changeCount;
  static unsigned int repeatCount;
  bool rc = false;

  if ((duration > LOW_STARTBIT_TIME && duration < HIGH_STARTBIT_TIME) && duration > timings2500[0] - STARTBIT_OFFSET && duration < timings2500[0] + STARTBIT_OFFSET) {
    repeatCount++;
    changeCount--;
    if (repeatCount == 2) {
#ifdef COMP_TX70DTH
      if (rc == false) {
        rc = receiveProtocolTX70DTH(changeCount);
      }
#endif
      if (rc == false) {
        // rc = next decoder;
      }
      repeatCount = 0;
    }
    changeCount = 0;
  } 
  else if (duration > STARTBIT_TIME) {
    changeCount = 0;
  }

  if (changeCount >= MAX_CHANGES) {
    changeCount = 0;
    repeatCount = 0;
  }
  timings2500[changeCount++] = duration;
}

/*-----------------------------------------------------------------------------------------------
/* Devices with temperatur / humidity functionality
-----------------------------------------------------------------------------------------------*/

#ifdef COMP_IT_TX
/*
 * TX Receiver
 */
#define TX_MAX_CHANGES 88
unsigned int timingsTX[TX_MAX_CHANGES+20];      //  TX

// http://www.f6fbb.org/domo/sensors/tx3_th.php
void IT_TX(unsigned int duration) {

  static unsigned int changeCount;
  static unsigned int fDuration;
  static unsigned int sDuration;

  sDuration = fDuration + duration;
  
  if ((sDuration > 1550 - 200 && sDuration < 1550 + 200) || (sDuration > 2150 - 200 && sDuration < 2150 + 200)) {
    if ((duration > 520 - 100 && duration < 520 + 100) || (duration > 1250 - 100 && duration < 1250 + 100) || (duration > 950 - 100 && duration < 950 + 100)) {
      if (changeCount == 0 ) {
        timingsTX[changeCount++] = fDuration;
      }
      timingsTX[changeCount++] = duration;
    }

    if ( changeCount == TX_MAX_CHANGES - 1) {
      receiveProtocolIT_TX(changeCount);
      changeCount = 0;
      fDuration = 0;
    } 
  } else {
    changeCount = 0;
  }

  fDuration = duration;
}

void receiveProtocolIT_TX(unsigned int changeCount) {
#define TX_ONE    520
#define TX_ZERO   1250
#define TX_GLITCH  100
#define TX_MESSAGELENGTH 44

  byte i;
  unsigned long code = 0;

#ifdef USE_IT_TX
  message = "TX";
#else
  message = "W00";
#endif

  for (i = 0; i <= 14; i = i + 2)
  {
    if ((timingsTX[i] > TX_ZERO - TX_GLITCH) && (timingsTX[i] < TX_ZERO + TX_GLITCH))    {
      code <<= 1;
    }
    else if ((timingsTX[i] > TX_ONE - TX_GLITCH) && (timingsTX[i] < TX_ONE + TX_GLITCH)) {
      code <<= 1;
      code |= 1;
    }
    else {
      return;
    }
  }

  // Startsequence 0000 1010 = 0xA
  if (code != 10) {
    return;
  }

  message += String(code,HEX);
  code = 0;

  // Sensor type 0000 = Temp / 1110 = Humidity
  for (i = 16; i <= 22; i = i + 2)
  {
    if ((timingsTX[i] > TX_ZERO - TX_GLITCH) && (timingsTX[i] < TX_ZERO + TX_GLITCH))    {
      code <<= 1;
    }
    else if ((timingsTX[i] > TX_ONE - TX_GLITCH) && (timingsTX[i] < TX_ONE + TX_GLITCH)) {
      code <<= 1;
      code |= 1;
    }
    else {
      return;
    }
  }

  message += String(code,HEX);
  code = 0;

  // Sensor adress
  for (i = 24; i <= 38; i = i + 2)
  {
    if ((timingsTX[i] > TX_ZERO - TX_GLITCH) && (timingsTX[i] < TX_ZERO + TX_GLITCH))    {
      code <<= 1;
    }
    else if ((timingsTX[i] > TX_ONE - TX_GLITCH) && (timingsTX[i] < TX_ONE + TX_GLITCH)) {
      code <<= 1;
      code |= 1;
    }
    else {
      return;
    }
  }
  message += String(code,HEX);
  code = 0;

  // Temp or Humidity
  for (i = 40; i <= 62; i = i + 2)
  {
    if ((timingsTX[i] > TX_ZERO - TX_GLITCH) && (timingsTX[i] < TX_ZERO + TX_GLITCH))    {
      code <<= 1;
    }
    else if ((timingsTX[i] > TX_ONE - TX_GLITCH) && (timingsTX[i] < TX_ONE + TX_GLITCH)) {
      code <<= 1;
      code |= 1;
    }
    else {
      return;
    }
  }
  message += String(code,HEX);
  code = 0;

  // Repeated Bytes temp / Humidity
  for (i = 64; i <= 78; i = i + 2)
  {
    if ((timingsTX[i] > TX_ZERO - TX_GLITCH) && (timingsTX[i] < TX_ZERO + TX_GLITCH))    {
      code <<= 1;
    }
    else if ((timingsTX[i] > TX_ONE - TX_GLITCH) && (timingsTX[i] < TX_ONE + TX_GLITCH)) {
      code <<= 1;
      code |= 1;
    }
    else {
      return;
    }
  }
  message += String(code,HEX);
  code = 0;

  // Checksum
  for (i = 80; i <= changeCount; i = i + 2)
  {
    if ((timingsTX[i] > TX_ZERO - TX_GLITCH) && (timingsTX[i] < TX_ZERO + TX_GLITCH))    {
      code <<= 1;
    }
    else if ((timingsTX[i] > TX_ONE - TX_GLITCH) && (timingsTX[i] < TX_ONE + TX_GLITCH)) {
      code <<= 1;
      code |= 1;
    }
    else {
      return;
    }
  }
  message += String(code,HEX);
  
  message.toUpperCase();

  available = true;
  return;
}
#endif

#ifdef COMP_KW9010
/*
 * KW9010
 */
bool receiveProtocolKW9010(unsigned int changeCount) {
#define KW9010_SYNC 9000
#define KW9010_ONE 4000
#define KW9010_ZERO 2000
#define KW9010_GLITCH 200
#define KW9010_MESSAGELENGTH 36

  if (changeCount < KW9010_MESSAGELENGTH * 2) return false;

  if ((timings[0] < KW9010_SYNC - KW9010_GLITCH) || (timings[0] > KW9010_SYNC + KW9010_GLITCH)) {
    return false;
  }

#ifdef DEBUG
  bool bitmessage[KW9010_MESSAGELENGTH];
  if (GetBitStream(timings, bitmessage, KW9010_MESSAGELENGTH * 2, KW9010_ZERO - KW9010_GLITCH, KW9010_ZERO + KW9010_GLITCH, KW9010_ONE - KW9010_GLITCH, KW9010_ONE + KW9010_GLITCH) == false) {
      return false;
  }
#endif

  String rawcode;
  rawcode = RawMessage(timings, KW9010_MESSAGELENGTH, KW9010_ZERO - KW9010_GLITCH, KW9010_ZERO + KW9010_GLITCH, KW9010_ONE - KW9010_GLITCH, KW9010_ONE + KW9010_GLITCH);

  if (rawcode == "") {
    return false;
  }

  String crcbitmessage = hex2bin(rawcode);
  
  // check Data integrity
  byte checksum = 0;
  checksum += (byte)(crcbitmessage[35] << 3 | crcbitmessage[34] << 2 | crcbitmessage[33] << 1 | crcbitmessage[32]);
  checksum &= 0xF;
  
  byte calculatedChecksum = 0;
  for (int i = 0 ; i <= 7 ; i++) {
    calculatedChecksum += (byte)(crcbitmessage[i*4 + 3] << 3 | crcbitmessage[i*4 + 2] << 2 | crcbitmessage[i*4 + 1] << 1 | crcbitmessage[i*4]);
  }
  calculatedChecksum &= 0xF;

  if (calculatedChecksum == checksum) {
    message = "W01";
    message += rawcode;
    available = true;
    return true;
  }

  return false;

}
#endif

#ifdef COMP_EUROCHRON
/*
 * EUROCHRON
 */
bool receiveProtocolEuroChron(unsigned int changeCount) {
#define Euro_SYNC   8050
#define Euro_ONE    2020
#define Euro_ZERO   1010
#define Euro_GLITCH  100
#define Euro_MESSAGELENGTH 36

  if (changeCount < Euro_MESSAGELENGTH * 2) {
    return false;
  }

  if ((timings[0] < Euro_SYNC - Euro_GLITCH) && (timings[0] > Euro_SYNC + Euro_GLITCH)) {
    return false;
  }
  
#ifdef DEBUG
  bool bitmessage[Euro_MESSAGELENGTH];
  if (GetBitStream(timings, bitmessage, Euro_MESSAGELENGTH * 2, Euro_ZERO - Euro_GLITCH, Euro_ZERO + Euro_GLITCH, Euro_ONE - Euro_GLITCH, Euro_ONE + Euro_GLITCH) == false) {
      return false;
  }
#endif
  String rawcode;
  rawcode = RawMessage(timings, Euro_MESSAGELENGTH, Euro_ZERO - Euro_GLITCH, Euro_ZERO + Euro_GLITCH, Euro_ONE - Euro_GLITCH, Euro_ONE + Euro_GLITCH);

  if (rawcode == "") {
      return false;
  }

  message = "W02";
  message += rawcode;
  available = true;
  return true;

}
#endif

#ifdef COMP_NC_WS
/*
 * NC_WS / PEARL NC7159, LogiLink W0002
 */
bool receiveProtocolNC_WS(unsigned int changeCount) {
#define NCWS_SYNC   9250
#define NCWS_ONE    3900
#define NCWS_ZERO   1950
#define NCWS_GLITCH 100
#define NCWS_MESSAGELENGTH 36

  if (changeCount < NCWS_MESSAGELENGTH * 2) {
    return false;
  }
  
  if ((timings[0] < NCWS_SYNC - NCWS_GLITCH) || (timings[0] > NCWS_SYNC + NCWS_GLITCH)) {
    return false;
  }

#ifdef DEBUG
  bool bitmessage[NCWS_MESSAGELENGTH];
  if (GetBitStream(timings, bitmessage, NCWS_MESSAGELENGTH * 2, NCWS_ZERO - NCWS_GLITCH, NCWS_ZERO + NCWS_GLITCH, NCWS_ONE - NCWS_GLITCH, NCWS_ONE + NCWS_GLITCH) == false) {
      return false;
  }
#endif
  String rawcode;
  rawcode = RawMessage(timings, NCWS_MESSAGELENGTH, NCWS_ZERO - NCWS_GLITCH, NCWS_ZERO + NCWS_GLITCH, NCWS_ONE - NCWS_GLITCH, NCWS_ONE + NCWS_GLITCH);
  if (rawcode == "") {
      return false;
  }

  // First 4 bits always 0101 == 5
  if (rawcode[0] != '5') {
    return false;
  }

  message = "W03";
  message += rawcode;
  available = true;
  return true;

}
#endif

#ifdef COMP_LIFETEC
/*
 * LIFETEC
 */
bool receiveProtocolLIFETEC(unsigned int changeCount) {
#define LIFE_SYNC   8640
#define LIFE_ONE    4084
#define LIFE_ZERO   2016
#define LIFE_GLITCH  460
#define LIFE_MESSAGELENGTH 24

  if (changeCount < LIFE_MESSAGELENGTH * 2) {
    return false;
  }
  if ((timings[0] < LIFE_SYNC - LIFE_GLITCH) || (timings[0] > LIFE_SYNC + LIFE_GLITCH)) {
    return false;
  }

#ifdef DEBUG
  bool bitmessage[TX_MESSAGELENGTH];
  if (GetBitStream(timings, bitmessage, LIFE_MESSAGELENGTH * 2, LIFE_ZERO - LIFE_GLITCH, LIFE_ZERO + LIFE_GLITCH, LIFE_ONE - LIFE_GLITCH, LIFE_ONE + LIFE_GLITCH) == false) {
      return false;
  }
#endif
  String rawcode;
  rawcode = RawMessage(timings, LIFE_MESSAGELENGTH, LIFE_ZERO - LIFE_GLITCH, LIFE_ZERO + LIFE_GLITCH, LIFE_ONE - LIFE_GLITCH, LIFE_ONE + LIFE_GLITCH);
  if (rawcode == "") {
      return false;
  }

  message = "W04";
  message += rawcode;
  message += "___"; // Fill up to 12 signs
  available = true;
  return true;

}
#endif

#ifdef COMP_TX70DTH
/*
 * TX70DTH
 */
bool receiveProtocolTX70DTH(unsigned int changeCount) {
#define TX_SYNC   4000
#define TX_ONE    2030
#define TX_ZERO   1020
#define TX_GLITCH  250
#define TX_MESSAGELENGTH 36

  if (changeCount < TX_MESSAGELENGTH * 2) {
    return false;
  }
  if ((timings2500[0] < TX_SYNC - TX_GLITCH) || (timings2500[0] > TX_SYNC + TX_GLITCH)) {
    return false;
  }

#ifdef DEBUG
  bool bitmessage[TX_MESSAGELENGTH];
  if (GetBitStream(timings2500, bitmessage, TX_MESSAGELENGTH * 2, TX_ZERO - TX_GLITCH, TX_ZERO + TX_GLITCH, TX_ONE - TX_GLITCH, TX_ONE + TX_GLITCH) == false) {
      return false;
  }
#endif
  String rawcode;
  rawcode = RawMessage(timings2500, TX_MESSAGELENGTH, TX_ZERO - TX_GLITCH, TX_ZERO + TX_GLITCH, TX_ONE - TX_GLITCH, TX_ONE + TX_GLITCH);
  if (rawcode == "") {
      return false;
  }

  message = "W05";
  message += rawcode;
  available = true;
  return true;
}
#endif

#ifdef COMP_AURIOL
/*
 * AURIOL (Lidl Version: 09/2013)
 */
bool receiveProtocolAURIOL(unsigned int changeCount) {
#define AURIOL_SYNC 9200
#define AURIOL_ONE 3900
#define AURIOL_ZERO 1950
#define AURIOL_GLITCH 150
#define AURIOL_MESSAGELENGTH 32

  if (changeCount != (AURIOL_MESSAGELENGTH * 2) + 1) {
    return false;
  }

  if ((timings[0] < AURIOL_SYNC - AURIOL_GLITCH) || (timings[0] > AURIOL_SYNC + AURIOL_GLITCH)) {
    return false;
  }

#ifdef DEBUG
  bool bitmessage[AURIOL_MESSAGELENGTH];
  if (GetBitStream(timings, bitmessage, AURIOL_MESSAGELENGTH * 2, AURIOL_ZERO - AURIOL_GLITCH, AURIOL_ZERO + AURIOL_GLITCH, AURIOL_ONE - AURIOL_GLITCH, AURIOL_ONE + AURIOL_GLITCH) == false) {
    Serial.println("Err: BitStream");
    return false;
  }
#endif

  String rawcode;
  rawcode = RawMessage(timings, AURIOL_MESSAGELENGTH, AURIOL_ZERO - AURIOL_GLITCH, AURIOL_ZERO + AURIOL_GLITCH, AURIOL_ONE - AURIOL_GLITCH, AURIOL_ONE + AURIOL_GLITCH);

  if (rawcode == "") {
    return false;
  }

  // check Data integrity
  message = "W06";
  message += rawcode;
  message += "_"; // Fill up to 12 signs
  available = true;
  return true;

}
#endif

/*-----------------------------------------------------------------------------------------------
/* Serial Command Handling
-----------------------------------------------------------------------------------------------*/
void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    switch(inChar)
    {
    case '\n':
    case '\r':
    case '\0':
    case '#':
      HandleCommand(cmdstring);
      break;
    default:
      cmdstring = cmdstring + inChar;
    }
  }
}

void HandleCommand(String cmd)
{
  // Version Information
  if (cmd.equals("V"))
  {
    Serial.println(F("V " PROGVERS " FHEMduino - compiled at " __DATE__ " " __TIME__));
  }
  // Print free Memory
  else if (cmd.equals("R")) {
    Serial.print(F("R"));
    Serial.println(freeRam());
  }
#ifdef COMP_FA20RF
  // Set FA20RF Repetition
  else if (cmd.startsWith("f"))
  {
     FA20RF_CMDs(cmd);
  }
#endif
#ifdef COMP_PT2262
  // Intertechno commands
  else if (cmd.startsWith("i")) {
    PT2262_CMDs(cmd);
  }
#endif
#ifdef COMP_DOORBELL
  // Tchibo TCM commands
  else if (cmd.startsWith("d")) {
    TCM_CMDs(cmd);
  }
  // Heidemann commands
  else if (cmd.startsWith("h")) {
    HeideTX_CMDs(cmd);
  }
#endif
  else if (cmd.equals("t")) {
    uptime(millis(), true);
  }
    else if (cmd.equals("XQ")) {
    disableReceive();
    Serial.flush();
    Serial.end();
  }
  // Print Available Commands
  else if (cmd.equals("?"))
  {
    Serial.println(F("? Use one of V i f d h t R q"));
  }
  cmdstring = "";
}

/*-----------------------------------------------------------------------------------------------
/* Helper functions
-----------------------------------------------------------------------------------------------*/
void uptime(unsigned long timepassed, bool Print)
{
  static unsigned long time_in_secs=0;
  static unsigned long temps=0;
  unsigned long secs=0;

  secs = timepassed/1000;
  
  if (secs < temps) {
     time_in_secs += secs;
  } else {
     time_in_secs = secs;
  }

  temps = timepassed/1000;
  
  if (Print) {
    //Display results
    String cPrint = "00000000";
    cPrint += String(time_in_secs,HEX);
    int StringStart = cPrint.length()-8;
    cPrint = cPrint.substring(StringStart);
    cPrint.toUpperCase();
    Serial.println(cPrint);
  }
}

// Get free RAM of UC
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

/*
 * Message Handling
 */
bool messageAvailable() {
  return (available && (message.length() > 0));
}

void resetAvailable() {
  available = false;
  message = "";
}

#ifdef DEBUG
bool GetBitStream(unsigned int timings[] ,bool bitmessage[], byte BitCount, unsigned int L_Zero, unsigned int R_Zero, unsigned int L_One, unsigned int R_One) {

  byte i=0;
  byte j=0;
  
  for (i = 0; i < BitCount; i = i + 2)
  {
    j = i + 2;
    if ((timings[j] > L_Zero) && (timings[j] < R_Zero))    {
      bitmessage[i >> 1] = false;
    }
    else if ((timings[j] > L_One) && (timings[j] < R_One)) {
      bitmessage[i >> 1] = true;
    }
    else {
      return false;
    }
  }

  Serial.print("Bit-Stream: ");
  for (i = 0; i < (BitCount / 2); i++) {
    Serial.print(bitmessage[i]);
  }
  Serial.println();

  return true;
}
#endif

String RawMessage(unsigned int timings[], byte BitCount, unsigned int L_Zero, unsigned int R_Zero, unsigned int L_One, unsigned int R_One) {

  byte i=0;
  byte j=0;
  byte code = 0;
  String hexText;
  
  for (i = 0; i < BitCount * 2; i = i + 2)
  {
    j = i + 2;
    if ((timings[j] > L_Zero) && (timings[j] < R_Zero))    {
      code = code << 1;
    }
    else if ((timings[j] > L_One) && (timings[j] < R_One)) {
      code <<= 1;
      code |= 1;
    }
    else {
      return "";
    }
    // Byte then chnage to hex
    if ((j % 8) == 0) {
      hexText += String(code,HEX);
      code = 0;
    }
  }
  
  // maybe some bits left
  if ((j % 8) != 0) {
    hexText += String(code,HEX);
  }

  return hexText;
}

unsigned long hexToDec(String hexString) {
  
  unsigned long decValue = 0;
  int nextInt;
  
  for (int i = 0; i < hexString.length(); i++) {
    
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    
    decValue = (decValue * 16) + nextInt;
  }
  
  return decValue;
}

String hex2bin(String hexaDecimal)
{
  byte i=0;
  String bitstream;

  while(hexaDecimal[i]){
    switch(hexaDecimal[i]){
      case '0': bitstream += "0000"; break;
      case '1': bitstream += "0001"; break;
      case '2': bitstream += "0010"; break;
      case '3': bitstream += "0011"; break;
      case '4': bitstream += "0100"; break;
      case '5': bitstream += "0101"; break;
      case '6': bitstream += "0110"; break;
      case '7': bitstream += "0111"; break;
      case '8': bitstream += "1000"; break;
      case '9': bitstream += "1001"; break;
      case 'A': bitstream += "1010"; break;
      case 'B': bitstream += "1011"; break;
      case 'C': bitstream += "1100"; break;
      case 'D': bitstream += "1101"; break;
      case 'E': bitstream += "1110"; break;
      case 'F': bitstream += "1111"; break;
      case 'a': bitstream += "1010"; break;
      case 'b': bitstream += "1011"; break;
      case 'c': bitstream += "1100"; break;
      case 'd': bitstream += "1101"; break;
      case 'e': bitstream += "1110"; break;
      case 'f': bitstream += "1111"; break;
      default:  ;
    }
    i++;
  }
  return bitstream;
}

