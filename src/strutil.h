/**
 * Splits a string of elements into chunks of elements, and performs an action
 * on each chunk.
 *
 * @param data The string of elements.
 * @param delim The delimeter.
 * @param chunkSize The maximum number of elements to break off into a chunk.
 * @param f The callback method to process each chunk.
 */
int chunks(char* data, const char delim, int chunkSize, void (*f)(char*, int));
