#include "CacheLine.h"
using namespace std;

CacheLine::CacheLine() {
  mTag=0;
  mLRU=0;
  mDirty=0;
  mValid=0;
}

void CacheLine::reset() {
  mTag=0;  
  mDirty=0;
  mValid=0;
}
