CC=g++

all: 
	  $(CC) -o cachesim_l1 main.cpp BaseCache.h BaseCache.cpp CacheLine.h CacheLine.cpp CacheL1.h CacheL1.cpp CacheL2.h CacheL2.cpp
