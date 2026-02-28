/*
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 * Released into the public domain.
 *
 * Sample program showing how to read data from a PICC using a MFRC522 reader on the Arduino SPI interface.
 *----------------------------------------------------------------------------- empty_skull 
 * Aggiunti pin per arduino Mega
 * add pin configuration for arduino mega
 * http://mac86project.altervista.org/
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>
#include <DFMiniMp3.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define VP_PIN 2
#define VM_PIN 3
#define L1_PIN 5
#define L2_PIN 6
#define L3_PIN 7
#define L4_PIN 8
#define V_BAT A6

class Mp3Notify
{
public:
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }

  static void OnPlayFinished(uint16_t globalTrack)
  {
    Serial.println();
    Serial.print("Play finished for #");
    Serial.println(globalTrack);   
  }

  static void OnCardOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("Card online ");
    Serial.println(code);     
  }

  static void OnUsbOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("USB Disk online ");
    Serial.println(code);     
  }

  static void OnCardInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("Card inserted ");
    Serial.println(code); 
  }

  static void OnUsbInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("USB Disk inserted ");
    Serial.println(code); 
  }

  static void OnCardRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("Card removed ");
    Serial.println(code);  
  }

  static void OnUsbRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("USB Disk removed ");
    Serial.println(code);  
  }
};

void led(char s)
{
   switch(s)
   {
    case 0:
      digitalWrite(L1_PIN,0);
      digitalWrite(L2_PIN,0);
      digitalWrite(L3_PIN,0);
      digitalWrite(L4_PIN,0);
      break;
    case 1:
      digitalWrite(L1_PIN,0);
      digitalWrite(L2_PIN,0);
      digitalWrite(L3_PIN,0);
      digitalWrite(L4_PIN,1);
      break;
    case 2:
      digitalWrite(L1_PIN,0);
      digitalWrite(L2_PIN,0);
      digitalWrite(L3_PIN,1);
      digitalWrite(L4_PIN,1);
      break;
    case 3:
      digitalWrite(L1_PIN,0);
      digitalWrite(L2_PIN,1);
      digitalWrite(L3_PIN,1);
      digitalWrite(L4_PIN,1);
      break;
    case 4:
      digitalWrite(L1_PIN,1);
      digitalWrite(L2_PIN,1);
      digitalWrite(L3_PIN,1);
      digitalWrite(L4_PIN,1);
      break;
   }
   return;
}

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
byte nuidPICC[4];
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial);
byte volu=5;
char level[5]={0,0,0,0,0};

void setup() {
  delay(1000);
  pinMode(L1_PIN,OUTPUT);
  pinMode(L2_PIN,OUTPUT);
  pinMode(L3_PIN,OUTPUT);
  pinMode(L4_PIN,OUTPUT);
  digitalWrite(VP_PIN,1);
  digitalWrite(VM_PIN,1);

  
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mp3.begin();
  mp3.setVolume(volu);
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Scan PICC to see UID and type...");
  
}

void loop() {
  // Look for new cards
  if(digitalRead(VP_PIN)==0 && volu<13)
  {
    volu++;
    mp3.setVolume(volu);
    delay(250);
  }
  if(digitalRead(VM_PIN)==0 && volu>0)
  {
    volu--;
    mp3.setVolume(volu);
    delay(250);
  }

  int nm=0;
  for(char i=0;i<4;i++)
  { 
    level[i]=level[i+1];
    nm+=level[i];
  }
  level[4]=analogRead(V_BAT)/10;
  nm+=level[4];
  nm=nm/5;  
  led((nm-68)/4);

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {   
    return;
  }
  
  byte buffer2[18];
  byte block=1;
  byte len=18;
  MFRC522::StatusCode status;
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    //Serial.print(F("Authentication failed: "));
    //Serial.println(mfrc522.GetStatusCodeName(status));
    mfrc522.PCD_Init();   
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    //Serial.print(F("Reading failed: "));
    //Serial.println(mfrc522.GetStatusCodeName(status));
    mfrc522.PCD_Init();
    return;
  }
  
  byte num[4];
  int number=0;
  for (uint8_t i = 1; i < 5; i++) {
    //Serial.print(buffer2[i]);
    num[i-1]=buffer2[i]-48;
  }
  number=(num[0]*1000)+(num[1]*100)+(num[2]*10)+(num[3]);
  mp3.playMp3FolderTrack(number);
  mfrc522.PCD_Init(); 
  delay(5000);

}
