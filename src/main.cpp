#include "deps/src/inc/keyvalue.h"

#include <cstdio>
#include <cstdlib>

#include "src/vector/vector.h"
#include "src/string/util.h"

#define CHUNK_SIZE 1

using MAPREDUCE_NS::KeyValue;

int main(int argc, char** argv) {
  char a[] = "1.5 2.5 3.5";
  char b[] = "4 5 6";

  MPI_Init(&argc, &argv);

  Vector* vecA = Vector::from(a, CHUNK_SIZE);
  Vector* vecB = Vector::from(b, CHUNK_SIZE);
  Vector* vecC = vecA->add(vecB);

  delete vecA;
  delete vecB;
  delete vecC;
}
