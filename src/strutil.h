#include <stdio.h>
#include <string.h>

#define MAX_INT_STRING_LEN   11 // "-4000000000"
#define MAX_FLOAT_STRING_LEN 20 // Guesstimated

/**
 * Splits a string of elements into chunks of elements, and performs an action
 * on each chunk.
 *
 * @param data The string of elements.
 * @param delim The delimeter.
 * @param chunkSize The maximum number of elements to break off into a chunk.
 * @param extra Extra data to pass to the callback.
 * @param f The callback method to process each chunk. Its parameters are:
 *            char* The chunk of data.
 *            int The number of elements in the chunk (delimited by |delim|).
 *            const char The delimiter passed into chunks.
 *            int The chunk size passed into chunks.
 *            void* The extra data passed into chunks.
 */
void withChunks(char* data, 
                const char delim, 
                int chunkSize, 
                void* extra, 
                void (*f)(char*, int, const char, int, void*));

/**
 * Like withChunks, but uses ' ' as a delimiter.
 */
void withChunksSpace(char* data, 
                     int chunkSize, 
                     void* extra, 
                     void (*f)(char*, int, const char, int, void*));

char* itoa(int n, int* len);

/**
 * Convert a packed string of floats into a float[] array (out parameter).
 *
 * @param floatsStr The floats string, e.g. "1.02.03.0"
 * @param numFloats The number of floats in floatsStr, e.g. 3
 * @param lengths The lengths of each float in bytes, e.g. [3,3,3]
 * @param floats An out-parameter to a float array of size |numFloats|.
 */
void strToFloats(char* floatsStr, int numFloats, int lengths[], float* floats);

inline char* firstToken(char* data, const char* delim) {
  return strtok(data, delim);
}

inline char* nextToken(const char* delim) {
  return strtok(NULL, delim);
}
