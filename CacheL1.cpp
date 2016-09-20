#include "CacheL1.h"

using namespace std;
extern CacheL2 mCacheL2;

void CacheL1::readAddr(long long inAddr, long inCyc) { 
  int way=0, set=0, valid=0;
  mReferences++;
  if(searchTag(inAddr, set, way)) {
    if(g_Debug) cout<<"L1: R-Cache hit 0x"<<hex<<inAddr<<dec<<endl;
    mHits++;
    mLatency+=mHitLat;
  } else {
    if(g_Debug) cout<<"L1: R-Cache miss 0x"<<hex<<inAddr<<dec<<endl;
    way = getWaytoAllocate(inAddr, inCyc);
    set = getSet(inAddr);
    mMisses++;
    mLatency+=mHitLat;
    mCache[set][way].resetValid();
    queueReq(inAddr, set, way, 0, inCyc);
  }
  updateLRU(set, way);
}

void CacheL1::writeAddr(long long inAddr, long inCyc) {
  int way=0, set=0, valid=0;
  mReferences++;
  if(searchTag(inAddr, set, way)) { //hit
    if(g_Debug) cout<<"L1: W-Cache hit 0x"<<hex<<inAddr<<dec<<endl;
    mHits++;
    mLatency+=mHitLat;
    mCache[set][way].setDirty();
  } else {
    if(g_Debug) cout<<"L1: W-Cache miss 0x"<<hex<<inAddr<<dec<<endl;
    way = getWaytoAllocate(inAddr, inCyc); 
    set = getSet(inAddr);           
    mMisses++;
    mLatency+=mHitLat;
    mLatency+=mMemLat;
    mCache[set][way].resetValid();
    //sending dirty as its gonna be dirtied anyway
    queueReq(inAddr, set, way, 1, inCyc);   
  }
  updateLRU(set, way);
}

void CacheL1::processWriteback(long inCyc) {
  if(!mActiveWb.empty()) {
    deque<reloadPkt>::iterator it = mActiveWb.begin();
    deque<reloadPkt>::iterator temp;
    while((it != mActiveWb.end()) && (mActiveWb.size()!=0)) {
      if(((it->cyc)+long(mHitLat)) <= inCyc) {
        if(g_Debug) cout<<"L1: Wb sent to L2 0x"<<hex<<it->addr<<" in Cyc: "<<(it->cyc+2)<<dec<<endl;
        mCacheL2.writeAddr(it->addr, it->cyc+2);
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

void CacheL1::processActiveReqs(long inCyc) {
  if(!mActiveRld.empty()) {
    deque<reloadPkt>::iterator it = mActiveRld.begin();
    deque<reloadPkt>::iterator temp;
    while((it != mActiveRld.end()) && (mActiveRld.size()!=0)) {
      if(((it->cyc)+long(mHitLat)) <= inCyc) {
        if(it->rd_sent==0) {
          if(g_Debug) cout<<"L1: Read sent to L2 0x"<<hex<<it->addr<<" in Cyc:"<<(it->cyc+2)<<dec<<endl;
          mCacheL2.readAddr(it->addr, it->cyc+2);
          it->rd_sent=1;
        }
      }
      ++it;
    } 
  }
}

void CacheL1::writeL1(long long inAddr) {
  if(!mActiveRld.empty()) {
    deque<reloadPkt>::iterator it = mActiveRld.begin();                      
    deque<reloadPkt>::iterator temp;                                         
    while((it != mActiveRld.end()) && (mActiveRld.size()!=0)) {              
      if((it->addr)==inAddr) {                               
        if(g_Debug) cout<<"L1: Write 0x"<<hex<<it->addr<<dec<<endl;
        allocateLine(it->addr, it->tag, it->set, it->way, it->dirty);      
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
