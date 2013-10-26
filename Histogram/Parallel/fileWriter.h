#define SUM_FILE "result.out"
#define HIST_A_FILE "hist.a"
#define HIST_B_FILE "hist.b"
#define HIST_C_FILE "hist.c"

int writeSumsToFile(string fileName, float sums[]);
int writeHistToFile(string fileName, int hist[]);
int writeAllOutputFiles(float sums[],
                        int histA[],
                        int histB[],
                        int histC[]);