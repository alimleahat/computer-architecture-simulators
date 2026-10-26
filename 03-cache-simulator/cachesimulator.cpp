/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size in bytes)  t=32-s-b
32 bit address (MSB -> LSB): TAG || SET || OFFSET

Tag Bits   : the tag field along with the valid bit is used to determine whether the block in the cache is valid or not.
Index Bits : the set index field is used to determine which set in the cache the block is stored in.
Offset Bits: the offset field is used to determine which byte in the block is being accessed.
*/

#include <algorithm>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss
#define NOWRITEMEM 5 // no write to memory
#define WRITEMEM 6   // write to memory

struct config
{
    int L1blocksize;
    int L1setsize;
    int L1size;
    int L2blocksize;
    int L2setsize;
    int L2size;
};

/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
// You need to define your cache class here, or design your own data structure for L1 and L2 cache

/*
A single cache block:
    - valid bit (is the data in the block valid?)
    - dirty bit (has the data in the block been modified by means of a write?)
    - tag (the tag bits of the address)
    - data (the actual data stored in the block, in our case, we don't need to store the data)
*/
struct CacheBlock
{
    // we don't actually need to allocate space for data, because we only need to simulate the cache action
    // or else it would have looked something like this: vector<number of bytes> Data; 
    bool valid = false;
    bool dirty = false;
    unsigned int tag = 0;
};

/*
A CacheSet:
    - a vector of CacheBlocks
    - a counter to keep track of which block to evict next
*/
struct set
{
    // tips: 
    // Associativity: eg. resize to 4-ways set associative cache    
    vector<CacheBlock> ways;
    int counter = 0;
};

// You can design your own data structure for L1 and L2 cache; just an example here
class cache
{
    // some cache configuration parameters.
    // cache L1 or L2
    int L1ways, L2ways, offset, L1idx, L2idx;
    vector<set> L1, L2;
public:
    cache(int L1blocksize, int L1setsize, int L1size, int L2blocksize, int L2setsize, int L2size){
        // initialize the cache according to cache parameters
        int L1blocks = L1size * 1024 / L1blocksize;
        int L2blocks = L2size * 1024 / L2blocksize;
        if (L1setsize == 0){
            L1ways = L1blocks;
        }
        else {
            L1ways = L1setsize;
        }
        if (L2setsize == 0){
            L2ways = L2blocks;
        }
        else {
            L2ways = L2setsize;
        }
        offset = (int)log2(L1blocksize);
        L1idx = (int)log2(L1blocks / L1ways);
        L2idx = (int)log2(L2blocks / L2ways);
        L1.resize(L1blocks / L1ways);
        L2.resize(L2blocks / L2ways);
        for (int i = 0; i < L1.size(); i++){
            L1[i].ways.resize(L1ways);
        }
        for (int i = 0; i < L2.size(); i++){
            L2[i].ways.resize(L2ways);
        }
    }

    int write(unsigned int addr){
        /*
        step 1: select the set in our L1 cache using set index bits
        step 2: iterate through each way in the current set
            - If Matching tag and Valid Bit High -> WriteHit!
                                                    -> Dirty Bit High
        step 3: Otherwise? -> WriteMiss!

        return WH or WM
        */
        unsigned int L1index = (addr >> offset) & ((1u << L1idx) - 1);
        unsigned int L1tag = addr >> (offset + L1idx);
        for (int i = 0; i < L1ways; i++){
            if (L1[L1index].ways[i].valid && L1[L1index].ways[i].tag == L1tag){
                L1[L1index].ways[i].dirty = true;
                return WH;
            }
        }
        return WM;
    }

    pair<int, int> writeL2(unsigned int addr){
        /*
        step 1: select the set in our L2 cache using set index bits
        step 2: iterate through each way in the current set
            - If Matching tag and Valid Bit High -> WriteHit!
                                                 -> Dirty Bit High
        step 3: Otherwise? -> WriteMiss!

        return {WM or WH, WRITEMEM or NOWRITEMEM}
        */
        unsigned int L2index = (addr >> offset) & ((1u << L2idx) - 1);
        unsigned int L2tag = addr >> (offset + L2idx);
        for (int i = 0; i < L2ways; i++){
            if (L2[L2index].ways[i].valid && L2[L2index].ways[i].tag == L2tag){
                L2[L2index].ways[i].dirty = true;
                return make_pair(WH, NOWRITEMEM);
            }
        }
        return make_pair(WM, WRITEMEM);
    }

    int readL1(unsigned int addr){
        /*
        step 1: select the set in our L1 cache using set index bits
        step 2: iterate through each way in the current set
            - If Matching tag and Valid Bit High -> ReadHit!
        step 3: Otherwise? -> ReadMiss!

        return RH or RM
        */
        unsigned int L1index = (addr >> offset) & ((1u << L1idx) - 1);
        unsigned int L1tag = addr >> (offset + L1idx);
        for (int i = 0; i < L1ways; i++){
            if (L1[L1index].ways[i].valid && L1[L1index].ways[i].tag == L1tag){
                return RH;
            }
        }
        return RM;
    }

    pair<int, int> readL2(unsigned int addr){
        /*
        step 1: select the set in our L2 cache using set index bits
        step 2: iterate through each way in the current set
            - If Matching tag and Valid Bit High -> ReadHit!
                                                 -> copy dirty bit
        step 3: otherwise? -> ReadMiss! -> need to pull data from Main Memory
        step 4: find a place in L1 for our requested data
            - case 1: empty way in L1 -> place requested data
            - case 2: no empty way in L1 -> evict from L1 to L2
                    - case 2.1: empty way in L2 -> place evicted L1 data there
                    - case 2.2: no empty way in L2 -> evict from L2 to memory

        return {RM or RH, WRITEMEM or NOWRITEMEM}
        */
        int L2state = RM;
        int memState = NOWRITEMEM;
        bool dirty = false;

        unsigned int L2index = (addr >> offset) & ((1u << L2idx) - 1);
        unsigned int L2tag = addr >> (offset + L2idx);
        for (int i = 0; i < L2ways; i++){
            if (L2[L2index].ways[i].valid && L2[L2index].ways[i].tag == L2tag){
                L2state = RH;
                dirty = L2[L2index].ways[i].dirty;
                L2[L2index].ways[i].valid = false;
            }
        }

        unsigned int L1index = (addr >> offset) & ((1u << L1idx) - 1);
        unsigned int L1tag = addr >> (offset + L1idx);
        int L1way = -1;
        for (int i = 0; i < L1ways; i++){
            if (!L1[L1index].ways[i].valid){
                L1way = i;
                break;
            }
        }

        if (L1way == -1){
            L1way = L1[L1index].counter;
            L1[L1index].counter = (L1[L1index].counter + 1) % L1ways;

            CacheBlock evicted = L1[L1index].ways[L1way];
            unsigned int evictedAddr = (evicted.tag << (offset + L1idx)) | (L1index << offset);
            unsigned int evictedIndex = (evictedAddr >> offset) & ((1u << L2idx) - 1);
            unsigned int evictedTag = evictedAddr >> (offset + L2idx);

            int L2way = -1;
            for (int i = 0; i < L2ways; i++){
                if (!L2[evictedIndex].ways[i].valid){
                    L2way = i;
                    break;
                }
            }

            if (L2way == -1){
                L2way = L2[evictedIndex].counter;
                L2[evictedIndex].counter = (L2[evictedIndex].counter + 1) % L2ways;
                if (L2[evictedIndex].ways[L2way].dirty){
                    memState = WRITEMEM;
                }
            }

            L2[evictedIndex].ways[L2way].valid = true;
            L2[evictedIndex].ways[L2way].dirty = evicted.dirty;
            L2[evictedIndex].ways[L2way].tag = evictedTag;
        }

        L1[L1index].ways[L1way].valid = true;
        L1[L1index].ways[L1way].dirty = dirty;
        L1[L1index].ways[L1way].tag = L1tag;

        return make_pair(L2state, memState);
    }
};

// Tips: another example:
/*
class CacheSystem
{
    Cache L1;
    Cache L2;
public:
    int L1AcceState, L2AcceState, MemAcceState;
    auto read(auto addr){};
    auto write(auto addr){};
};
class Cache
{
    set * CacheSet;
public:
    auto read_access(auto addr){};
    auto write_access(auto addr){};
    auto check_exist(auto addr){};
    auto evict(auto addr){};
};
*/
/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/





int main(int argc, char *argv[])
{

    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while (!cache_params.eof())                   // read config file
    {
        cache_params >> dummyLine;                // L1:
        cache_params >> cacheconfig.L1blocksize;  // L1 Block size
        cache_params >> cacheconfig.L1setsize;    // L1 Associativity
        cache_params >> cacheconfig.L1size;       // L1 Cache Size
        cache_params >> dummyLine;                // L2:
        cache_params >> cacheconfig.L2blocksize;  // L2 Block size
        cache_params >> cacheconfig.L2setsize;    // L2 Associativity
        cache_params >> cacheconfig.L2size;       // L2 Cache Size
    }
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    string line;
    string accesstype;     // the Read/Write access type from the memory trace;
    string xaddr;          // the address from the memory trace store in hex;
    unsigned int addr;     // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;




/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
    // Implement by you:
    // initialize the hirearch cache system with those configs
    // probably you may define a Cache class for L1 and L2, or any data structure you like
    if (cacheconfig.L1blocksize!=cacheconfig.L2blocksize){
        printf("please test with the same block size\n");
        return 1;
    }
    cache c1(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size,
             cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);

    int L1AcceState = NA; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState = NA; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
    int MemAcceState = NOWRITEMEM; // Main Memory access state variable, can be either NA or WH;

    if (traces.is_open() && tracesout.is_open())
    {
        while (getline(traces, line))
        { // read mem access file and access Cache

            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)){
                break;
            }
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32>(addr);

            // access the L1 and L2 Cache according to the trace;
            if (accesstype.compare("R") == 0)  // a Read request
            {
                // Implement by you:
                //   read access to the L1 Cache,
                //   and then L2 (if required),
                //   update the access state variable;
                //   return: L1AcceState L2AcceState MemAcceState
                
                // For example:
                // L1AcceState = cache.readL1(addr); // read L1
                // if(L1AcceState == RM){
                //     L2AcceState, MemAcceState = cache.readL2(addr); // if L1 read miss, read L2
                // }
                // else{ ... }
                L1AcceState = c1.readL1(addr);
                L2AcceState = NA;
                MemAcceState = NOWRITEMEM;
                if (L1AcceState == RM) {
                    pair<int, int> r = c1.readL2(addr);
                    L2AcceState = r.first;
                    MemAcceState = r.second;
                }
            }
            else{ // a Write request
                // Implement by you:
                //   write access to the L1 Cache, or L2 / main MEM,
                //   update the access state variable;
                //   return: L1AcceState L2AcceState

                // For example:
                // L1AcceState = cache.writeL1(addr);
                // if (L1AcceState == WM){
                //     L2AcceState, MemAcceState = cache.writeL2(addr);
                // }
                // else if(){...}
                L1AcceState = c1.write(addr);
                L2AcceState = NA;
                MemAcceState = NOWRITEMEM;
                if (L1AcceState == WM) {
                    pair<int, int> r = c1.writeL2(addr);
                    L2AcceState = r.first;
                    MemAcceState = r.second;
                }
            }
/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/




            // Grading: don't change the code below.
            // We will print your access state of each cycle to see if your simulator gives the same result as ours.
            tracesout << L1AcceState << " " << L2AcceState << " " << MemAcceState << endl; // Output hit/miss results for L1 and L2 to the output file;
        }
        traces.close();
        tracesout.close();
    }
    else
        cout << "Unable to open trace or traceout file ";

    return 0;
}
