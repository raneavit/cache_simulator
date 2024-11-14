// cacheModule.cpp
#include "includes.h"
#include "cacheModule.h"
#include "defines.h"

CacheModule::CacheModule(string myId, int size_n, int assoc_n, int blockSize_n, CacheModule* prev,  CacheModule* next)
{
    id = myId;
    cacheHits = 0;
    cacheMisses = 0;
    prevMemHeirarchy = prev;
    nextMemHeirarchy = next;
    size = size_n;
    assoc = assoc_n;
    blockSize = blockSize_n;
    noOfReads  = 0;
    noOfReadMisses = 0;
    noOfPrefetchReadHits = 0;
    noOfPrefetchWriteHits = 0;
    noOfReadMissesExcludingPrefetchHits = 0;
    noOfWrites = 0;
    noOfWriteMisses = 0;
    noOfWriteMissesExcludingPrefetchHits = 0;
    missRate = 0;
    noOfPrefetchRequests = 0;
    noOfWritebacks = 0;
    totalMemTraffic = 0;          
    myPrefetchModule = NULL;            
    myAddressParser.calculateBits(size, assoc, blockSize);
    reshapeCacheArray(size, assoc, blockSize_n);

}

void CacheModule::reshapeCacheArray(int size, int assoc, int blockSize)
{
    int i = 0, j = 0;
    cacheHeight = size/(assoc*blockSize); // no. of rows = no. of sets
    cacheWidth =  assoc; //no. of columns = associativity

    cacheArray.resize(cacheHeight,  vector<cacheBlock> (cacheWidth));

    //initialize lru array with values <0,1,2,3...>
    for(i = 0; i < cacheHeight; i++)
    {
        for(j = 0; j < cacheWidth; j++)
        {
            cacheArray[i][j].LRUCount = j;
            cacheArray[i][j].col = j;
            cacheArray[i][j].row = i;
        }
    }

}

void CacheModule::updateLRUCount(cacheBlock block){

    int temp;
    temp =  block.LRUCount;

    for(int j=0; j<cacheWidth; j++){ //increment any counter lower than current accessed
        if(cacheArray[block.row][j].LRUCount < temp){
            cacheArray[block.row][j].LRUCount += 1;
        }
    }

    cacheArray[block.row][block.col].LRUCount = 0; //make it the most recently used
}


void CacheModule::returnBlock(uint32_t address)
{

    if(prevMemHeirarchy != NULL){
        prevMemHeirarchy->installBlock(address);
        prevMemHeirarchy->returnBlock(address);
    }
    else{
        return;
    }
}

void CacheModule::installBlock(uint32_t address)
{
    myAddressParser.getTagIndexOffset(address, &tag, &index, &offset);
    
    for(int i=0 ; i<cacheWidth; i++){
        if(cacheArray[index][i].LRUCount == (cacheWidth - 1)){//find least recently used
            if(cacheArray[index][i].validBit == 0){
                cacheArray[index][i].validBit = 1;
                cacheArray[index][i].tag = tag;
                cacheArray[index][i].dirtyBit = 0;
                updateLRUCount(cacheArray[index][i]);
                break;
            }
            else{
                // evictBlock(cacheArray[index][i]);
                cacheArray[index][i].tag = tag;
                cacheArray[index][i].dirtyBit = 0;
                updateLRUCount(cacheArray[index][i]);
                break;
            }
        }
    }
    return;
}

void CacheModule::evictBlock()
{
    uint32_t blockAddress;

    for(int i=0 ; i<cacheWidth; i++){
        if(cacheArray[index][i].LRUCount == (cacheWidth - 1)){//find least recently used
            if(cacheArray[index][i].dirtyBit == 1){
                myAddressParser.reconstructBlockAddress(cacheArray[index][i].tag, cacheArray[index][i].row, &blockAddress);
                writeBack(blockAddress);
            }
        }
    }
}

void CacheModule::writeBack(uint32_t address)
{
    CacheModule* next;
    next = nextMemHeirarchy;
    if(next != NULL){
        noOfWritebacks += 1;
        next->myAddressParser.getTagIndexOffset(address, &next->tag, &next->index, &next->offset);
        next->writeReq(address);
    }

    else{
        //write back to mem += 1
        noOfWritebacks += 1;
        totalMemTraffic += 1;
    }

}

void CacheModule::readReq(uint32_t address)
{
    uint32_t blockAddress;

    //find in my tag array
    // if not in my tag array find in next level

    noOfReads += 1;
    myAddressParser.getTagIndexOffset(address, &tag, &index, &offset);

    for(int i=0; i<cacheWidth; i++){
        if(tag == cacheArray[index][i].tag && cacheArray[index][i].validBit == 1){
            //cache hit
            cacheHits += 1;
            updateLRUCount(cacheArray[index][i]);

            if(myPrefetchModule != NULL){
                myAddressParser.reconstructBlockAddress(tag, index, &blockAddress);
                bool prefetchStatus = myPrefetchModule->fetchBlock(blockAddress, true);
            }

            returnBlock(address);
            return;
        }
    }

    //cache miss
    cacheMisses += 1;
    noOfReadMisses+=1;
    evictBlock();

    if(myPrefetchModule != NULL){
        myAddressParser.reconstructBlockAddress(tag, index, &blockAddress);
        bool prefetchStatus = myPrefetchModule->fetchBlock(blockAddress, false);

        if(prefetchStatus == true){ // prefetch hit, cache miss
            noOfPrefetchReadHits +=1;
            installBlock(address);
            returnBlock(address);
            return;
        }
    }

    if(nextMemHeirarchy != NULL){
        nextMemHeirarchy->readReq(address);
        return;
    }
    else{
        //take block from memory
        totalMemTraffic += 1;
        installBlock(address);
        returnBlock(address);
        return;
    }
    
}


void CacheModule::writeReq(uint32_t address)
{

    uint32_t blockAddress;
    //find in my tag array
    // if not in my tag array find in next level
    myAddressParser.getTagIndexOffset(address, &tag, &index, &offset);

    noOfWrites += 1;
    
    for(int i=0; i<cacheWidth; i++){
        if(tag == cacheArray[index][i].tag && cacheArray[index][i].validBit == 1){
            //cache hit
        
            cacheHits += 1;
            updateLRUCount(cacheArray[index][i]);
            cacheArray[index][i].dirtyBit = 1; //set dirty bit

            if(myPrefetchModule != NULL){
                myAddressParser.reconstructBlockAddress(tag, index, &blockAddress);
                bool prefetchStatus = myPrefetchModule->fetchBlock(blockAddress, true);
   
            }

            return;
        }
    }

    //cache miss
    noOfWriteMisses += 1;
    cacheMisses += 1;
    evictBlock();

    //check in prefetch
    if(myPrefetchModule != NULL){
            myAddressParser.reconstructBlockAddress(tag, index, &blockAddress);
            bool prefetchStatus = myPrefetchModule->fetchBlock(blockAddress, false);
            if(prefetchStatus == true){ // prefetch hit, cache miss
                noOfPrefetchWriteHits += 1;
                installBlock(address);
                for(int i=0; i < cacheWidth; i+=1){
                    if(cacheArray[index][i].tag == tag){
                        cacheArray[index][i].dirtyBit = 1;
                    }
                }
                return;
            }
    }

    if(nextMemHeirarchy != NULL){
        nextMemHeirarchy->readReq(address);
        //set dirty bit
        for(int i=0; i < cacheWidth; i+=1){
            if(cacheArray[index][i].tag == tag){
                cacheArray[index][i].dirtyBit = 1;
            }
        }
        return;
    }
    else{
    
        //memory is acccessed, install block from memory
        totalMemTraffic += 1;
        installBlock(address);
        for(int i=0; i < cacheWidth; i+=1){
            if(cacheArray[index][i].tag == tag){
                cacheArray[index][i].dirtyBit = 1;
            }
        }
        // returnBlock(address);
        // cacheArray[index][i].dirtyBit = 1; //set dirty bit, copy is dirty wrt memory
        return;
    }

    return;
}

void CacheModule::printCacheStatus(){


    int i = 0, j = 0, currentLRU = 0, checkValidityofSet = 0;
    bool printSet;

    for(i = 0; i < cacheHeight; i++)
    {
        printSet = true;
        checkValidityofSet = 0;
        for(j=0; j<cacheWidth; j++){
            if(cacheArray[i][j].validBit == 0) checkValidityofSet+=1;
        }
        if(checkValidityofSet == cacheWidth-1) printSet = false;

        if(printSet = false){
            break;
        }

        else{
            cout << "set\t" << i << ":\t";
            currentLRU = 0;
            j = 0;
            while(j < cacheWidth)
            {
                if(cacheArray[i][j].LRUCount == currentLRU){
                    if(cacheArray[i][j].validBit == 1){
                        cout << hex << cacheArray[i][j].tag << dec << "  ";
                        if(cacheArray[i][j].dirtyBit == 1){
                             cout << "D  ";
                        }
                        else{
                        cout << "   "; 
                        }
                    }
                    else{
                        cout << "\t" << "  ";
                    }
                    
                    currentLRU += 1;
                    j = 0;
                    continue;  
                }
                j+=1;
            }
            cout << endl;
        }    
    }
}

void CacheModule::instantiatePrefetchModule(int n, int m, int blockSize){
    myPrefetchModule = new PrefetchModule(n, m, blockSize);
}







