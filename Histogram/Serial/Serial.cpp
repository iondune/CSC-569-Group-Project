
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cmath>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>


unsigned int GetFileSize(char const * const fileName)
{
    struct stat sb;
    if (stat(fileName, & sb) != 0)
    {
        fprintf(stderr, "'stat' failed for '%s': %s.\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sb.st_size;
}


static char * ReadWholeFile(const char * fileName)
{
    unsigned int s;
    char * contents;
    FILE * f;
    size_t bytesRead;
    int status;

    s = GetFileSize(fileName);
    if (! (contents = (char *) malloc(s + 1)))
    {
        fprintf(stderr, "Not enough memory.\n");
        exit(EXIT_FAILURE);
    }

    if (! (f = fopen(fileName, "r")))
    {
        fprintf(stderr, "Could not open '%s': %s.\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((bytesRead = fread(contents, sizeof(char), s, f)) != s)
    {
        fprintf (stderr, "Short read of '%s': expected %d bytes "
                 "but got %d: %s.\n", fileName, s, bytesRead, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if ((status = fclose(f)) != 0)
    {
        fprintf (stderr, "Error closing '%s': %s.\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return contents;
}

template <typename T>
 static T const Clamp(T const & v, T const & min, T const & max)
{
        return ((v > max) ? max : ((v < min) ? min : v));
}

int main (int argc, char *argv[])
{
        char *src1, *src2;

        if (argc != 3)
        {
            fprintf(stderr, "usage: serial <file1> <file2>");
            exit(EXIT_FAILURE);
        }
        
        src1 = ReadWholeFile(argv[1]);
        src2 = ReadWholeFile(argv[2]);
        
        std::vector<float> File1, File2;
        
        char const * const Tokenizer = " ";
        char * pch = strtok(src1, Tokenizer);
        while (pch != NULL)
        {
                File1.push_back(atof(pch));
                pch = strtok(NULL, Tokenizer);
        }
        free(src1);
        pch = strtok(src2, Tokenizer);
        while (pch != NULL)
        {
                File2.push_back(atof(pch));
                pch = strtok(NULL, Tokenizer);
        }
        free(src2);
        File1.erase(--File1.end());
        File2.erase(--File2.end());
        
        if (File1.size() != File2.size())
        {
                std::cerr << "Vector sizes differ! " << File1.size() << " " << File2.size() << std::endl;
                return 1;
        }
        
        std::cout << "Vector size is " << File1.size() << std::endl;
        
        std::vector<float> Output;Output.resize(File1.size());
        for (int i = 0; i < File1.size(); ++ i){
                Output[i] = File1[i] + File2[i];}
        
        float Max1 = *std::max_element(File1.begin(), File1.end());
        float Max2 = *std::max_element(File2.begin(), File2.end());
        float Max3 = *std::max_element(Output.begin(), Output.end());
        float Min1 = -10.f, Min2 = -10.f, Min3 = -20.f;
        float H = 0.5f;
        float BinSize = 0.5f;
        
        int K1 = ceil((Max1 - Min1) / H);
        int K2 = ceil((Max2 - Min2) / H);
        int K3 = ceil((Max3 - Min3) / H);
        
        
        FILE * output = fopen("result.out", "w");
        for (int i = 0; i < File1.size(); ++ i)
                fprintf(output, "%.2f ", Output[i]);
        fclose(output);
        
        
        int * Bins1 = new int[K1];
        memset(Bins1, 0, K1*4);
        for (int i = 0; i < File1.size(); ++ i)
        {
                int index = (int) floor((File1[i] - Min1) / (Max1 - Min1) * K1);
                if (index >= 0 && index < K1)
                        Bins1[index]++;
                else{
                        Bins1[0]++; // don't ask;
                        printf("Agh a! %d\n", index);}
        }
        int * Bins2 = new int[K2];
        memset(Bins2, 0, K2*4);
        for (int i = 0; i < File1.size(); ++ i)
        {
                int index = (int) floor((File2[i] - Min2) / (Max2 - Min2) * K2);
                if (index >= 0 && index < K2)
                        Bins2[index]++;
                else{
                        Bins2[0]++; // don't ask;
                        printf("Agh b! %d\n", index);}
        }
        int * Bins3 = new int[K3];
        memset(Bins3, 0, K3*4);
        for (int i = 0; i < File1.size(); ++ i)
        {
            float Val = Output[i];
            Val -= Min3;
            Val /= BinSize;
            int index = (int) Val;
                //int index = (int) floor((Output[i] - Min3) / (Max3 - Min3) * K3);
                if (index >= 0 && index < K3)
                        Bins3[index]++;
                else{
                        Bins3[0]++; // don't ask;
                        printf("Agh c! %d\n", index);}
        }
        
        FILE * hista = fopen("hist.a", "w");
        for (int i = 0; i < K1; ++ i)
                fprintf(hista, "%d, %d\n", i, Bins1[i]);
        fclose(hista);
        FILE * histb = fopen("hist.b", "w");
        for (int i = 0; i < K2; ++ i)
                fprintf(histb, "%d, %d\n", i, Bins2[i]);
        fclose(histb);
        FILE * histc = fopen("hist.c", "w");
        for (int i = 0; i < K3; ++ i)
                fprintf(histc, "%d, %d\n", i, Bins3[i]);
        fclose(histc);
        
        return 0;
}
