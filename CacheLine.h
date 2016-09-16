#ifndef CacheLine_h
#define CacheLine_h

#include <stdio.h> 
#include <iostream>

class CacheLine {                           
  public:                                   
    CacheLine();
    void reset();    
    void setTag(long long inTag) { mTag = inTag; }
    void setLRU(int inLRU)       { mLRU = inLRU; }
    void setValid()              { mValid = 1; }
    void resetValid()            { mValid = 0; }
    void setDirty()              { mDirty = 1; } 
    void incrementLRU()          { mLRU++; }     
    bool isValid()               { return (mValid==1); }
    long long  getTag()          { return mTag; }
    int  getLRU()                { return mLRU; }
  private:                                  
    long long mTag;                               
    int mLRU;  
    int mDirty;
    int mValid;    
};                                          

#endif
