// addressParser.cpp
#include "includes.h"
#include "addressParser.h"
#include "defines.h"

AddressParser::AddressParser()
{
    addressBits = ADDRESS_SIZE;
    tag = 0;
    index = 0;
    offset = 0;
}

void AddressParser::calculateBits(int size, int assoc, int blockSize)
{
  noOfSets = size/(assoc*blockSize);
  indexBits = log2(noOfSets);
  offsetBits = log2(blockSize);
  tagBits = addressBits - indexBits - offsetBits;
}

void AddressParser::getTagIndexOffset(uint32_t address, uint32_t* tag, uint32_t* index, uint32_t* offset)
{
    //Make all arguments zeros first
    *tag = 0;
    *index = 0;
    *offset = 0;

    for(int i = 0; i<offsetBits; i++){
        *offset = (*offset) | ((address & 0b1) << i);
        address = address >> 1;
    }

    for(int i = 0; i<indexBits; i++){
        *index = (*index) |  ((address & 0b1) << i);
        address = address >> 1;
    }

    *tag = address;
}

void AddressParser::reconstructBlockAddress(uint32_t tag, uint32_t index, uint32_t* dummyAddress)
{
    *dummyAddress = 0;

    for(int i = 0; i<offsetBits; i++){
        index = index << 1;
    }

    *dummyAddress = index | *dummyAddress;

    for(int i = 0; i<offsetBits + indexBits; i++){
        tag = tag << 1;
    }

    *dummyAddress = tag | *dummyAddress;
}