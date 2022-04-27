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
    vector<Package> table(numBlock);
    for(int i = 0; i < trace.size(); i++) {
        int index = (trace[i].address / lineSize) % numBlock;
        long tag = trace[i].address >> tagOffset;
        if(table.at(index).tag == tag) {
            hits++;
            continue;
        }
        table[index].tag = tag;
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Direct Mapped (x=number of cache hits, y=number of memory accesses)
};

string Cache::SA(int assoc) {
    //cout << assoc << "-way set associative\n";
    int lineSize = 32;
    int tagOffset = 14;     //tagOffset for 16KB
    int ways = (lineSize * assoc);
    int sets = 16384 / ways;
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

string Cache::FAN() {
    int tagOffset = 14;     //tagOffset for 16KB
    int numBlock = 512;
    int hits = 0;
    int access = trace.size();
    vector<Package> table(numBlock);
    for(int i = 0; i < trace.size(); i++) {
        long tag = trace[i].address >> tagOffset;
        int LRU = 0;
        int miss = 1;
        for(int j = 0; j < numBlock; j++) {
            if(table[j].tag == -1) {
                table[j].tag = tag;
                table[j].age = 0;
                break;
            }
            if(table[j].tag == tag) {
                table[j].age = 0;
                hits++;
                miss = 0;
                break;
            }
            table[j].age++;
            if(table[j].age > table[LRU].age) {
                LRU = j;
            }
        }
        if(miss) {
            table[LRU].tag = tag;
            table[LRU].age = 0;
        }
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Fully Associative (LRU) and (Hot-Cold LRU)
};

string Cache::FAHC() {
    int tagOffset = 14;     //tagOffset for 16KB
    int numBlock = 512;
    int hits = 0;
    int access = trace.size();
    vector<Package> table(numBlock+1);
    for(int i = 0; i < trace.size(); i++) {
        //cout << i+1 << " / " << trace.size() << endl;
        long tag = trace[i].address >> tagOffset;
        int miss = 1;
        for(int j = 1; j < table.size(); j++) {
            if(table[j].tag == tag) {
                table[j].hot = 1;
                if(j%2 == 1) {
                    table[j+1].hot = 0;
                }
                else {
                    table[j-1].hot = 0;
                }
                hits++;
                miss = 0;
                break;
            }
        }
        if(miss) {
            //cout << "Finding coldest path...\n";
            int j = 0;
            while(j < table.size()) {
                int left = (2*j)+1;
                int right = (2*j)+2;
                //cout << "left " << left << " right " << right << endl;
                if(left > table.size() || right < table.size()) {
                    break;
                }
                if(table[left].hot) {
                    j = right;
                    continue;
                }
                else if(table[right].hot){
                    j = left;
                    continue;
                }
            }
            table[j].tag = tag;
            table[j].hot = 1;
            if(j%2 == 1) {
                table[j+1].hot = 0;
            }
            else {
                table[j-1].hot = 0;
            }
            //cout << "Coldest path found\n";
        }
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with No allocation on a write miss
}

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
