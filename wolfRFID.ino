#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include "wolfRfid.h"

#define SS_PIN 10
#define RST_PIN 9
#define SAVE_SIGNAL 2

MFRC522 reader(SS_PIN, RST_PIN);
byte eepromToWrite[] = "533e9fd3mm120p10d000c969251d3rf099p12d000c";
byte arr[43];
byte cardData[8];
//byte eepromData[1025];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  reader.PCD_Init();
  pinMode(SAVE_SIGNAL, INPUT);
  /*
  EEPROM.put(0, eepromToWrite);
  Serial.println("Wrote to eeprom");
  */
  EEPROM.get(0, arr);
}

void loop() {
/*
  if(digitalRead(SAVE_SIGNAL) == HIGH){
    Serial.println("Saving...");
    EEPROM.put(0, arr);
    Serial.println("Save complete. Please release save button...");
    while(digitalRead(SAVE_SIGNAL) == HIGH){
      continue;
    }
  }
  */

  if ( ! reader.PICC_IsNewCardPresent()) {
    return;
  }


  if ( ! reader.PICC_ReadCardSerial()) {
    return;
  }
  String id = readID(reader);
  String found = findInArray(arr, id);
  Serial.println(wolfData(found.c_str(), id));
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

String wolfData(const byte wolfData[], const String& id) {
  String uid = sliceArray(wolfData, 0, 7);
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
  int weight = sliceArray(wolfData, 10, 12).toInt();
  String last_seen = sliceArray(wolfData, 14, 15);
  int scan_count = incrementScanCount(id);
  char data_out[255];
  sprintf(data_out, "UID: %s\nSpecies: %s\nSex: %s\nWeight: %d lbs\nLast Seen: %s days\nScan Count: %d", uid.c_str(), species.c_str(), sex.c_str(), weight, last_seen.c_str(), scan_count);
  //Serial.println(data_out);
  return data_out;
}

String sliceArray(const byte blob[], const int& start, const int& end){
  String str;
  char temp[8];
  for(int i=start; i<=end; i++){
    sprintf(temp, "%c", blob[i]);
    str += temp;
  }
  return str;
}

String findInArray(const byte blob[], const String& id){
  String frame;
  for(int i=0; i<1024; i+=21){
    frame = sliceArray(blob, i, i+20);
    Serial.print("Searching Frame: ");
    Serial.println(frame);
    String temp = sliceArray(frame.c_str(), 0, 7);
    if(temp == id){
      Serial.println("Found ID! Printing Data...");
      break;
    }else{
      Serial.println("ID not found, continuing to next frame.");
    }
  }
  return frame;
}

int findCountHandle(const byte blob[], const String& id){
  String frame;
  for(int i=0; i<1024; i+=21){
    frame = sliceArray(blob, i, i+20);
    String temp = sliceArray(frame.c_str(), 0, 7);
    if(temp == id){
      return i+17;
    }
  }
}

int incrementScanCount(const String& id){
  char temp[3];
  const int handle = findCountHandle(arr, id);
  temp[0] = arr[handle];
  temp[1] = arr[handle + 1];
  temp[2] = arr[handle + 2];
  int temp_i = atoi(temp);
  int rvalue = temp_i + 1;
  sprintf(temp, "%03d", rvalue);
  arr[handle] = temp[0];
  arr[handle + 1] = temp[1];
  arr[handle + 2] = temp[2];
  return rvalue;
}
