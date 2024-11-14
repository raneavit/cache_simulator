#include "includes.h"

// structures.h
#ifndef structures_H
#define structures_H

typedef 
struct {
   uint32_t BLOCKSIZE;
   uint32_t L1_SIZE;
   uint32_t L1_ASSOC;
   uint32_t L2_SIZE;
   uint32_t L2_ASSOC;
   uint32_t PREF_N;
   uint32_t PREF_M;
} cache_params_t;

typedef 
struct {
   uint32_t tag;
   uint32_t validBit;
   uint32_t dirtyBit;
   uint32_t LRUCount;
   int row;
   int col;
} cacheBlock;

typedef 
struct {
   int id;
   uint32_t validBit;
   uint32_t prefetchStreamStart;
} streamBuffer;

typedef 
struct {
   int a;
   int b;
   int c;
   int d;
   int e;
   int f;
   int g;
   int h;
   int i;
   int j;
   int k;
   int l;
   int m;
   int n;
   int o;
   int p;
   int q;
} outputStruct;

#endif