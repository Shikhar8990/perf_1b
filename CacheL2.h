#ifndef CacheL2_h
#define CacheL2_h

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <deque>
#include "CacheLine.h"
#include "BaseCache.h"

using namespace std;

class CacheL2: public BaseCache {
  public:
    void readAddr(long long inAddr, long inCyc);
    void writeAddr(long long inAddr, long inCyc);
    void sendMemReq(long long inAddr, int inSet, int inWay, int inDirty, long inCyc);
    void processActiveReloads(long inCyc);
};

#endif
