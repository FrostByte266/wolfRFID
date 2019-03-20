#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include "wolfRfid.h"

#define SS_PIN 10
#define RST_PIN 9
MFRC522 reader(SS_PIN, RST_PIN);
byte arr1[] = "533e9fd3mm120p10d000c969251d3rf099p12d081c";
byte arr[43];
byte cardData[8];
//byte eepromData[1025];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  reader.PCD_Init();
  //EEPROM.put(0, arr1);
  //Serial.println("Wrote to eeprom");
  EEPROM.get(0, arr);
}

void loop() {

  if ( ! reader.PICC_IsNewCardPresent()) {
    return;
  }


  if ( ! reader.PICC_ReadCardSerial()) {
    return;
  }
  String id = readID(reader);
  Serial.println(id);
  String found = findInArray(arr, id);
  //Serial.println(found);
  //wolfData(found.c_str());
  Serial.println(wolfData(found.c_str()));
  reader.PICC_HaltA();

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

String wolfData(String _wolfData) {
  char wolfData[255];
  _wolfData.toCharArray(wolfData, 22);
  //Serial.println(_wolfData);
  String uid = sliceArray(wolfData, 0, 7);
  //Serial.println(uid);
  char species_r = wolfData[8];
  String species;
  switch(species_r){
    case 'g':
      species = "Gray";
      break;
    case 'm':
      species = "Mexican Gray Wolf";
      break;
    case 'r':
      species = "Red Wolf";
      break;
    default:
      species = "ERROR";
      break;
  }
  char sex_r = wolfData[9];
  String sex;
  switch(sex_r){
    case 'm':
      sex = "Male";
      break;
    case 'f':
      sex = "Female";
      break;
    default:
      sex = "ERROR";
      break;
  }
  String weight = sliceArray(wolfData, 10, 12);
  String last_seen = sliceArray(wolfData, 14, 15);
  String scan_count = sliceArray(wolfData, 17, 19);
  char data_out[255];
  sprintf(data_out, "UID: %s\nSpecies: %s\nSex: %s\nWeight: %slbs\nLast Seen: %s days\nScan Count: %s", uid.c_str(), species.c_str(), sex.c_str(), weight.c_str(), last_seen.c_str(), scan_count.c_str());
  //Serial.println(data_out);
  return data_out;
}
