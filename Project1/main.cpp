#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

string toBinary(unsigned long long decimal) {       //returns binary of decimal in reverse order
    string binaryResult = "";
    while(decimal != 0) {
        //cout << decimal%2;
        binaryResult += to_string(decimal%2);
        decimal = decimal/2;
    }
    //cout << binaryResult << endl;
    return binaryResult;
}

int toInt(string bin) {
    int dec = 0;
    for(int i = 0; i < bin.length(); i++) {
        //cout << bin[i];
        if(bin[i] == '1') {
            dec += pow(2, i);
        }
    }
    //cout << "; " << dec << "; ";
    return dec;
}

struct Address {
    unsigned long long addr;
    unsigned long long target;
    int behavior = 1;
    string binAddr;
    Address(unsigned long long a, unsigned long long t, string b) {
        addr = a;
        target = t;
        if(b == "NT") {
            behavior = 0;
        }
        binAddr = toBinary(addr);
    };
};

int always(string behavior, string prediction) {
    if(behavior == prediction) {
        return 1;
    }
    return 0;
}

void bimodal1(int* result, vector<Address>* input) {
    int count = 0;
    for(int i = 4; i < 12; i++) {
        if(i == 6) {        //skip 64 (2^6)
            //cout << "\nTable 64 - Skipped\n";
            continue;
        }
        else {
            //create history table//
            int tableSize = pow(2, i);
            vector<int> table;
            table.resize(tableSize, 1);
            //cout << "Table " << tableSize << "\tResult[" << count << "]\n";

            //Loop through inputs//
            for(int j = 0; j < input->size(); j++) {
                string substr(input->at(j).binAddr, 0, i);
                int dec = toInt(substr);
                //cout << input->at(j).addr << ": " << substr << "\t|" << dec << "\t|TV:" << table[dec] << "\t|B:" << input->at(j).behavior;
                if(input->at(j).behavior == table[dec]) {
                    //cout << "\tPrediction correct\n";
                    result[count]++;
                    continue;
                }
                else {
                    //cout << "\tUpdating table[" << dec << "] to " << input->at(j).behavior << endl;
                    table[dec] = input->at(j).behavior;
                }
            }
            //cout << "result[" << count << "] = " << result[count] << endl << endl;
            count++;
        }
    }
}

void bimodal2(int* result, vector<Address> *input) {
    int count = 0;
    for(int i = 4; i < 12; i++) {
        if(i == 6) {        //skip 64 (2^6)
            //cout << "\nTable 64 - Skipped\n";
            continue;
        }
        else {
            //create history table//
            int tableSize = pow(2, i);
            vector<int> table;
            table.resize(tableSize, 3);
            //cout << "Table " << tableSize << "\tResult[" << count << "]\n";

            //Loop through inputs//
            for(int j = 0; j < input->size(); j++) {
                string substr(input->at(j).binAddr, 0, i);
                int dec = toInt(substr);
                //cout << input->at(j).addr << ": " << substr << "\t|" << dec << "\t|TV:" << table[dec] << "\t|B:" << input->at(j).behavior << endl;
                if(input->at(j).behavior == 0) {        //Not Taken
                    switch(table[dec]) {
                        case 0:
                            result[count]++;
                            break;
                        case 1:
                            result[count]++;
                            table[dec]--;
                            break;
                        case 2:
                            table[dec]--;
                            break;
                        case 3:
                            table[dec]--;
                            break;
                    }
                    continue;
                }
                else {      //Taken
                    switch(table[dec]) {
                        case 3:
                            result[count]++;
                            break;
                        case 2:
                            result[count]++;
                            table[dec]++;
                            break;
                        case 1:
                            table[dec]++;
                            break;
                        case 0:
                            table[dec]++;
                            break;
                    }
                }
            }
            //cout << "result[" << count << "] = " << result[count] << endl << endl;
            count++;
        }
    }
}

void Gshare(int* result, vector<Address> *input) {
}


int main(int argc, char *argv[]) {
    //Initialize variables and open files//
    ifstream inFile;
    ofstream outFile;
    inFile.open(string(argv[1]));
    outFile.open(string(argv[2]));

    unsigned long long addr, target;
    string behavior, line;
    vector<Address> *input = new vector<Address>;

    //predictor results//
    int alwaysT = 0;
    int alwaysNT = 0;
    int singleBit[7] = {0, 0, 0, 0, 0, 0, 0};
    int* singlePtr = singleBit;
    int doubleBit[7] = {0, 0, 0, 0, 0, 0, 0};
    int* doublePtr = doubleBit;
    int GshareRes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int* GsharePtr = GshareRes;

    //main loop//
    while(getline(inFile, line)) {
        //process input//
        stringstream ss(line);
        ss >> std::hex >> addr >> behavior >> std::hex >> target;
        //cout << addr << " " << behavior << " " << target << endl;
        input->push_back(Address(addr, target, behavior));

        alwaysT += always(behavior, "T");        //always taken
        alwaysNT += always(behavior, "NT");      //always not taken
    }

    //predictors//
    bimodal1(singlePtr, input);
    bimodal2(doublePtr, input);
    Gshare(GsharePtr, input);

    //output//
    outFile << alwaysT << "," << input->size() << "; \n";
    outFile << alwaysNT << "," << input->size() << "; \n";
    for(int i = 0; i < 7; i++) {    //Single bit Bimodal predictors
        outFile << singleBit[i] << "," << input->size() << "; ";
    }
    outFile << endl;

    for(int i = 0; i < 7; i++) {    //Two bit saturating Bimodal Predictors
        outFile << doubleBit[i] << "," << input->size() << "; ";
    }
    outFile << endl;

    for(int i = 0; i < 9; i++) {    //Gshare
        outFile << GshareRes[i] << "," << input->size() << "; ";
    }
    outFile << endl;

    outFile << "x,y; " << "<- Tournament" << endl;
    outFile << "x,y; " << "<- BTB correct predictions, BTB attempted predictions" << endl;
    
    return 0;
}