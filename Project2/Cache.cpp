#include "Cache.h"

string Cache::DM(int cacheSize) {
    int numBlock;
    switch(cacheSize) {
        case 1:
            numBlock = 32;
            break;
        case 2:
            numBlock = 128;
            cacheSize = 4;
            break;
        case 3:
            numBlock = 512;
            cacheSize = 16;
            break;
        case 4:
            numBlock = 1024;
            cacheSize = 32;
            break;
    }
    int hits = 0;
    int access = addresses.size();

    int lineSize = 32;
    vector<Package> table;
    table.resize(numBlock);
    for(int i = 0; i < addresses.size(); i++) {
        int index = (addresses[i].address / lineSize) % numBlock;
        if(table.at(index).inRange(addresses[i].address)) {
            table[index].hit = 1;
            hits++;
            continue;
        }
        table[index].hit = 0;
        table[index].content = addresses[i].address;
    }
    //(address / lineSize) % numBlock

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Direct Mapped (x=number of cache hits, y=number of memory accesses)
};

string Cache::SA(int Assoc) {
    int hits = 0;
    int access = 0;

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative 
};

string Cache::FA(int policy) {
    int hits = 0;
    int access = 0;

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Fully Associative (LRU) and (Hot-Cold LRU)
};

string Cache::SANA(int Assoc) {
    int hits = 0;
    int access = 0;

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with No allocation on a write miss
};

string Cache::SANP(int Assoc) {
    int hits = 0;
    int access = 0;

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with next-line prefetching
};

string Cache::PF(int Assoc) {
    int hits = 0;
    int access = 0;

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with next-line prefetching on every miss
};

void Cache::pushBack(char ins, long addr) {
    addresses.push_back(Address(ins, addr));
};