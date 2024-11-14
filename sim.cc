
#include "includes.h"
#include "sim.h"
#include "defines.h"
#include "cacheModule.h"
#include "addressParser.h"
#include "structures.h"


/*  "argc" holds the number of command-line arguments.
    "argv[]" holds the arguments themselves.

    Example:
    ./sim 32 8192 4 262144 8 3 10 gcc_trace.txt
    argc = 9
    argv[0] = "./sim"
    argv[1] = "32"
    argv[2] = "8192"
    ... and so on
*/
int main (int argc, char *argv[]) {
   FILE *fp;			// File pointer.
   char *trace_file;		// This variable holds the trace file name.
   cache_params_t params;	// Look at the sim.h header file for the definition of struct cache_params_t.
   char rw;			// This variable holds the request's type (read or write) obtained from the trace.
   uint32_t addr;		// This variable holds the request's address obtained from the trace.
				// The header file <inttypes.h> above defines signed and unsigned integers of various sizes in a machine-agnostic way.  "uint32_t" is an unsigned integer of 32 bits.
   bool request; //0 - read, 1 - write

   CacheModule* L1;
   CacheModule* L2;

   // Exit with an error if the number of command-line arguments is incorrect.
   if (argc != 9) {
      printf("Error: Expected 8 command-line arguments but was provided %d.\n", (argc - 1));
      exit(EXIT_FAILURE);
   }
    
   // "atoi()" (included by <stdlib.h>) converts a string (char *) to an integer (int).
   params.BLOCKSIZE = (uint32_t) atoi(argv[1]);
   params.L1_SIZE   = (uint32_t) atoi(argv[2]);
   params.L1_ASSOC  = (uint32_t) atoi(argv[3]);
   params.L2_SIZE   = (uint32_t) atoi(argv[4]);
   params.L2_ASSOC  = (uint32_t) atoi(argv[5]);
   params.PREF_N    = (uint32_t) atoi(argv[6]);
   params.PREF_M    = (uint32_t) atoi(argv[7]);
   trace_file       = argv[8];

   // Open the trace file for reading.
   fp = fopen(trace_file, "r");
   if (fp == (FILE *) NULL) {
      // Exit with an error if file open failed.
      printf("Error: Unable to open file %s\n", trace_file);
      exit(EXIT_FAILURE);
   }
    

   if(params.L1_SIZE == 0){
      cout << "Error, no cache spec provided";
   }
   else{
      if(params.L2_SIZE == 0){
         L1 = new CacheModule("L1", params.L1_SIZE, params.L1_ASSOC, params.BLOCKSIZE, NULL, NULL);
         L1->nextMemHeirarchy = NULL;
         L1->prevMemHeirarchy = NULL;
         if(params.PREF_N !=0){
            L1->instantiatePrefetchModule(params.PREF_N, params.PREF_M, params.BLOCKSIZE);
         }
      }
      else{
         L1 = new CacheModule("L1", params.L1_SIZE, params.L1_ASSOC, params.BLOCKSIZE, NULL, NULL);
         L2 = new CacheModule("L2", params.L2_SIZE, params.L2_ASSOC, params.BLOCKSIZE, L1, NULL);
         L1->nextMemHeirarchy = L2;
         L2->prevMemHeirarchy = L1;

         if(params.PREF_N !=0){
            L2->instantiatePrefetchModule(params.PREF_N, params.PREF_M, params.BLOCKSIZE);
         }

         }  
      }


   while (fscanf(fp, "%c %x\n", &rw, &addr) == 2) {	// Stay in the loop if fscanf() successfully parsed two tokens as specified.
      if (rw == 'r'){
         request = false; 
      }
      else if (rw == 'w'){
         request = true;
      }
      else {
         printf("Error: Unknown request type %c.\n", rw);
	     exit(EXIT_FAILURE);
      }


      ///////////////////////////////////////////////////////
      // Issue the request to the L1 cache instance here.
      ///////////////////////////////////////////////////////
      request == false ? L1->readReq(addr) : L1->writeReq(addr);
    }

        // Print simulator configuration.
   cout << "===== Simulator configuration =====\n";
   cout << "BLOCKSIZE:   " <<   params.BLOCKSIZE << endl;
   cout << "L1_SIZE:     "   << params.L1_SIZE<< endl;
   cout << "L1_ASSOC:    " << params.L1_ASSOC<< endl;
   cout << "L2_SIZE:     " << params.L2_SIZE<< endl;
   cout << "L2_ASSOC:    " << params.L2_ASSOC<< endl;
   cout << "PREF_N:      " << params.PREF_N<< endl;
   cout << "PREF_M:      " << params.PREF_M<< endl;
   cout << "trace_file:  " << trace_file<< endl;
   cout << endl;
   
   
   
   if(params.L1_SIZE == 0){
      cout << "Error, no cache spec provided";
   }
   else{
      if(params.L2_SIZE == 0){

         L1->noOfReadMissesExcludingPrefetchHits = L1->noOfReadMisses - L1->noOfPrefetchReadHits;
         L1->noOfWriteMissesExcludingPrefetchHits = L1->noOfWriteMisses - L1->noOfPrefetchWriteHits;
         L1->missRate = ((double)(L1->noOfReadMissesExcludingPrefetchHits + L1->noOfWriteMissesExcludingPrefetchHits))/((double)(L1->noOfReads + L1->noOfWrites));
         cout << "===== L1 contents =====" << endl;
         L1->printCacheStatus();
         cout << endl;

         if(params.PREF_N !=0){
            cout << "===== Stream Buffer(s) contents =====" << endl;
            L1->myPrefetchModule->printContents();
            cout << endl;
         }

         cout << "===== Measurements =====" << endl;
         cout << "a. L1 reads:" << "\t" << L1->noOfReads << endl;
         cout << "b. L1 read misses:" << "\t" << L1->noOfReadMissesExcludingPrefetchHits << endl;
         cout << "c. L1 writes:" << "\t" << L1->noOfWrites << endl;
         cout << "d. L1 write misses:" << "\t" << L1->noOfWriteMissesExcludingPrefetchHits << endl;
         cout.setf(ios::fixed,ios::floatfield);
         cout.precision(4);
         cout << "e. L1 miss rate:" << "\t" << L1->missRate << endl;
         cout << "f. L1 writebacks:" << "\t" << L1->noOfWritebacks << endl;
         
         if(params.PREF_N !=0){
            cout << "g. L1 prefetches:" << "\t" << L1->myPrefetchModule->prefetchRequests << endl;
         }
         else{
            cout << "g. L1 prefetches:" << "\t" << 0 << endl;
         }
         
         cout << "h. L2 reads (demand):" << "\t" << 0 << endl;
         cout << "i. L2 read misses (demand):" << "\t" << 0 << endl;
         cout << "j. L2 reads (prefetch):" << "\t" << 0 << endl;
         cout << "k. L2 read misses (prefetch):" << "\t" << 0 << endl;
         cout << "l. L2 writes:" << "\t" << 0 << endl;
         cout << "m. L2 write misses:" << "\t" << 0 << endl;
         cout.precision(4);
         cout << "n. L2 miss rate:" << "\t" << 0.0000 << endl;
         cout << "o. L2 writebacks:" << "\t" << 0 << endl;
         cout << "p. L2 prefetches:" << "\t" << 0 << endl;

         if(params.PREF_N !=0){
            cout << "q. memory traffic:" << "\t" << L1->totalMemTraffic + L1->myPrefetchModule->prefetchRequests << endl;
         }
         else{
            cout << "q. memory traffic:" << "\t" << L1->totalMemTraffic << endl;
         }
         
      }
      else{

         L1->noOfReadMissesExcludingPrefetchHits = L1->noOfReadMisses - L1->noOfPrefetchReadHits;
         L1->noOfWriteMissesExcludingPrefetchHits = L1->noOfWriteMisses - L1->noOfPrefetchWriteHits;
         L1->missRate = ((double)(L1->noOfReadMissesExcludingPrefetchHits + L1->noOfWriteMissesExcludingPrefetchHits))/((double)(L1->noOfReads + L1->noOfWrites));
         
         L2->noOfReadMissesExcludingPrefetchHits = L2->noOfReadMisses - L2->noOfPrefetchReadHits;
         L2->noOfWriteMissesExcludingPrefetchHits = L2->noOfWriteMisses - L2->noOfPrefetchWriteHits;
         L2->missRate = ((double)(L2->noOfReadMissesExcludingPrefetchHits))/((double)(L2->noOfReads));
         

         cout << "===== L1 contents =====" << endl;
         L1->printCacheStatus();
         cout << endl;

         cout << "===== L2 contents =====" << endl;
         L2->printCacheStatus();
         cout << endl;

         if(params.PREF_N !=0){
            cout << "===== Stream Buffer(s) contents =====" << endl;
            L2->myPrefetchModule->printContents();
            cout << endl;
         }
         
         cout << "===== Measurements =====" << endl;
         cout << "a. L1 reads:" << "\t" << L1->noOfReads << endl;
         cout << "b. L1 read misses:" << "\t" << L1->noOfReadMissesExcludingPrefetchHits << endl;
         cout << "c. L1 writes:" << "\t" << L1->noOfWrites << endl;
         cout << "d. L1 write misses:" << "\t" << L1->noOfWriteMissesExcludingPrefetchHits << endl;

         cout.setf(ios::fixed,ios::floatfield);
         cout.precision(4);

         cout << "e. L1 miss rate:" << "\t" << L1->missRate << endl;
         cout << "f. L1 writebacks:" << "\t" << L1->noOfWritebacks << endl;
         cout << "g. L1 prefetches:" << "\t" << 0 << endl;

         cout << "h. L2 reads (demand):" << "\t" << L2->noOfReads << endl;
         cout << "i. L2 read misses (demand):" << "\t" << L2->noOfReadMissesExcludingPrefetchHits << endl;
         cout << "j. L2 reads (prefetch):" << "\t" << 0 << endl;
         cout << "k. L2 read misses (prefetch):" << "\t" << 0 << endl;
         cout << "l. L2 writes:" << "\t" << L2->noOfWrites << endl;
         cout << "m. L2 write misses:" << "\t" << L2->noOfWriteMissesExcludingPrefetchHits << endl;
         cout.precision(4);
         cout << "n. L2 miss rate:" << "\t" <<  L2->missRate << endl;
         cout << "o. L2 writebacks:" << "\t" << L2->noOfWritebacks << endl;

         if(params.PREF_N !=0){
            cout << "p. L2 prefetches:" << "\t" << L2->myPrefetchModule->prefetchRequests << endl;
            cout << "q. memory traffic:" << "\t" << L2->totalMemTraffic + L2->myPrefetchModule->prefetchRequests << endl;
         }
         else{
            cout << "p. L2 prefetches:" << "\t" << 0 << endl;
            cout << "q. memory traffic:" << "\t" << L2->totalMemTraffic << endl;
         }
         
         
         }  
      }


    return(0);
}
