#include "Cache.h"

const int LINESIZE = 32;

string Cache::DM(int cacheSize) {
    int numBlock;
    int byteOffset;
    switch(cacheSize) {
        case 1:
            numBlock = 32;
            byteOffset = 10;
            break;
        case 2:
            numBlock = 128;
            byteOffset = 12;
            break;
        case 3:
            numBlock = 512;
            byteOffset = 14;
            break;
        case 4:
            numBlock = 1024;
            byteOffset = 15;
            break;
    }
    int hits = 0;
    int access = trace.size();

    
    vector<Package> table(numBlock);
    for(int i = 0; i < trace.size(); i++) {
        int index = (trace[i].address / LINESIZE) % numBlock;
        long tag = trace[i].address >> byteOffset;
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
    
    int byteOffset = log(16384/assoc);
    //int waySize = (LINESIZE * assoc);
    int sets = 16384 / (LINESIZE * assoc);
    //cout << assoc << " ways | " << sets << " sets | " << byteOffset << " offset\n";
    int hits = 0;
    int access = trace.size();

    vector<vector<Package>> table(sets, vector<Package> (assoc));
    for(int i = 0; i < trace.size(); i++) {
        int index = (trace[i].address / LINESIZE) % sets;
        long tag = trace[i].address >> byteOffset;
        int miss = 1;
        //cout << trace[i].address;
        for(int j = 0; j < assoc; j++) {
            table[index][j].age++;
            if(table[index][j].tag == tag) {        //found match
                hits++;
                miss = 0;
                table[index][j].age = 0;
                //cout << " \tindex[" << index << "][" << j << "] age " << table[index][j].age << endl;
                break;
            }
            if(table[index][j].tag == -1) {
                table[index][j].tag = tag;
                table[index][j].age = 0;
                miss = 0;
                break;
            }
        }
        if(miss) {
            //cout << "\nMISS! Finding LRU entry\n";
            int LRU = 0;
            for(int j = 1; j < assoc; j++) {
                if(table[index][j].age > table[index][LRU].age) {       //find oldest entry
                    //cout << table[index][j].age << " > " << table[index][LRU].age << endl;
                    LRU = j;
                }
            }
            table[index][LRU].tag = tag;
            table[index][LRU].age = 0;
            //cout << " \tindex[" << index << "][" << LRU << "] age " << table[index][LRU].age << endl;
        }
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative 
};

string Cache::FAN() {
    int byteOffset = 5;
    int numBlock = 512;
    int hits = 0;
    int access = trace.size();
    vector<Package> table(numBlock);
    for(int i = 0; i < trace.size(); i++) {
        long tag = trace[i].address >> byteOffset;
        int LRU = 0;
        int miss = 1;
        for(int j = 0; j < numBlock; j++) {
            table[j].age++;
            if(table[j].tag == -1) {
                table[j].tag = tag;
                table[j].age = 0;
                miss = 0;
                break;
            }
            if(table[j].tag == tag) {
                table[j].age = 0;
                hits++;
                miss = 0;
                break;
            }
        }
        if(miss) {
            for(int j = 1; j < table.size(); j++) {
                if(table[j].age > table[LRU].age) {       //find oldest entry
                    LRU = j;
                }
            }
            table[LRU].tag = tag;
            table[LRU].age = 0;
        }
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Fully Associative (LRU) and (Hot-Cold LRU)
};

string Cache::FAHC() {
    int byteOffset = 5;
    int numBlock = 512;
    int hits = 0;
    int access = trace.size();
    vector<Package> table(numBlock+1);
    for(int i = 0; i < trace.size(); i++) {
        //cout << i+1 << " / " << trace.size() << endl;
        long tag = trace[i].address >> byteOffset;
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
                if(table[right].hot) {
                    j = left;
                    continue;
                }
                else if(table[left].hot){
                    j = right;
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
    int byteOffset = log(16384/assoc);
    int waySize = (LINESIZE * assoc);
    int sets = 16384 / waySize;
    int hits = 0;
    int access = trace.size();

    vector<vector<Package>> table(sets, vector<Package> (assoc));
    for(int i = 0; i < trace.size(); i++) {
        int index = (trace[i].address / LINESIZE) % sets;
        long tag = trace[i].address >> byteOffset;
        int LRU = 0;
        int miss = 1;
        for(int j = 0; j < assoc; j++) {
            table[index][j].age++;
            if(table[index][j].tag == tag) {
                table[index][j].age = 0;
                hits++;
                miss = 0;
                break;
            }
            if(table[index][j].tag == -1) {
                table[index][j].tag = tag;
                table[index][j].age = 0;
                miss = 0;
                break;
            }
        }
        if(miss) {
            if(trace[i].instruction != 'S') {
                for(int j = 1; j < assoc; j++) {
                    if(table[index][j].age > table[index][LRU].age) {       //find oldest entry
                        LRU = j;
                    }
                }
                table[index][LRU].tag = tag;
                table[index][LRU].age = 0;
            }
        }
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with No allocation on a write miss
};

string Cache::SANP(int assoc) {
    int byteOffset = log(16384/assoc);
    int waySize = (LINESIZE * assoc);
    int sets = 16384 / waySize;
    int hits = 0;
    int access = trace.size();

    vector<vector<Package>> table(sets, vector<Package> (assoc));
    for(int i = 0; i < trace.size(); i++) {
        int index = (trace[i].address / LINESIZE) % sets;
        long tag = trace[i].address >> byteOffset;
        int miss = 1;
        for(int j = 0; j < assoc; j++) {
            table[index][j].age++;
            if(table[index][j].tag == tag) {
                table[index][j].age = 0;
                hits++;
                miss = 0;
                break;
            }
            if(table[index][j].tag == -1) {
                table[index][j].tag = tag;
                table[index][j].age = 0;
                miss = 0;
                break;
            }
        }
        if(miss) {
            int LRU = 0;
            for(int j = 1; j < assoc; j++) {
                if(table[index][j].age > table[index][LRU].age) {       //find oldest entry
                    LRU = j;
                }
            }
            table[index][LRU].tag = tag;
            table[index][LRU].age = 0;
        }
        long preTag = trace[i].address + 32;
        preTag = preTag >> byteOffset;
        miss = 1;
        for(int j = 0; j < assoc; j++) {
            if(table[index][j].tag == preTag) {
                table[index][j].age = 0;
                miss = 0;
                break;
            }
        }
        if(miss) {
            int LRU = 0;
            for(int j = 1; j < assoc; j++) {
                if(table[index][j].age > table[index][LRU].age) {       //find oldest entry
                    LRU = j;
                }
            }
            table[index][LRU].tag = preTag;
            table[index][LRU].age = 0;
        }
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with next-line prefetching
};

string Cache::PF(int assoc) {
    int byteOffset = log(16384/assoc);
    int waySize = (LINESIZE * assoc);
    int sets = 16384 / waySize;
    int hits = 0;
    int access = trace.size();

    vector<vector<Package>> table(sets, vector<Package> (assoc));
    for(int i = 0; i < trace.size(); i++) {
        int index = (trace[i].address / LINESIZE) % sets;
        long tag = trace[i].address >> byteOffset;
        int miss = 1;
        for(int j = 0; j < assoc; j++) {
            table[index][j].age++;
            if(table[index][j].tag == tag) {
                table[index][j].age = 0;
                hits++;
                miss = 0;
                break;
            }
            if(table[index][j].tag == -1) {
                table[index][j].tag = tag;
                table[index][j].age = 0;
                miss = 0;
                break;
            }
        }
        if(miss) {
            int LRU = 0;
            for(int j = 1; j < assoc; j++) {
                if(table[index][j].age > table[index][LRU].age) {       //find oldest entry
                    LRU = j;
                }
            }
            table[index][LRU].tag = tag;
            table[index][LRU].age = 0;
            //prefetch
            long preTag = trace[i].address + 32;
        preTag = preTag >> byteOffset;
        miss = 1;
        for(int j = 0; j < assoc; j++) {
            if(table[index][j].tag == preTag) {
                table[index][j].age = 0;
                miss = 0;
                break;
            }
        }
        if(miss) {
            int LRU = 0;
            for(int j = 1; j < assoc; j++) {
                if(table[index][j].age > table[index][LRU].age) {       //find oldest entry
                    LRU = j;
                }
            }
            table[index][LRU].tag = preTag;
            table[index][LRU].age = 0;
        }
        }
        
    }

    string result = to_string(hits) + "," + to_string(access) + ";";
    return result;  //<- Set Associative with next-line prefetching on every miss
};

void Cache::pushBack(char ins, long addr) {
    trace.push_back(Address(ins, addr));
};
