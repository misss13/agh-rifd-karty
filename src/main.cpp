#include <MFRC522.h>
#include <SPI.h>
#include "UART.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

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
void uid_w();
void data_w();

int bistable = 0;
int light = 0;

const char *ssid = "yourAP";
const char *password = "yourPassword";
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init();
  pinMode(greenPin, OUTPUT);
  
  UART::init();
  UART::println("AP constructing");
  WiFi.softAP(ssid, password);
  server.begin();
  UART::println("Server UP");
  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
}

void loop() {
  UART::tick();
  WiFiClient client = server.available();

  if ( rfid.PICC_IsNewCardPresent() ){ // new tag is available
    if ( rfid.PICC_ReadCardSerial() ){ // NUID has been readed
      if ( UART::WLANCZNIK == 0 ){
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        int uid_c = uid_r();
        int data_c = data_r();
          if ( uid_c == 1 && data_c == 1) {
            Serial.println("Access is granted");
            if ( bistable == 0) {
              digitalWrite(greenPin, HIGH);
              delay(2000);
              digitalWrite(greenPin, LOW);
            } else {
              if( light == 0){
                light = 1;
                digitalWrite(greenPin, HIGH);
              } else {
                light = 0;
                digitalWrite(greenPin, LOW);
              }
            }
          } else {
            Serial.print("Access denied!");
            Serial.println();
          }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
      }
      else {
        uid_w();
        data_w();
        Serial.print("Card saved");
        rfid.PICC_HaltA(); // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD
      }
    }
  }
  if (client) {                       
    UART::println("New client connected");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("<button onclick=\"window.location.href='/prog'\">Tryb programowania</button><br/>");
            client.print("<button onclick=\"window.location.href='/once'\">Otworz drzwi raz</button><br/>");
            client.print("<button onclick=\"window.location.href='/multi'\">Otworz drzwi na kilka minut</button><br/>");
            client.print("<button onclick=\"window.location.href='/bis'\">Tryb bistabilny</button><br/>"); //po przyłożeniu karty drzwi się zamukają
            client.print("<button onclick=\"window.location.href='/list'\">Lista kart</button><br/>");
            
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        if (currentLine.endsWith("GET /prog")) {
          digitalWrite(LED_BUILTIN, HIGH);
          //func
        }
        if (currentLine.endsWith("GET /once")) {
          bistable = 0;
        }
        if (currentLine.endsWith("GET /multi")) {
          digitalWrite(LED_BUILTIN, HIGH); 
          //func
        }
        if (currentLine.endsWith("GET /bis")) {
          bistable = 1;
        }
        if (currentLine.endsWith("GET /list")) {
          digitalWrite(LED_BUILTIN, HIGH); 
          //func
        }
      }
    }
    client.stop();
    UART::println("Client Disconnected.");
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

void uid_w(){
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("New saved UID: ");
  for (int i = 0; i<4; i++){
      Serial.print(rfid.uid.uidByte[i], HEX);
      Serial.print(" ");
      keyTagUID[i] = rfid.uid.uidByte[i];
  }
}

void data_w(){
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; //KEY-A
  byte buffer[SIZE_BUFFER] = {0};
  byte block = 1;
  byte size = SIZE_BUFFER;
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  status = rfid.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }
  Serial.print("New data to save from 1 sector 1 block: ");
  for ( uint8_t i = 0; i < MAX_SIZE_BLOCK; i++ ) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
    karta[i] = buffer[i];
  }
}