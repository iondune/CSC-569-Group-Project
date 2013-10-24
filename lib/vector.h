#include "deps/src/inc/mapreduce.h"

namespace MAPREDUCE_NS {
class KeyValue;
}  // namespace MAPREDUCE_NS

class Vector : private MAPREDUCE_NS::MapReduce {
 public:
  Vector* copy();

  /**
   * Create a Vector* from a pointer to a string of floats. Break the data
   * up into chunks of size |chunkSize|, for parallelization.
   */
  static Vector* from(char* data, int chunkSize);

  /**
   * Add two Vectors together. Returns a new Vector (does not modify this one).
   */
  Vector* add(Vector* other);

  /**
   * Get the maximum value. Has the following side effects:
   *    The computation is gathered to one processor.
   *    The values are sorted.
   */
  float max();

  /**
   * Print key/value pairs to stdout.
   */
  void print();

 private:
  Vector(MPI_Comm comm);
  //uint64_t add(Vector* other);

  static void handleVectorChunk(char* data,
                                int ordinal,
                                const char delim,
                                int chunkSize,
                                int count,
                                void* extra);
};
