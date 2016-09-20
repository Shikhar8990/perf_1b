#include <stdio.h>    
#include <iostream>   
#include <stdlib.h>  
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "BaseCache.h"
#include "CacheL1.h"
#include "CacheL2.h"

#define ADDR_SIZE 64
#define DEBUG_1 1
#define DEBUG_2 1

using namespace std;
CacheL1 mCacheL1;
CacheL2 mCacheL2;

bool checkPowof2(int num) {
  int numBits = 0;
  while(num && numBits <=1) {
    if((num & 0x1) == 1) 
      numBits++;
     num>>= 1;
  }
  return(numBits==1);
}

int main(int argc, char **argv) {
  ifstream inFile(argv[1]);
  string line;
  bool g_Debug = 1;
  int mem_lat=0;
  while(getline(inFile, line)) {
    //istringstream iss(line);
    char *seg = NULL;
    char *line_tr = &line[0];
    int val, cnt=0;
    string parm;
    if(strlen(line_tr)>1) {
      seg = strtok(line_tr, " :");
      while(seg != NULL) {
        //string temp(seg);
        if(cnt==0) parm = string(seg);
        if(cnt==1) val = atoi(seg);
        seg = strtok(NULL, " :");
        cnt++;
      }
      if(parm == "l1_cap") {
        if(!checkPowof2(val)) {
          cout<<" ERROR: Parameter not a power of 2 "<<parm<<endl;
          exit(0);
        }
        mCacheL1.setCacheSize(val*1024);
      } else if(parm == "l1_assoc") {
        if(!checkPowof2(val)) {
          cout<<" ERROR: Parameter not a power of 2 "<<parm<<endl;
          exit(0);
        }
        mCacheL1.setAssoc(val);
      } else if(parm == "l1_cline") {
        if(!checkPowof2(val)) {
          cout<<" ERROR: Parameter not a power of 2 "<<parm<<endl;
          exit(0);
        } 
        mCacheL1.setBlockSize(val);
        mCacheL2.setBlockSize(val);
      } else if(parm == "l1_hlat") {
        mCacheL1.setHitLatency(val);
      } else if(parm == "l2_hlat") {
        mCacheL2.setHitLatency(val); 
      } else if(parm == "mem_lat") {
        mCacheL1.setMemLatency(0);
        mCacheL2.setMemLatency(val);
        mem_lat = val;
      } else if(parm == "l2_cap") {
        if(!checkPowof2(val)) {                                   
          cout<<" ERROR: Parameter not a power of 2 "<<parm<<endl;
          exit(0);                                                
        }                                                         
        mCacheL2.setCacheSize(val*1024*1024);
      } else if(parm == "l2_assoc") {
         if(!checkPowof2(val)) {                                   
           cout<<" ERROR: Parameter not a power of 2 "<<parm<<endl;
           exit(0);                                                
         }                                                         
         mCacheL2.setAssoc(val);                                   
      } else if(parm == "l2_cline") {
        if(!checkPowof2(val)) {                                   
          cout<<" ERROR: Parameter not a power of 2 "<<parm<<endl;
          exit(0);                                                
        }                                                         
        mCacheL2.setBlockSize(val);                               
      }  
    }
  }
  mCacheL1.init_Cache();
  mCacheL2.init_Cache();
  ifstream inFile1(argv[2]);
  char *line_tr = NULL;
  long cycle;
  while(getline(inFile1, line)) {
    int cnt=0, r_w=0;
    long long addr;
    char *line_tr = &line[0];
    char *seg = NULL;
    if(strlen(line_tr)>1) {
      seg = strtok(line_tr, ",");
      while(seg != NULL) {
        string temp(seg);
        istringstream convert(temp);
        if(cnt == 0) cycle = strtol(seg, NULL, 16);
        else if(cnt == 1) r_w = atoi(seg);
        else if(cnt == 2) addr = strtol(seg, NULL, 16);
        seg = strtok(NULL, ",");
        cnt++;
      }
      mCacheL1.processActiveReqs(cycle);
      mCacheL2.processActiveReqs(cycle);
      mCacheL1.processWriteback(cycle);
      mCacheL2.processWriteback(cycle);
      if(r_w == 1) {
        if(g_Debug) cout<<"Cyc:"<<hex<<cycle<<dec<<" Read Request for Addr:"<<hex<<addr<<dec<<endl; 
        mCacheL1.readAddr(addr, cycle);
      }
      else {
        if(g_Debug) cout<<"Cyc:"<<hex<<cycle<<dec<<" Write Request for Addr:"<<hex<<addr<<dec<<endl; 
        mCacheL1.writeAddr(addr, cycle);
      }
    }
  }
  mCacheL1.processActiveReqs(cycle+mem_lat);
  mCacheL2.processActiveReqs(cycle+mem_lat);
  mCacheL1.processWriteback(cycle+mem_lat);
  mCacheL2.processWriteback(cycle+mem_lat);

  if(g_Debug) cout<<" Results L1 "<<endl<<" Hits: "<<mCacheL1.getHits()<<endl<<" Misses: "
                  <<mCacheL1.getMisses()<<endl<<" Latency: "<<mCacheL1.getTotalLatency()
                  <<endl<<" References: "<<mCacheL1.getReferences()<<endl;
  if(g_Debug) cout<<" Results L2 "<<endl<<" Hits: "<<mCacheL2.getHits()<<endl<<" Misses: "
                  <<mCacheL2.getMisses()<<endl<<" Latency: "<<mCacheL2.getTotalLatency()
                  <<endl<<" References: "<<mCacheL2.getReferences()<<endl;
  cout<<"L1 hit rate: "<<fixed<<setprecision(2)<<(float(mCacheL1.getHits())/(float(mCacheL1.getReferences())))<<endl
      <<"L2 hit rate: "<<fixed<<setprecision(2)<<(float(mCacheL2.getHits())/(float(mCacheL2.getReferences())))<<endl
      <<"Total latency: "<<mCacheL1.getTotalLatency()+mCacheL2.getTotalLatency()<<endl
      <<"L1 references: "<<mCacheL1.getReferences()<<endl
      <<"L2 references: "<<mCacheL2.getReferences()<<endl
      <<"AMAT: "<<fixed<<setprecision(2)<<((float(mCacheL1.getTotalLatency()+mCacheL2.getTotalLatency()))/(float(mCacheL1.getReferences())))<<endl;
  return 0;
}
