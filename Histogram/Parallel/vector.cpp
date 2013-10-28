#include "vector.h"

#include <cstdlib> // atof
#include <cstring>
#include <vector>

#include "deps/src/inc/mapreduce.h"
#include "deps/src/inc/keyvalue.h"
#include "strutil.h"

using MAPREDUCE_NS::KeyValue;
using std::vector;

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

static int getBinNum(float min, float max, float width, float val);

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

static void valuesScan(char* keyStr,
                       int keyLen,
                       char* valueStr,
                       int valueLen,
                       void* extra);

static void printScan(char *key,
                      int keybytes,
                      char *value,
                      int valuebytes,
                      void *ptr);

typedef struct {
  float min;
  float max;
  float width;
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

Vector* Vector::from(char* data, int chunkSize, int numProcs) {
  Vector* vec = new Vector(MPI_COMM_WORLD);
  withChunksSpace(data, chunkSize, numProcs, vec, &handleVectorChunk);
  return vec;
}

void Vector::handleVectorChunk(char* data,
                               int ordinal,
                               const char delim,
                               int chunkSize,
                               int count,
                               int numProcs,
                               void* extra) {
  Vector* vec = (Vector*) extra;
  FromExtra extraData = { data, ordinal, chunkSize, count };
  vec->map(numProcs, &mapChunk, &extraData, 1);
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

  // Gets set to true if, in addReduce, it is discovered that the vectors were
  // of unequal length.
  bool unequalLength = false; 
  sum->reduce(&addReduce, &unequalLength);
  if (unequalLength)
    return NULL;

  return static_cast<Vector*>(sum);
}

void addReduce(char* key,
               int keybytes,
               char* values,
               int numValues,
               int lengths[],
               KeyValue* keyValue,
               void* extra) {
  if (numValues != 2) {
    *((bool*) extra) = true;
    return;
  }

  float floatA = *((float*) values);
  float floatB = *((float*) (values + sizeof(float)));
  float sum = floatA + floatB;
  keyValue->add(key, keybytes, (char*) &sum, sizeof(float));
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

void Vector::bin(float min, float max, float width, int* bins) {
  BinExtra extra = { min, max, width };
  map(this, binMap, &extra);
  collate(NULL);
  reduce(binReduce, NULL);
  gather(1);
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
  int binNum = getBinNum(binExtra->min, binExtra->max, binExtra->width, val);
  keyValue->add((char*) &binNum, sizeof(binNum), NULL, 0);
}

int getBinNum(float min, float max, float width, float val) {
  int binCount = ceil((max - min) / width);
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

vector<float> Vector::values() {
  sort_keys(3); // 3 means compare two floats
  vector<float> values;
  gather(1);
  scan(&valuesScan, &values);
  return values;
}

void valuesScan(char* keyStr, int keyLen, char* valueStr, int valueLen, void* extra) {
  vector<float>* values = (vector<float>*) extra;
  float val = *((float*) valueStr);
  values->push_back(val);
}

void Vector::print() {
  gather(1);
  scan(&printScan, NULL);
}

void printScan(char* key, int keyLen, char* value, int valueLen, void* extra) {
  printf("Key: %d, Val: %f\n", *((int*) key), *((float*) value));
}
