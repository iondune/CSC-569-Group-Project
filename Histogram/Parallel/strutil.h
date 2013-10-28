#include <stdio.h>
#include <string.h>

#define MAX_INT_STRING_LEN   11 // "-4000000000"
#define MAX_FLOAT_STRING_LEN 20 // Guesstimated

/**
 * Splits a string of elements into chunks of elements, and performs an action
 * on each chunk. Delimited by ' '.
 *
 * @param data      The string of elements.
 * @param chunkSize The size of the chunks to create.
 * @param extra     Extra data to pass to the callback.
 * @param f         The callback method to process each chunk. Its parameters are:
 *                     char*      The chunk of data.
 *                     int        The ordinal value of this chunk (0, 1, ...)
 *                     int        The requested chunk size.
 *                     int        The actual chunk size.
 *                     void*      The extra data passed into chunks.
 */
void withChunks(char* data,
                int chunkSize,
                void* extra,
                void (*f)(char*, int, int, int, void*));

inline char* firstToken(char* data) {
  return strtok(data, " ");
}

inline char* nextToken() {
  return strtok(NULL, " ");
}
