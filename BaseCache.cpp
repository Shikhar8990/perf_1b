#include "BaseCache.h"
using namespace std;

BaseCache::BaseCache() {
  mAssoc=4;
  mBlock_size = 256;
  mCache_size = 4096;
  mCache      = NULL;
  mSets       = 0;
  mTagBits    = 0;
  mBlockOffet = 0;
  mIndexBits  = 0;
  mIndexMask  = 0;
  mHitLat     = 2;
  mMemLat     = 100;
  mRead_Reqs  = 0;
  mWrite_Reqs = 0;
  mMisses     = 0;
  mHits       = 0;
  mLatency    = 0;
  mHits       = 0;
  mReferences = 0;
  mActiveRld.clear();
  g_Debug = 1;
} 

void BaseCache::init_Cache() {
  int blocks = mCache_size/mBlock_size;
  int sets = blocks/mAssoc;
  mSets = sets;
  mBlockOffet = log2(mBlock_size);
  mIndexBits = log2(mSets);
  if(g_Debug) cout<<"Cache Parms "<<endl<<" Block Size" <<mBlock_size<<endl<<" Cache Size "<<mCache_size<<endl
      <<" Assoc "<<mAssoc<<endl<<" Number of Lines "<<blocks<<endl<<" Sets "<<sets<<endl
      <<" BLock offset "<<mBlockOffet<<endl<<" index bits "<<mIndexBits<<endl;
  if(g_Debug) cout<<"Mem Latency :"<<mMemLat<<endl;
  for(int x=0; x<mIndexBits;x++) {
    mIndexMask = (mIndexMask<<1)|1;
  }
  if(g_Debug) cout<<" Index Mask "<<mIndexMask<<endl;
  mCache = (CacheLine **)malloc(sets*sizeof(CacheLine *));
  for(int set=0; set<sets; set++) {
    mCache[set] = (CacheLine *)malloc(mAssoc*sizeof(CacheLine));
  }
  //Initializing the LRU counter
  for(int set=0; set<mSets; set++) {
    for(int way=0; way<mAssoc; way++) {
      mCache[set][way].setLRU(way);
    }
  }
  mActiveRld.clear();
}

bool BaseCache::searchTag(long long inAddr, int &inSet, int &inWay) {
  if(g_Debug) cout<<" Searching for Tag 0x"<<hex<<getTag(inAddr)<<dec<<endl;
  inSet = getSet(inAddr);
  for(int way=0; way<mAssoc; way++) {
    if((getTag(inAddr) == mCache[inSet][way].getTag()) && (mCache[inSet][way].isValid())) {
      inWay = way;
      return true;
    }
  }
  return false;
}

void BaseCache::allocateLine(long long inAddr, long long inTag, int inSet, int inWay, int inDirty) {
  mCache[inSet][inWay].reset();
  mCache[inSet][inWay].setTag(inTag);
  mCache[inSet][inWay].setValid();
  if(inDirty == 1) 
    mCache[inSet][inWay].setDirty();
  if(g_Debug) cout<<" Cache Write: "<<hex<<" Addr 0x:"<<inAddr<<" Tag: 0x"<<inTag<<" Set: 0x"<<inSet
      <<" Way: 0x"<<inWay<<" Dirty: "<<inDirty<<dec<<endl;
}

void BaseCache::queueReq(long long inAddr, int inSet, int inWay, int inDirty, long inCyc) {
  reloadPkt pkt;                                                                                   
  pkt.tag = getTag(inAddr);                                                                        
  pkt.addr = inAddr;                                                                               
  pkt.set = inSet;                                                                                 
  pkt.way = inWay;                                                                                 
  pkt.cyc = inCyc;                                                                                 
  pkt.dirty = inDirty;
  pkt.rd_sent = 0; 
  mActiveRld.push_front(pkt);                                                                      
}                                                                                                  
                                                                                                   
void BaseCache::queueWb(long long inAddr, int inSet, int inWay, int inDirty, long inCyc) { 
  reloadPkt pkt;                                                                                   
  pkt.tag = getTag(inAddr);                                                                        
  pkt.addr = inAddr;                                                                               
  pkt.set = inSet;                                                                                 
  pkt.way = inWay;                                                                                 
  pkt.cyc = inCyc;                                                                                 
  pkt.dirty = inDirty;                                                                             
  pkt.rd_sent = 0; 
  mActiveWb.push_front(pkt);                                                                       
}                                                                                                  

int BaseCache::getSet(long long inAddr) {
  inAddr = inAddr>>mBlockOffet;
  return (inAddr&mIndexMask); 
}
int BaseCache::getWaytoAllocate(long long inAddr, long inCyc) {
  int way = 0;
  int set = getSet(inAddr);
  int lru = mCache[set][0].getLRU();
  for(int y=1; y<mAssoc; y++) {
    if(mCache[set][y].getLRU() > lru) {
      lru = mCache[set][y].getLRU();
      way = y;
    }
  }
  if(mCache[set][way].isValid() && mCache[set][way].isDirty()) {
    queueWb(inAddr, set, way, 1, inCyc);
  }
  return way; 
}

void BaseCache::updateLRU(int inSet, int inWay) {
  mCache[inSet][inWay].setLRU(0);
  for(int way=0; way<mAssoc; way++) {
    if(way != inWay) {
      mCache[inSet][way].incrementLRU();
    }
  }
}

long long BaseCache::getTag(long long inAddr) {
  return (inAddr>>(mBlockOffet+mIndexBits));
}
