#include "lib/vector.h"

#include <cstdlib> // atof
#include <cstring>

#include "lib/strutil.h"
#include "deps/src/inc/mapreduce.h"
#include "deps/src/inc/keyvalue.h"

using MAPREDUCE_NS::KeyValue;

static void mapChunk(int itask, 
                           KeyValue* keyValue, 
                           void* extra);

static void addReduce(char* key,
                      int keybytes,
                      char* values,
                      int numValues,
                      int lengths[],
                      KeyValue* kv,
                      void* ptr);

static void printScan(char *key, 
                      int keybytes, 
                      char *value, 
                      int valuebytes, 
                      void *ptr);

typedef struct {
  char* data;
  int ordinal;
  int chunkSize;
  int count;
} FromExtra;

Vector* Vector::from(char* data, int chunkSize) {
  Vector* vec = new Vector(MPI_COMM_WORLD);
  withChunksSpace(data, chunkSize, vec, &handleVectorChunk);
  return vec;
}

void Vector::handleVectorChunk(char* data, 
                               int ordinal, 
                               const char delim, 
                               int chunkSize,
                               int count, 
                               void* extra) {
  Vector* vec = (Vector*) extra;
  FromExtra extraData = { data, ordinal, chunkSize, count };
  vec->map(1, &mapChunk, &extraData, 1);
}

void mapChunk(int itask, KeyValue* keyValue, void* extra) {
  FromExtra* extraData = (FromExtra*) extra;
  if (extraData->count == 0)
    return;

  // First iteration is a special case, because that's how strtok rolls.
  int key = extraData->ordinal * extraData->chunkSize;
  float val = atof(firstToken(extraData->data));
  keyValue->add((char*) &key, sizeof(key), (char*) &val, sizeof(val));

  for (int i = 1; i < extraData->count; ++i) {
    key = extraData->ordinal * extraData->chunkSize + i;
    val = atof(nextToken());
    keyValue->add((char*) &key, sizeof(key), (char*) &val, sizeof(val));
  }
}

Vector* Vector::add(Vector* vecA, Vector* vecB) {
  Vector* vecC = new Vector(MPI_COMM_WORLD);
  vecC->add(vecA);
  vecC->add(vecB);
  vecC->collate(NULL);
  vecC->reduce(&addReduce, NULL);
  return vecC;
}

void addReduce(char* key,
               int keybytes,
               char* values,
               int numValues,
               int lengths[],
               KeyValue* kv,
               void* ptr) {
  float floatA = *((float*) values);
  float floatB = *((float*) (values + sizeof(float)));
  float sum = floatA + floatB;
  kv->add(key, keybytes, (char*) &sum, sizeof(float));
}

float Vector::max() {
  return 10.0; // TODO
}

void Vector::print() {
  scan(&printScan, NULL);
}

void printScan(char *key, int keybytes, char *value, int valuebytes, void *ptr) {
  printf("Key: %d, Val: %f\n", *((int*) key), *((float*) value));
}
