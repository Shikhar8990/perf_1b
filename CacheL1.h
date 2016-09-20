#ifndef CacheL1_h
#define CacheL1_h

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <deque>
#include "CacheLine.h"
#include "BaseCache.h"
#include "CacheL2.h"

using namespace std;

class CacheL1: public BaseCache {
  public:
    void readAddr(long long inAddr, long inCyc);
    void writeAddr(long long inAddr, long inCyc);
    void processActiveReqs(long inCyc);
    void processWriteback(long inCyc);
    void writeL1(long long inAddr);
};

#endif
