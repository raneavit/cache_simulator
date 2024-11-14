#include "includes.h"
#include "addressParser.h"
#include "structures.h"

// prefetchModule.h
#ifndef prefetchModule_H
#define prefetchModule_H

class PrefetchModule
{
  int streamSize, noOfStreams, blockSize; //dimensions
  vector<streamBuffer> prefetchArray;
  AddressParser myAddressParser;
  


public:
  int prefetchHits, prefetchMisses, prefetchRequests;


  PrefetchModule(int n, int m, int blockSize_n);
  void reshapePrefetchArray(int n);
  void updateLRU(streamBuffer x);
  bool fetchBlock(uint32_t address, bool cacheStatus); //cache status: 0 = miss, 1 = hit
  bool searchStream(streamBuffer x, uint32_t addr);
  void printContents();


};

#endif