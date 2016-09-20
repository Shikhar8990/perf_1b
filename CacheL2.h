#ifndef CacheL2_h
#define CacheL2_h

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <deque>
#include "CacheLine.h"
#include "BaseCache.h"
#include "CacheL1.h"

using namespace std;

class CacheL2: public BaseCache {
  public:
    void readAddr(long long inAddr, long inCyc);
    void writeAddr(long long inAddr, long inCyc);
    void processActiveReqs(long inCyc);
    void processWriteback(long inCyc);
};

#endif
