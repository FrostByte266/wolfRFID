#ifndef WOLFRFID_H
  #define WOLFRFID_H

class wolf{
public:
  wolf();
  void dataFromBlob(const int& position, const char* blob[]);
private:
  unsigned char species;
  unsigned int weight;

};

#endif
