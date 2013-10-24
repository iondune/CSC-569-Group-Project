#include "deps/src/inc/mapreduce.h"

namespace MAPREDUCE_NS {
class KeyValue;
}  // namespace MAPREDUCE_NS

class Vector : private MAPREDUCE_NS::MapReduce {
 public:
  /**
   * Create a Vector* from a pointer to a string of floats. Break the data
   * up into chunks of size |chunkSize|, for parallelization.
   */
  static Vector* from(char* data, int chunkSize);

  /**
   * Add two Vectors together.
   */
  static Vector* add(Vector* vecA, Vector* vecB);

  /**
   * Get the maximum value.
   */
  float max();

  /**
   * Print key/value pairs to stdout.
   */
  void print();

 private:
  Vector(MPI_Comm);
  uint64_t add(Vector* other);


  static void handleVectorChunk(char* data,
                                int ordinal,
                                const char delim,
                                int chunkSize,
                                int count,
                                void* extra);
};
