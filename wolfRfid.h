#ifndef WOLFRFID_H
  #define WOLFRFID_H

#include<Arduino.h>

String sliceArray(const byte blob[], const int& start, const int& end);

class wolf{
public:
  wolf(const byte _id[4]);
  void dataFromBlob(const int& position, const byte blob[1025], String out[]);
private:
  byte species;
  int weight;
  byte id[4];

};

#endif
