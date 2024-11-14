// prefetchModule.cpp
#include "includes.h"
#include "prefetchModule.h"
#include "addressParser.h"
#include "defines.h"

PrefetchModule::PrefetchModule(int n, int m, int blockSize_n)
{
    noOfStreams = n;
    streamSize = m;
    blockSize = blockSize_n;
    reshapePrefetchArray(noOfStreams);

    prefetchHits = 0;
    prefetchMisses = 0;
    prefetchRequests = 0;
}

void PrefetchModule::reshapePrefetchArray(int n)
{
    prefetchArray.resize(n);

    for(int i=0; i<noOfStreams; i++){
        prefetchArray[i].id = i;
    }
    
}

void PrefetchModule::updateLRU(streamBuffer x)
{
    int pos = 0;
    streamBuffer temp;
    temp = x;

    for(int i = 0; i<noOfStreams; i++){
        if(prefetchArray[i].id == x.id){
            pos = i;
            break;
        }
    }

    memcpy(&prefetchArray[1], &prefetchArray[0], sizeof(streamBuffer)*pos);

    prefetchArray[0] = temp;    
}

bool PrefetchModule::fetchBlock(uint32_t address, bool cacheStatus)
{
    int blockPos =0;
    int i = 0,j = 0;

    for(i=0; i<noOfStreams; i++){
        if((address >= prefetchArray[i].prefetchStreamStart && address < (prefetchArray[i].prefetchStreamStart + streamSize*blockSize)) && prefetchArray[i].validBit == 1){
            //prefetch hit
            // cout << "Prefetch Hit" << endl;
            //find which block

            while(address >= prefetchArray[i].prefetchStreamStart + j*blockSize){
                j++;
            }
            blockPos = prefetchArray[i].prefetchStreamStart + j*blockSize;  
            prefetchRequests += j;
            prefetchArray[i].prefetchStreamStart = blockPos;
            updateLRU(prefetchArray[i]);
            return true; //prefetch hit
        }
    }

    //prefetch miss
    // cout << "Prefetch Miss" << endl;
    if(cacheStatus == false){
        //miss in cache, miss in streambuf
        
        prefetchRequests += streamSize;
        prefetchArray[noOfStreams-1].prefetchStreamStart = address + blockSize;
        prefetchArray[noOfStreams-1].validBit = 1;
        updateLRU(prefetchArray[noOfStreams-1]);
    }
    else{
        //hit in cache, miss in stream buf
    }
    return false; //prefetch miss


    
}

void PrefetchModule::printContents(){


    int i = 0, j = 0;
    int blockOffsetBits = (log2(blockSize));

    for(i = 0; i < noOfStreams; i++)
    {
        if(prefetchArray[i].validBit == 1){
            for(j = 0; j<streamSize; j++){
                cout << hex << ((prefetchArray[i].prefetchStreamStart + j*blockSize) >> blockOffsetBits) << dec << "\t";
            }
            cout << endl;
        }
         
    }


}



