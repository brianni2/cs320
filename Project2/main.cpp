#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include "Cache.h"

using namespace std;

int main(int argc, char * argv[]) {
    ifstream inFile;
    ofstream outFile;
    inFile.open(string(argv[1]));
    outFile.open(string(argv[2]));

    string line;
    Cache cache = Cache();
    while(getline(inFile, line)) {
        stringstream ss(line);
        char ins;
        long addr;
        ss >> ins >> std::hex >> addr;
        cache.pushBack(ins, addr);
    }

    string DMRes = "";
    string SARes = "";
    string FANRes = "";
    string FAHCRes = "";
    string SANARes = "";
    string SANPRes = "";
    string PFRes = "";

    for(int i = 1; i < 5; i++) {
        DMRes += cache.DM(i);
        SARes += cache.SA(pow(2, i));
        SANARes += cache.SANA(pow(2, i));
        SANPRes += cache.SANP(pow(2, i));
        PFRes += cache.PF(pow(2, i));
        switch(i) {
            case 4:
                DMRes += "\n";
                SARes += "\n";
                SANARes += "\n";
                SANPRes += "\n";
                PFRes += "\n";
                break;
            default:
                DMRes += " ";
                SARes += " ";
                SANARes += " ";
                SANPRes += " ";
                PFRes += " ";
                break;
        }
    }
    FANRes = cache.FAN() + "\n";
    FAHCRes = cache.FAHC() + "\n";

    outFile << DMRes << SARes << FANRes << FAHCRes << SANARes << SANPRes << PFRes;

    outFile.close();
    return 0;
}