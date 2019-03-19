#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include "wolfRfid.h"

#define SS_PIN 10
#define RST_PIN 9
MFRC522 reader(SS_PIN, RST_PIN);

byte cardData[8];
byte eepromData[1025];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  reader.PCD_Init();

  const byte arr[] = "8162617bmm120p10d000c";
  Serial.println(wolfData(arr));
}

void loop() {

  if ( ! reader.PICC_IsNewCardPresent()) {
    return;
  }


  if ( ! reader.PICC_ReadCardSerial()) {
    return;
  }
  Serial.println(readID(reader));
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

String readID(MFRC522 _reader) {
  byte uidBytes[4];
  readToArray(_reader.uid.uidByte, uidBytes);
  return makeUidString(uidBytes);
}

String wolfData(char wolfData[]) {
  String uid = sliceArray(wolfData, 0, 7);
  char species = wolfData[8];
  char gender = wolfData[9];
  String weight = sliceArray(wolfData, 10, 13);
  String last_seen = sliceArray(wolfData, 14, 16);
  String scan_count = sliceArray(wolfData, 17, 19);
  char data_out[255];
  sprintf(data_out, "UID: %s, Species: %c, Gender: %c, Weight: %s, Last Seen: %s, Scan Count: %s", uid.c_str(), species, gender, weight.c_str(), last_seen.c_str(), scan_count.c_str());
  return data_out;
}

String wolfData(char wolfData[]) {
  String uid = sliceArray(wolfData, 0, 7);
  String species = wolfData[8];
  String gender = wolfData[9];
  String weight = sliceArray(wolfData, 10, 13);
  String last_seen = sliceArray(wolfData, 14, 16);
  String scan_count = sliceArray(wolfData, 17, 19);
  char data_out[255];
  sprintf(data_out, "UID: %s, Species: %s, Gender: %s, Weight: %s, Last Seen: %s, Scan Count: %s", uid, species, gender, weight, last_seen, scan_count);
}

