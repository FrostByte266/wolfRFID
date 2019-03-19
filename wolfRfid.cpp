#include <Arduino.h>

#include "wolfRfid.h"

String sliceArray(const byte blob[], const int& start, const int& end){
  String str;
  char temp[8];
  for(int i=start; i<=end; i++){
    sprintf(temp, "%c", blob[i]);
    str += temp;
  }
  return str;
}

wolf::wolf(const byte _id[4]){
  for(int i=0; i<4; i++){
    id[i] = _id[i];
  }

}

String findInArray(const byte blob[], const String& id){
  String frame;
  for(int i=0; i<1024; i+=21){
    frame = sliceArray(blob, i, i+20);
    Serial.print("Frame: ");
    Serial.println(frame);
    String temp = sliceArray(frame.c_str(), 0, 7);
    if(temp == id){
        break;
    }
  }
  return frame;
}

void wolf::dataFromBlob(const int& position, const byte blob[1025], String out[]){

}
