#include <stdio.h>
#include <string.h>

#define MAX_INT_STRING_LEN   11 // "-4000000000"
#define MAX_FLOAT_STRING_LEN 20 // Guesstimated

/**
 * Splits a string of elements into chunks of elements, and performs an action
 * on each chunk.
 *
 * @param data      The string of elements.
 * @param delim     The delimeter.
 * @param chunkSize The size of the chunks to create.
 * @param extra     Extra data to pass to the callback.
 * @param f         The callback method to process each chunk. Its parameters are:
 *                     char*      The chunk of data.
 *                     int        The ordinal value of this chunk (0, 1, ...)
 *                     const char The delimiter passed into chunks.
 *                     int        The requested chunk size.
 *                     int        The actual chunk size.
 *
 *                     void*      The extra data passed into chunks.
 */
void withChunks(char* data, 
                const char delim, 
                int chunkSize,
                void* extra, 
                void (*f)(char*, int, const char, int, int, void*)); 
/**
 * Like withChunks, but uses ' ' as a delimiter.
 */
void withChunksSpace(char* data, 
                     int chunkSize,
                     void* extra, 
                     void (*f)(char*, int, const char, int, int, void*));

inline char* firstToken(char* data) {
  return strtok(data, " ");
}

inline char* nextToken() {
  return strtok(NULL, " ");
}
