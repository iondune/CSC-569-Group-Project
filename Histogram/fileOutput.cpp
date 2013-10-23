#include <iostream>
#include <fstream>
#include <iomanip>

#define SUM_FILE "result.out"
#define HIST_A_FILE "hist.a"
#define HIST_B_FILE "hist.b"
#define HIST_C_FILE "hist.c"

using namespace std;

int writeSumsToFile(string fileName, vector<float> sums);
int writeHistToFile(string fileName, vector<int> hist);
int writeAllOutputFiles(vector<float> sums,
                        vector<int> histA,
                        vector<int> histB,
                        vector<int> histC);

int writeSumsToFile(string fileName, vector<float> sums) {
    ofstream outFile;

    outFile.open(fileName, ios::trunc);

    for (int i = 0; i < sums.size(); i++) {
        if (i > 0) {
            // elements separated by single space
            outFile << " ";
        }

        // write sum float to two decimal places
        outFile << std::fixed << std::setprecision(2) << sums[i];
    }

    outFile.close();
    return 0;
}

int writeHistToFile(string fileName, vector<int> hist) {
    ofstream outFile;

    outFile.open(fileName, ios::trunc);

    for (int i = 0; i < sums.size(); i++) {
        // write bucket num and value
        outFile << i << ", ";
        outFile << hist[i] << endl;
    }

    outFile.close();
    return 0;
}

int writeAllOutputFiles(vector<float> sums,
                        vector<int> histA,
                        vector<int> histB,
                        vector<int> histC) {
    int errorCode = 0;

    if (errorCode = writeSumsToFile(SUM_FILE, sums) < 0) {
        return errorCode;
    }

    if (errorCode = writeHistToFile(HIST_A_FILE, histA) < 0) {
        return errorCode;
    }

    if (errorCode = writeHistToFile(HIST_B_FILE, histB) < 0) {
        return errorCode;
    }

    if (errorCode = writeHistToFile(HIST_C_FILE, histC) < 0) {
        return errorCode;
    }

    return errorCode;
}









