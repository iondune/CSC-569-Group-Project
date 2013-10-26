#include "vector.h"

#include <cstdlib> // atof
#include <cstring>

#include "strutil.h"
#include "mapreduce.h"
#include "keyvalue.h"

//using MAPREDUCE_NS::MapReduce;
using MAPREDUCE_NS::KeyValue;

template <typename T>
static T const Clamp(T const & v, T const & min, T const & max)
{
    return ((v >= max) ? max-1 : ((v < min) ? min : v));
}

static void mapChunk(int itask, KeyValue* keyValue, void* extra);

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

static int maxCompare(char* str1, int len1, char* str2, int len2);

static void maxScan(char* keystr,
                    int keyLen,
                    char* valstr,
                    int valLen,
                    void* extra);

static void binMap(uint64_t itask,
                   char* keyStr,
                   int keyLen,
                   char* valStr,
                   int valLen,
                   KeyValue* keyValue,
                   void* extra);

static int getBinNum(float min, float max, float width, int binCount, float val);

static void binReduce(char* key,
                      int keyLen,
                      char* multiValue,
                      int numValues,
                      int* valueBytes,
                      KeyValue* keyValue,
                      void* extra);

static void binScan(char* keyStr,
                    int keyLen,
                    char* valStr,
                    int valLen,
                    void* extra);

typedef struct {
  float min;
  float max;
  float width;
  int binCount;
} BinExtra;

typedef struct {
  char* data;
  int ordinal;
  int chunkSize;
  int count;
} FromExtra;

// Fallthrough.
Vector::Vector(MPI_Comm comm) : MapReduce(comm) {
}

Vector* Vector::copy() {
  return static_cast<Vector*>(MapReduce::copy());
}

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

Vector* Vector::add(Vector* other) {
  MapReduce* sum = MapReduce::copy();
  sum->add(other);
  sum->collate(NULL);
  sum->reduce(&addReduce, NULL);
  return static_cast<Vector*>(sum);
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
  // Must gather first, because sort_values will only sort each processor's
  // KeyValue pairs.
  gather(1);
  sort_values(&maxCompare);

  float max;
  scan(&maxScan, &max);
  return max;
}

int maxCompare(char* str1, int len1, char* str2, int len2) {
  float val1 = *((float*) str1);
  float val2 = *((float*) str2);
  return val1 - val2;
}

void maxScan(char* keystr, int keyLen, char* valstr, int valLen, void* extra) {
  *((float*) extra) = *((float*) valstr);
}

void Vector::bin(float min, float max, float width, int binCount, int* bins) {
  BinExtra extra = { min, max, width, binCount };
  map(this, binMap, &extra);
  collate(NULL);
  reduce(binReduce, NULL);
  scan(binScan, bins);
}

void binMap(uint64_t itask,
            char* keyStr,
            int keyLen,
            char* valStr,
            int valLen,
            KeyValue* keyValue,
            void* extra) {
  BinExtra* binExtra = (BinExtra*) extra;
  float val = *((float*) valStr);
  int binNum = getBinNum(binExtra->min, binExtra->max, binExtra->width, binExtra->binCount, val);
  keyValue->add((char*) &binNum, sizeof(binNum), NULL, 0);
}

int getBinNum(float min, float max, float width, int binCount, float val) {
  int index = Clamp((int) ((val - min) / width), 0, binCount);
  return Clamp(index, 0, binCount);
}



void binReduce(char* key,
               int keyLen,
               char* multiValue,
               int numValues,
               int* valueBytes,
               KeyValue* keyValue,
               void* extra) {
  keyValue->add(key, keyLen, (char*) &numValues, sizeof(numValues));
}

void binScan(char* keyStr, int keyLen, char* valStr, int valLen, void* extra) {
  int* bins = (int*) extra;
  int index = *((int*) keyStr);
  int count = *((int*) valStr);

  bins[index] = count;
}

void Vector::print() {
  scan(&printScan, NULL);
}

void printScan(char *key, int keybytes, char *value, int valuebytes, void *ptr) {
  printf("Key: %d, Val: %f\n", *((int*) key), *((float*) value));
}
