#include "strutil.h"

#include <stdlib.h>

void withChunks(char* data,
                const char delim,
                int chunkSize,
                int numProcs,
                void* extra,
                void (*f)(char*, int, const char, int, int, int, void*)) {
  char* begin = data;
  char* cur = data;
  int ordinal = 0;

  while (*begin != '\0') {
    int count = 0;
    for (cur = begin; *cur != '\0' && count < chunkSize; ++cur) {
      if (*cur == delim)
        count++;
    }
    if (*cur == '\0')
      count++; // Last item.

    f(begin, ordinal, delim, chunkSize, count, numProcs, extra);
    begin = cur;
    ordinal++;
  }
}

void withChunksSpace(char* data,
                     int chunkSize,
                     int numProcs,
                     void* extra,
                     void (*f)(char*, int, const char, int, int, int, void*)) {
  withChunks(data, ' ', chunkSize, numProcs, extra, f);
}

char* itoa(int n, int* len) {
  static char arr[MAX_INT_STRING_LEN]; // Big enough to hold "-4000000000"
  *len = snprintf(arr, MAX_INT_STRING_LEN, "%d", n);
  return arr;
}

void strToFloats(char* floatsStr, int numFloats, int lengths[], float* floats) {
  static char arr[MAX_FLOAT_STRING_LEN];

  char* ptr = arr;
  for (int i = 0; i < numFloats; ++i) {
    int len = lengths[i];
    memcpy(arr, ptr, len);
    arr[len] = '\0';

    floats[i] = atof(arr);
    ptr += len;
  }
}