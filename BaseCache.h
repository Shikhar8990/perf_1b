#ifndef BaseCache_h
#define BaseCache_h

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <deque>
#include "CacheLine.h"


using namespace std;

class BaseCache {
  public:
    BaseCache();
    //non virtual functions
    void setAssoc(int inAssoc)         { mAssoc = inAssoc; }
    void setCacheSize(int inCacheSize) { mCache_size = inCacheSize; }
    void setBlockSize(int inBlockSize) { mBlock_size = inBlockSize; }
    void setHitLatency(int inLat)      { mHitLat = inLat; }
    void setMemLatency(int inLat)      { mMemLat = inLat; }
    void init_Cache();
    virtual void readAddr(long long inAddr, long inCyc)=0;
    virtual void writeAddr(long long inAddr, long inCyc)=0;
    void updateLRU(int inSet, int inWay);
    void allocateLine(long long inAddr, long long inTag, int inSet, int inWay, int inDirty);
    bool searchTag(long long inAddr, int &inSet, int &inWay);
    virtual void sendMemReq(long long inAddr, int inSet, int inWay, int inDirty, long inCyc)=0;
    virtual void processActiveReloads(long inCyc)=0;
    int  getWaytoAllocate(long long inAddr);
    int  getSet(long long inAddr);
    long long getTag(long long inAddr);
    long int  getTotalReferences() { return (mRead_Reqs+mWrite_Reqs); }
    long int  getMisses() { return mMisses; }
    long int  getTotalLatency() { return mLatency; }
    long int  getHits() { return mHits; }
    long int  getReferences() { return mReferences; }
    struct reloadPkt {
      long long tag;
      long long addr;
      int set;
      int way;
      int cnt;
      long cyc;
      int dirty;
    };
  protected:  
    bool g_Debug;
    CacheLine **mCache;
    int mAssoc;
    int mSets;
    int mBlock_size;
    int mCache_size;
    int mIndexBits;
    int mIndexMask;
    int mBlockOffet;
    int mTagBits;
    int mMemLat;
    int mHitLat;
    long int mRead_Reqs;
    long int mWrite_Reqs;
    long int mMisses;
    long int mHits;
    long int mLatency;
    long int mReferences;
    deque<reloadPkt> mActiveRld;
};

#endif
