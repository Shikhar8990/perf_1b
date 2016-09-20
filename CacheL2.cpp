#include "CacheL2.h"

using namespace std;
extern CacheL1 mCacheL1;

void CacheL2::readAddr(long long inAddr, long inCyc) { 
  int way=0, set=0, valid=0;
  mReferences++;
  if(searchTag(inAddr, set, way)) {
    if(g_Debug) cout<<"L2: R-Cache hit 0x"<<hex<<inAddr<<dec<<endl;
    mHits++;
    mLatency+=mHitLat;
  } else {
    if(g_Debug) cout<<"L2: R-Cache miss 0x"<<hex<<inAddr<<dec<<endl;
    way = getWaytoAllocate(inAddr, inCyc);
    set = getSet(inAddr);
    mMisses++;
    mLatency+=mHitLat;
    mLatency+=mMemLat;
    mCache[set][way].resetValid();
    queueReq(inAddr, set, way, 0, inCyc);
  }
  updateLRU(set, way);
}

void CacheL2::writeAddr(long long inAddr, long inCyc) {
  int way=0, set=0, valid=0;
  //mReferences++;
  if(searchTag(inAddr, set, way)) { //hit
    if(g_Debug) cout<<"L2: W-Cache hit 0x"<<hex<<inAddr<<dec<<endl;
    //mHits++;
    //mLatency+=mHitLat;
    mCache[set][way].setDirty();
  } else {
    if(g_Debug) cout<<"L2: W-Cache miss 0x"<<hex<<inAddr<<dec<<endl;
    way = getWaytoAllocate(inAddr, inCyc); 
    set = getSet(inAddr);           
    //mMisses++;
    //mLatency+=mHitLat;
    //mLatency+=mMemLat;
    mCache[set][way].resetValid();
    queueReq(inAddr, set, way, 1, inCyc);   
  }
  updateLRU(set, way);
}

void CacheL2::processActiveReqs(long inCyc) {
  if(!mActiveRld.empty()) {
    deque<reloadPkt>::iterator it = mActiveRld.begin();
    deque<reloadPkt>::iterator temp;
    while((it != mActiveRld.end()) && (mActiveRld.size()!=0)) {
      if(((it->cyc)+long(mMemLat+mHitLat)) <= inCyc) {
        allocateLine(it->addr, it->tag, it->set, it->way, it->dirty);
        mCacheL1.writeL1(it->addr);
        temp = it;
        ++temp;
        mActiveRld.erase(it);
        it = temp;
      } else {
        ++it;
      }
    }
  }
}

void CacheL2::processWriteback(long inCyc) {                               
  if(!mActiveWb.empty()) {                                                 
    deque<reloadPkt>::iterator it = mActiveWb.begin();                     
    deque<reloadPkt>::iterator temp;                                       
    while((it != mActiveWb.end()) && (mActiveWb.size()!=0)) {              
    if(((it->cyc)+long(mHitLat)) <= inCyc) {                             
      if(g_Debug) cout<<"L2: Wb sent to Mem 0x"<<hex<<it->addr<<dec<<endl;
      temp = it;                                                         
      ++temp;                                                            
      mActiveWb.erase(it);                                              
      it = temp;                                                         
      } else {                                                             
        ++it;                                                            
      }                                                                    
    }
  }    
}
