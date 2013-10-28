#include <iostream>
#include <fstream>
#include <iomanip>
#include "fileWriter.h"

using namespace std;

int writeSumsToFile(string fileName, float sums[]) {
    ofstream outFile;

    outFile.open(fileName.c_str(), ios::trunc);

    int listSize = sizeof(sums) / sizeof(float);

    for (int i = 0; i < listSize; i++) {
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

int writeHistToFile(string fileName, int hist[]) {
    ofstream outFile;

    outFile.open(fileName.c_str(), ios::trunc);

    int listSize = sizeof(hist) / sizeof(int);

    for (int i = 0; i < listSize; i++) {
        // write bucket num and value
        outFile << i << ", ";
        outFile << hist[i] << endl;
    }

    outFile.close();
    return 0;
}

int writeAllOutputFiles(float sums[],
                        int histA[],
                        int histB[],
                        int histC[]) {
    int errorCode = 0;

    if ((errorCode = writeSumsToFile(SUM_FILE, sums)) < 0) {
        return errorCode;
    }

    if ((errorCode = writeHistToFile(HIST_A_FILE, histA)) < 0) {
        return errorCode;
    }

    if ((errorCode = writeHistToFile(HIST_B_FILE, histB)) < 0) {
        return errorCode;
    }

    if ((errorCode = writeHistToFile(HIST_C_FILE, histC)) < 0) {
        return errorCode;
    }

    return errorCode;
}









