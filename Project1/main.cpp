#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

int taken(string behavior, string prediction) {
    if(behavior == prediction) {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    //Initialize and open files//
    ifstream inFile;
    ofstream outFile;
    inFile.open(string(argv[1]));
    outFile.open(string(argv[2]));

    unsigned long long addr, target;
    string behavior, line;

    //predictor results//
    int alwaysT = 0;
    int alwaysNT = 0;


    //main loop//
    int count = 0;
    while(getline(inFile, line)) {
        //process input//
        stringstream ss(line);
        ss >> std::hex >> addr >> behavior >> std::hex >> target;

        //predictors//
        alwaysT += taken(behavior, "T");        //always taken
        alwaysNT += taken(behavior, "NT");      //always not taken

        count++;
    }

    //output//
    outFile << alwaysT << "," << count << "; \n";
    outFile << alwaysNT << "," << count << "; \n";
    outFile << "x,y; x,y; x,y; x,y; x,y; x,y; x,y; " << "<- Single bit Bimodal predictors" << endl;
    outFile << "x,y; x,y; x,y; x,y; x,y; x,y; x,y; " << "<- Two bit saturating Bimodal Predictors" << endl;
    outFile << "x,y; x,y; x,y; x,y; x,y; x,y; x,y; x,y; x,y; " << "<- Gshare" << endl;
    outFile << "x,y; " << "<- Tournament" << endl;
    outFile << "x,y; " << "<- BTB correct predictions, BTB attempted predictions" << endl;
    
    return 0;
}