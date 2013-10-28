#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace MAPREDUCE_NS;

struct Spec {
  int n,limit,min,binWidth,binCount,flag;
};

void sum(char *key, int keybytes, char *multivalue,
	 int nvalues, int *valuebytes, KeyValue *kv, void *ptr) 
{
  kv->add(key,keybytes,(char *) &nvalues,sizeof(int));
}

void output(uint64_t itask, char *key, int keybytes, char *value,
	    int valuebytes, KeyValue *kv, void *ptr)
{
  Spec *spec = (Spec *) ptr;
  spec->n++;
  if (spec->n > spec->limit) return;

  float n = *(float *) value;
  int index = (int) (n - spec->min) / spec->binWidth) % spec->binCount;
  int bin = Clamp(index, 0, spec->binCount);
  int count = 1;
  if (spec->flag) printf("%f %d\n",value,bin);
  else kv->add((char *) &bin,sizeof(int),(char *) &count,sizeof(int));
}

template <typename T>
static T const Clamp(T const & v, T const & min, T const & max)
{
    return ((v >= max) ? max-1 : ((v < min) ? min : v));
}

