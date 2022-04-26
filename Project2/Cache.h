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
    int age;
    long tag;
    Package() {
        hit = 0;
        age = 0;
        tag = -1;
    }
};

class Cache {
    public:
        vector<Address> trace;
        void pushBack(char, long);

        string DM(int);         //direct map
        string SA(int);         //set associative
        string FA(int);         //fully associative
        string SANA(int);       //set associative no allocation
        string SANP(int);       //set associative prefetch
        string PF(int);         //prefetch
};
