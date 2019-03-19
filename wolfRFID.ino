#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include "wolfRfid.h"

#define SS_PIN 10
#define RST_PIN 9
MFRC522 reader(SS_PIN, RST_PIN);

byte cardData[8];
byte eepromData[1024];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  reader.PCD_Init();
}

void loop() {

  if ( ! reader.PICC_IsNewCardPresent()) {
    return;
  }


  if ( ! reader.PICC_ReadCardSerial()) {
    return;
  }
  
  void loop() {
  byte uidBytes[4];
  readToArray(reader.uid.uidByte, uidBytes);
  Serial.println(makeUidString(uidBytes));
  reader.PICC_HaltA();

}

void getAllEeprom(byte eepromCells[]) {
  for (int i = 0; i < 1024; i++) {
    eepromCells[i] = EEPROM.read(i);
  }
}

void readToArray(byte readerData[], byte output[]) {
  for (int i = 0; i < 4; i++) {
    output[i] = reader.uid.uidByte[i];
  }
}

String makeUidString(byte in[]) {
  String out;
  char temp[2];
  for (int i = 0; i < 4; i++) {
    sprintf(temp, "%02x", in[i]);
    out += temp;
  }
  return out;
}
