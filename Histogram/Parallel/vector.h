#include "mapreduce.h"

template <typename T>
static T const Clamp(T const & v, T const & min, T const & max);


namespace MAPREDUCE_NS {
class KeyValue;
}  // namespace MAPREDUCE_NS

class Vector : private MAPREDUCE_NS::MapReduce {
 public:
  Vector(MPI_Comm comm);

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
   * Bin fills out-parameter |bins| with the counts of the binned values of this
   * vector. This Vector is destroyed in the process, and should not be used
   * any longer.
   *
   * @param min   The minimum value in the vector.
   * @param max   The maximum value in the vector
   * @param width The width of the bins.
   * @param bins  The bins to fill.
   */
  // KV -> (map variant 5, to bin/NULL pairs) -> KV -> (collate) -> KMV -> (reduce) -> KV -> scan, fill out param
  void bin(float min, float max, float width, int binCount, int* bins);

  /**
   * Print key/value pairs to stdout.
   */
  void print();

 private:
  //Vector(MPI_Comm comm);
  //uint64_t add(Vector* other);

  static void handleVectorChunk(char* data,
                                int ordinal,
                                const char delim,
                                int chunkSize,
                                int count,
                                void* extra);
};
