#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

struct BTB_Result {
    int correct;
    int attempt;
    BTB_Result(int x, int y) {
        correct = x;
        attempt = y;
    };
};

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

int Xor(string addr, vector<int> history) {
    string result = "";
    for(int i = 0; i < history.size(); i++) {
        if(addr[i] == '0') {
            switch(history[i]) {
                case 0 :
                    result += '0';
                    break;
                case 1 :
                    result += '1';
                    break;
            }
        }
        if(addr[i] == '1') {
            switch(history[i]) {
                case 0 :
                    result += '1';
                    break;
                case 1 :
                    result += '0';
                    break;
            }
        }
    }
    for(int i = history.size(); i < addr.length(); i++) {
        switch(addr[i]) {
            case '0':
                result += '0';
                break;
            case '1':
                result += '1';
                break;
        }
    }
    return toInt(result);
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
    int count = 0;
    for(int i = 3; i < 12; i++) {
        vector<int> table;
        table.resize(2048, 3);
        vector<int> history;
        history.resize(i, 0);
        for(int j = 0; j < input->size(); j++) {
            string substr(input->at(j).binAddr, 0, 11);
            int index = Xor(substr, history);
            //cout << index << "| \t" << table[index] << "|\t" << input->at(j).behavior << endl;
            if(input->at(j).behavior == 0) {        //Not Taken
                switch(table[index]) {
                    case 0:
                        result[count]++;
                        break;
                    case 1:
                        result[count]++;
                        table[index]--;
                        break;
                    case 2:
                        table[index]--;
                        break;
                    case 3:
                        table[index]--;
                        break;
                }
                for(int k = history.size()-1; k < 0; k++) {
                    history[k] = history[k-1];
                }
                history[0] = 0;
                continue;
            }
            else {      //Taken
                switch(table[index]) {
                    case 3:
                        result[count]++;
                        break;
                    case 2:
                        result[count]++;
                        table[index]++;
                        break;
                    case 1:
                        table[index]++;
                        break;
                    case 0:
                        table[index]++;
                        break;
                }
                for(int k = history.size()-1; k < 0; k++) {
                    history[k] = history[k-1];
                }
                history[0] = 1;
            }
        }
        count++;
    }
}

int Tournament(vector<Address> *input) {
    vector<int> Gtable;
    vector<int> Btable;
    vector<int> history;
    vector<int> pref;
    Gtable.resize(2048, 3);
    Btable.resize(2048, 3);
    history.resize(11, 0);
    pref.resize(2048, 0);

    int correct = 0;

    for(int i = 0; i < input->size(); i++) {
        string substr(input->at(i).binAddr, 0, 11);
        int dec = toInt(substr);
        int index = Xor(substr, history);
        int Bcorrect = 0;
        int Gcorrect = 0;

        if(input->at(i).behavior == 0) {        //Not Taken
            switch(Btable[dec]) {               //Bimodal
                case 0:
                    Bcorrect = 1;
                    break;
                case 1:
                    Bcorrect = 1;
                    Btable[dec]--;
                    break;
                case 2:
                    Bcorrect = 0;
                    Btable[dec]--;
                    break;
                case 3:
                    Bcorrect = 0;
                    Btable[dec]--;
                    break;
            }
            switch(Gtable[index]) {             //Gshare
                case 0:
                    Gcorrect = 1;
                    break;
                case 1:
                    Gcorrect = 1;
                    Gtable[index]--;
                    break;
                case 2:
                    Gcorrect = 0;
                    Gtable[index]--;
                    break;
                case 3:
                    Gcorrect = 0;
                    Gtable[index]--;
                    break;
            }
            for(int k = history.size()-1; k < 0; k++) {
                history[k] = history[k-1];
            }
            history[0] = 0;
            continue;
        }
        else {                          //Taken
            switch(Btable[dec]) {       //Bimodal
                case 3:
                    Bcorrect = 1;
                    break;
                case 2:
                    Bcorrect = 1;
                    Btable[dec]++;
                    break;
                case 1:
                    Bcorrect = 0;
                    Btable[dec]++;
                    break;
                case 0:
                    Bcorrect = 0;
                    Btable[dec]++;
                    break;
            }
            switch(Gtable[index]) {     //Gshare
                case 3:
                    Gcorrect = 1;
                    break;
                case 2:
                    Gcorrect = 1;
                    Gtable[index]++;
                    break;
                case 1:
                    Gcorrect = 0;
                    Gtable[index]++;
                    break;
                case 0:
                    Gcorrect = 0;
                    Gtable[index]++;
                    break;
            }
            for(int k = history.size()-1; k < 0; k++) {
                history[k] = history[k-1];
            }
            history[0] = 1;
        }
        //check preference
        switch(pref.at(dec)) {
            case 3:     //strongly prefer Bimodal
                if(Bcorrect && Gcorrect) {
                    correct++;
                    break;
                }
                if(Bcorrect) {
                    correct++;
                    break;
                }
                if(Gcorrect) {
                    pref.at(dec)--;
                    break;
                }
            case 2:     //prefer Bimodal
                if(Bcorrect && Gcorrect) {
                    correct++;
                    break;
                }
                if(Bcorrect) {
                    correct++;
                    pref.at(dec)++;
                    break;
                }
                if(Gcorrect) {
                    pref.at(dec)--;
                    break;
                }
            case 1:     //prefer Gshare
                if(Bcorrect && Gcorrect) {
                    correct++;
                    break;
                }
                if(Bcorrect) {
                    pref.at(dec)++;
                    break;
                }
                if(Gcorrect) {
                    correct++;
                    pref.at(dec)--;
                    break;
                }
            case 0:     //strongly prefer Gshare
                if(Bcorrect && Gcorrect) {
                    correct++;
                    break;
                }
                if(Bcorrect) {
                    pref.at(dec)++;
                    break;
                }
                if(Gcorrect) {
                    correct++;
                    pref.at(dec);
                    break;
                }
        }
    }
    return correct;
}

BTB_Result BTB(vector<Address> *input) {
    //cout << "BTB\n";
    BTB_Result result = BTB_Result(0, 0);

    vector<int> table;
    vector<string> buffer;
    table.resize(512, 1);
    buffer.resize(512);

    for(int i = 0; i < input->size(); i++) {
        //cout << i << endl;
        string substr(input->at(i).binAddr, 0, 9);
        string subtar(toBinary(input->at(i).target), 0, 9);
        int dec = toInt(substr);
        if(subtar == buffer[dec]) {
            result.attempt++;
            result.correct++;
            continue;
        }
        if(input->at(i).behavior == 0) {        //Not Taken
            switch(table[dec]) {
                case 0:
                    break;
                case 1:
                    table[dec]--;
                    break;
                case 2:     //predict taken
                    result.attempt++;
                    if(buffer[dec] == subtar) {
                        result.correct++;
                    }
                    table[dec]--;
                    break;
                case 3:     //predict taken
                    result.attempt++;
                    if(buffer[dec] == subtar) {
                        result.correct++;
                    }
                    table[dec]--;
                    break;
            }
            continue;
        }
        else {      //Taken
            //cout << "Setting buffer\n";
            buffer[dec] = subtar;
            switch(table[dec]) {
                case 3:
                    break;
                case 2:
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

    return result;
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
    int tourPred;

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
    tourPred = Tournament(input);
    BTB_Result branchTarg = BTB(input);

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

    outFile << tourPred << "," << input->size() << "; \n"; //Tournament
    outFile << branchTarg.correct << "," << branchTarg.attempt << "; \n";   //BTB
    
    return 0;
}