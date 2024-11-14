// addressParser.h
#ifndef addressParser_H
#define addressParser_H

class AddressParser
{
  int addressBits;
  int tagBits;
  int noOfSets;
  int indexBits;
  int offsetBits;

  int tag;
  int index;
  int offset;


public:  
  AddressParser();
  void calculateBits(int size, int assoc, int blockSize);
  void getTagIndexOffset(uint32_t address, uint32_t* tag, uint32_t* index, uint32_t* offset);
  void reconstructBlockAddress(uint32_t tag, uint32_t index, uint32_t* dummyAddress);
 

};

#endif