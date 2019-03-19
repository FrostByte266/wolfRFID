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
  char species_r = wolfData[8];
  switch(species_r){
    case 'g':
      String species = "Gray";
      break;
    case 'm':
      String species = "Mexican Gray Wolf";
      break;
    case 'r':
      String species = "Red Wolf";
      break;
    default:
      String species = "ERROR";
      break;
  }
  char sex_r = wolfData[9];
  switch(sex_r){
    case 'm':
      String sex = "Male";
      break;
    case 'f'
      String sex = "Female";
      break;
    default:
      String sex = "ERROR";
      break;
  }
  String weight = sliceArray(wolfData, 10, 13);
  String last_seen = sliceArray(wolfData, 14, 16);
  String scan_count = sliceArray(wolfData, 17, 19);
  char data_out[255];
  sprintf(data_out, "UID: %s, Species: %c, Sex: %c, Weight: %s, Last Seen: %s, Scan Count: %s", uid.c_str(), species, sex, weight.c_str(), last_seen.c_str(), scan_count.c_str());
  return data_out;
  
}
