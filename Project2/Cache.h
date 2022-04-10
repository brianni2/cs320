#include <string>
#include <vector>

using namespace std;

struct Address {
    char instruction;
    long address;
    Address(char ins, long addr) {
        instruction = ins;
        address = addr;
    };
};

struct Package {
    int hit;
    long content;
    Package() {
        hit = 0;
        content = -1;
    }
    int inRange(long addr) {
        if(addr < content && addr > content-8) {
            return 1;
        }
        return 0;
    };
};

class Cache {
    public:
        vector<Address> addresses;
        void pushBack(char, long);

        string DM(int);         //direct map
        string SA(int);         //set associative
        string FA(int);         //fully associative
        string SANA(int);       //set associative no allocation
        string SANP(int);       //set associative prefetch
        string PF(int);         //prefetch
};
