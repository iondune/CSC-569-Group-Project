#include "deps/src/inc/mapreduce.h"
#include "deps/src/inc/keyvalue.h"

#include "src/strutil.h"

#include <cstdio>

using MAPREDUCE_NS::KeyValue;
using MAPREDUCE_NS::MapReduce;

void mapChunk(int itask, KeyValue* keyValue, void* extra);
void handleChunk(char* data, int count, const char delim, int chunkSize, void* extra);
void myreduce(char* key, int keybytes, char* floatsStr, int numValues, int lengths[], KeyValue* kv, void* ptr);

MapReduce* mr;

typedef struct {
  char* data;
  int count;
  int chunkSize;
} ExtraData;

int main(int argc, char** argv) {
  char data[] = "1.5 2.5 3.5";

  MPI_Init(&argc, &argv);

  MapReduce* mr = new MapReduce(MPI_COMM_WORLD);
  withChunksSpace(data, 1, NULL, &handleChunk);
  mr->collate(NULL);
  mr->reduce(&myreduce, NULL);
  delete mr;
}

void mapChunk(int itask, KeyValue* keyValue, void* extra) {
  ExtraData* extraData = (ExtraData*) extra;
  if (extraData->count == 0)
    return;

  // First iteration is a special case, because that's how strtok rolls.
  int keyLen;
  char* key = itoa(itask * extraData->chunkSize, &keyLen);
  char* floatStr = firstToken(extraData->data, " ");
  keyValue->add(key, keyLen, floatStr, strlen(floatStr));

  for (int i = 1; i < extraData->count; ++i) {
    key = itoa(itask * extraData->chunkSize + i, &keyLen);
    floatStr = nextToken(" ");
    keyValue->add(key, keyLen, floatStr, strlen(floatStr));
  }
}

void handleChunk(char* data, int count, const char delim, int chunkSize, void* extra) {
  ExtraData extraData = { data, count, chunkSize };
  mr->map(1, &mapChunk, &extraData, 1);
}

void myreduce(char* key,
              int keybytes,
              char* floatsStr,
              int numValues,
              int lengths[],
              KeyValue* kv,
              void* ptr) {
  float floats[2];
  strToFloats(floatsStr, numValues, lengths, floats);

  float sum = floats[0] + floats[1];
  // TODO: Put (key, sum) key/value pair.
}
