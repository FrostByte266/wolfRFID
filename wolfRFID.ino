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

void setup() {
  Serial.begin(9600);
  //Setup Reader
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
  //Would apply the save function, only uncomment if you have the wires hooked
  //up with pull down resistors
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

  //If no new card is present or if the read fails, restart the loop
  if ( ! reader.PICC_IsNewCardPresent()) {
    return;
  }


  if ( ! reader.PICC_ReadCardSerial()) {
    return;
  }

  //Get ID from reader
  String id = readID(reader);
  //Search EEPROM
  String found = findInArray(arr, id);
  Serial.println(wolfData(found.c_str(), id));
  //Send halt signal to RC522
  reader.PICC_HaltA();

}

void readToArray(byte output[], MFRC522& _reader) {
  //Save reader's UID data to array
  for (int i = 0; i < 4; i++) {
    output[i] = _reader.uid.uidByte[i];
  }
}

String makeUidString(byte in[]) {
  //Convert each hex digit into characters
  String out;
  char temp[2];
  for (int i = 0; i < 4; i++) {
    //Convert hex -> char
    sprintf(temp, "%02x", in[i]);
    //Append to output string
    out += temp;
  }
  return out;

}

String readID(MFRC522& _reader) {
  //Use the reader to read the ID and return the string of the UID
  byte uidBytes[4];
  readToArray(uidBytes, _reader);
  return makeUidString(uidBytes);
}

String wolfData(const byte wolfData[], const String& id) {
  //Deserialize inupt and dump output to serial
  String uid = sliceArray(wolfData, 0, 7);
  char species_r = wolfData[8];
  String species;
  //Expand species char to full string
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
  //Expand sex char to full string
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
  //Format data to output string
  sprintf(data_out, "UID: %s\nSpecies: %s\nSex: %s\nWeight: %d lbs\nLast Seen: %s days\nScan Count: %d", uid.c_str(), species.c_str(), sex.c_str(), weight, last_seen.c_str(), scan_count);
  //Serial.println(data_out);
  return data_out;
}

String sliceArray(const byte blob[], const int& start, const int& end){
  //Return a new string with a slice of the given array
  String str;
  char temp[8];
  for(int i=start; i<=end; i++){
    //Format byte -> char
    sprintf(temp, "%c", blob[i]);
    //Append to output
    str += temp;
  }
  return str;
}

String findInArray(const byte blob[], const String& id){
  //Search for id in the blob
  String frame;
  for(int i=0; i<1024; i+=21){
    //Get the frame (21 chars)
    frame = sliceArray(blob, i, i+20);
    Serial.print("Searching Frame: ");
    Serial.println(frame);
    //Get ID from frame
    String temp = sliceArray(frame.c_str(), 0, 7);
    //Check if the ID is equal to the one we are looking for
    if(temp == id){
      //Stop searching and return frame
      Serial.println("Found ID! Printing Data...");
      break;
    }else{
      //Search the next frame
      Serial.println("ID not found, continuing to next frame.");
    }
  }
  return frame;
}

int findCountHandle(const byte blob[], const String& id){
  //Searches the blob and returns the position of the scan count in EEPROm
  String frame;
  for(int i=0; i<1024; i+=21){
    //Get current frame
    frame = sliceArray(blob, i, i+20);
    String temp = sliceArray(frame.c_str(), 0, 7);
    //If the ID matches what we are looking for, returnt he position of the scan count
    if(temp == id){
      return i+17;
    }
  }
}

int incrementScanCount(const String& id){
  //Increment (add 1) to the scan count of the given ID
  char temp[3];
  //Get handle
  const int handle = findCountHandle(arr, id);
  //Copy the current count to temporary array
  temp[0] = arr[handle];
  temp[1] = arr[handle + 1];
  temp[2] = arr[handle + 2];
  //Convert char -> integer and add 1
  int temp_i = atoi(temp);
  int rvalue = temp_i + 1;
  //Convert int -> char (with padding 0)
  sprintf(temp, "%03d", rvalue);
  //Copy the incremented value back into the array
  arr[handle] = temp[0];
  arr[handle + 1] = temp[1];
  arr[handle + 2] = temp[2];
  return rvalue;
}
