#include <Arduino.h>

#include "wolfRfid.h"

wolf::wolf(const byte _id[4]){
  for(int i=0; i<4; i++){
    id[i] = _id[i];
  }

}


void wolf::dataFromBlob(const int& position, const byte blob[1025], String out[]){

}
