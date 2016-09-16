#include "CacheL2.h"

using namespace std;

void CacheL2::readAddr(long long inAddr, long inCyc) { 
  int way=0, set=0, valid=0;
  mReferences++;
  if(searchTag(inAddr, set, way)) {
    if(g_Debug) cout<<" Cache hit 0x"<<hex<<inAddr<<dec<<endl;
    mHits++;
    mLatency+=mHitLat;
  } else {
    if(g_Debug) cout<<" Cache miss 0x"<<hex<<inAddr<<dec<<endl;
    way = getWaytoAllocate(inAddr);
    set = getSet(inAddr);
    mMisses++;
    mLatency+=mMemLat;
    mCache[set][way].resetValid();
    sendMemReq(inAddr, set, way, 0, inCyc);
  }
  updateLRU(set, way);
}

void CacheL2::writeAddr(long long inAddr, long inCyc) {
  int way=0, set=0, valid=0;
  mReferences++;
  if(searchTag(inAddr, set, way)) { //hit
    if(g_Debug) cout<<" Cache hit 0x"<<hex<<inAddr<<dec<<endl;
    mHits++;
    mLatency+=mHitLat;
    mCache[set][way].setDirty();
  } else {
    if(g_Debug) cout<<" Cache miss 0x"<<hex<<inAddr<<dec<<endl;
    way = getWaytoAllocate(inAddr); 
    set = getSet(inAddr);           
    mMisses++;
    mLatency+=mMemLat;
    mCache[set][way].resetValid();
    sendMemReq(inAddr, set, way, 1, inCyc);   
  }
  updateLRU(set, way);
}

void CacheL2::sendMemReq(long long inAddr, int inSet, int inWay, int inDirty, long inCyc) {
  reloadPkt pkt;
  pkt.tag = getTag(inAddr);
  pkt.addr = inAddr;
  pkt.set = inSet;
  pkt.way = inWay;
  pkt.cnt = mMemLat;
  pkt.cyc = inCyc;
  pkt.dirty = inDirty;
  mActiveRld.push_front(pkt);
  if(g_Debug) cout<<" Mem Req for Addr: 0x"<<hex<<inAddr<<" Set: 0x"<<inSet<<" Way: 0x"<<inWay<<dec<<endl;
}

void CacheL2::processActiveReloads(long inCyc) {
  if(g_Debug) cout<<"---------------------------------"<<endl;
  if(g_Debug) cout<<" isEmpty "<<mActiveRld.empty()<<endl;
  if(!mActiveRld.empty()) {
    if(g_Debug) cout<<" HereA: Size "<<mActiveRld.size()<<endl;
    deque<reloadPkt>::iterator it = mActiveRld.begin();
    deque<reloadPkt>::iterator temp;

    while((it != mActiveRld.end()) && (mActiveRld.size()!=0)) {
      if(g_Debug) cout<<" Here Addr: 0x"<<hex<<it->addr<<dec<<endl;
      if(g_Debug) cout<<" Here: Sent 0x"<<hex<<(it->cyc)<<" Write 0x"<<((it->cyc)+(mMemLat))<<" Curr cyc: 0x"<<inCyc<<dec<<endl;
      if(((it->cyc)+long(mMemLat)) <= inCyc) {
        allocateLine(it->addr, it->tag, it->set, it->way, it->dirty);
        if(g_Debug) cout<<" Here1: Erased 0x"<<hex<<it->tag<<dec<<endl;
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
