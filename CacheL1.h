#ifndef CacheL1_h
#define CacheL1_h

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <deque>
#include "CacheLine.h"
#include "BaseCache.h"

using namespace std;

class CacheL1: public BaseCache {
  public:
    void readAddr(long long inAddr, long inCyc);
    void writeAddr(long long inAddr, long inCyc);
    void sendMemReq(long long inAddr, int inSet, int inWay, int inDirty, long inCyc);
    void processActiveReloads(long inCyc);
};

#endif
