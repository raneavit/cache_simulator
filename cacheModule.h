#include "includes.h"
#include "addressParser.h"
#include "prefetchModule.h"
#include "structures.h"

// cacheModule.h
#ifndef cacheModule_H
#define cacheModule_H

class CacheModule
{
  string id;
  int size, assoc, blockSize;
  uint32_t tag, index, offset;
  int cacheHeight, cacheWidth; //dimensions of cache block
  
  vector<vector<cacheBlock> > cacheArray;

  AddressParser myAddressParser;


public:
  int cacheHits, cacheMisses;
  int noOfReads;
  int noOfReadMisses;
  int noOfPrefetchReadHits;
  int noOfPrefetchWriteHits;
  int noOfReadMissesExcludingPrefetchHits;
  int noOfWrites;
  int noOfWriteMisses;
  int noOfWriteMissesExcludingPrefetchHits;
  double missRate;
  int noOfPrefetchRequests;
  int noOfWritebacks;
  int totalMemTraffic;

  CacheModule* prevMemHeirarchy;
  CacheModule* nextMemHeirarchy;
  PrefetchModule* myPrefetchModule;

  CacheModule(string id, int size_n, int assoc_n, int blockSize_n, CacheModule* prev,  CacheModule* next);
  void reshapeCacheArray(int size, int assoc, int block_offset);
  void readReq(uint32_t address);
  void writeReq(uint32_t address);
  void installBlock(uint32_t address);
  void returnBlock(uint32_t address);
  void evictBlock();
  void writeBack(uint32_t address);
  void setDirtyBit(cacheBlock block);
  void updateLRUCount(cacheBlock block);
  void printCacheStatus();
  void instantiatePrefetchModule(int n, int m, int blockSize);
};

#endif