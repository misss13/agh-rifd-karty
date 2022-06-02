#include <MFRC522.h>
#include <SPI.h>
#include "UART.h"

#define SS_PIN 21
#define RST_PIN 22
#define SIZE_BUFFER 18
#define MAX_SIZE_BLOCK 16
#define greenPin 12

MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

MFRC522 rfid(SS_PIN, RST_PIN);

byte keyTagUID[4] = {0x20, 0x36, 0x4A, 0x30};
byte karta[18] = {0x47, 0x29, 0x5E, 0x38, 0xEB, 0x29, 0x5E, 0x38, 0xD7, 0x0, 0x2, 0x0, 0x0, 0x4, 0x0, 0x0 };

int uid_r();
int data_r();


void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  pinMode(greenPin, OUTPUT);

  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        int uid_c = uid_r();
        int data_c = data_r();
          if ( uid_c == 1 && data_c == 1) {
          Serial.println("Access is granted");
          digitalWrite(greenPin, HIGH);
          delay(2000);
          digitalWrite(greenPin, LOW);
          }
          else
          {
            Serial.print("Access denied, UID:");
            Serial.println();
          }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}

int uid_r(){
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("UID: ");
      for (int i = 0; i<4; i++){
          Serial.print(rfid.uid.uidByte[i], HEX);
          Serial.print(" ");
      }
      
      if (rfid.uid.uidByte[0] == keyTagUID[0] &&
          rfid.uid.uidByte[1] == keyTagUID[1] &&
          rfid.uid.uidByte[2] == keyTagUID[2] &&
          rfid.uid.uidByte[3] == keyTagUID[3] ) {
          return 1;
      } else {
        return 0;
      }

}

int data_r(){
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; //KEY-A
  byte buffer[SIZE_BUFFER] = {0};
  byte block = 1;
  byte size = SIZE_BUFFER;
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return 0;
  }
  status = rfid.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return 0;
  }
  Serial.print("Data from 1 sector 1 block: ");
  int czek = 1;
  for ( uint8_t i = 0; i < MAX_SIZE_BLOCK; i++ ) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
    if( buffer[i] != karta[i]){
      czek = 0;
    }
  }
  if ( czek == 1 )
    return 1;
  
  return 0;
}