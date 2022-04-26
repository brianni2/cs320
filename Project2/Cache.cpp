#include "Cache.h"

string Cache::DM(int cacheSize) {
    int numBlock;
    int tagOffset;
    switch(cacheSize) {
        case 1:
            numBlock = 32;
            tagOffset = 10;
            break;
        case 2:
            numBlock = 128;
            tagOffset = 12;
            break;
        case 3:
            numBlock = 512;
            tagOffset = 14;
            break;
        case 4:
            numBlock = 1024;
            tagOffset = 15;
            break;
    }
    int hits = 0;
    int access = trace.size();

    int lineSize = 32;
    vector<Package> table;
    table.resize(numBlock);
    for(int i = 0; i < trace.size(); i++) {
        int index = (trace[i].address / lineSize) % numBlock;
        long tag = trace[i].address >> tagOffset;
        if(table.at(index).tag == tag) {
            //table[index].hit = 1;
            hits++;
            continue;
        }
        table[index].tag = tag;
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Direct Mapped (x=number of cache hits, y=number of memory accesses)
};

string Cache::SA(int assoc) {
    int lineSize = 32;
    int tagOffset = 14;     //tagOffset for 16KB
    int sets = 16384 / (lineSize * assoc);
    int hits = 0;
    int access = trace.size();

    vector<vector<Package>> table(sets, vector<Package> (assoc));
    for(int i = 0; i < trace.size(); i++) {
        int index = (trace[i].address / lineSize) % sets;
        long tag = trace[i].address >> tagOffset;
        int LRU = 0;
        int miss = 1;
        for(int j = 0; j < assoc; j++) {
            table[index][j].age++;
            if(table[index][j].tag == tag) {        //found match
                hits++;
                miss = 0;
                table[index][j].age = 0;
            }
            if(table[index][j].age > table[index][LRU].age) {       //find oldest entry
                LRU = j;
            }
        }
        if(miss) {
            table[index][LRU].tag = tag;
            table[index][LRU].age = 0;
        }
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative 
};

string Cache::FA(int policy) {
    int lineSize = 32;
    int tagOffset = 14;     //tagOffset for 16KB
    int hits = 0;
    int access = trace.size();

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Fully Associative (LRU) and (Hot-Cold LRU)
};

string Cache::SANA(int assoc) {
    int hits = 0;
    int access = trace.size();

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with No allocation on a write miss
};

string Cache::SANP(int assoc) {
    int hits = 0;
    int access = trace.size();

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with next-line prefetching
};

string Cache::PF(int assoc) {
    int hits = 0;
    int access = trace.size();

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with next-line prefetching on every miss
};

void Cache::pushBack(char ins, long addr) {
    trace.push_back(Address(ins, addr));
};