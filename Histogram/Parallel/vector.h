#include "deps/src/inc/mapreduce.h"

#include <vector>

using std::vector;

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
   * Add two Vectors together. Returns a new Vector (does not modify this one).
   * Returns NULL if the vectors are of unequal length.
   */
  Vector* add(Vector* other);

  void myGather();
  vector<float> myValues();

  /**
   * Get the maximum value. Has the following side effects:
   *    The computation is gathered to one processor.
   *    The values are sorted.
   * Calling max with an empty vector throws an MPI error.
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
  void bin(float min, float max, float width, int* bins);

  /**
   * The values contained in this vector. Sorts the vector internally, by index.
   */
  vector<float> values();

  /**
   * Print key/value pairs to stdout.
   */
  void print();

 private:
  Vector(MPI_Comm comm);

  // This callback needs to be a static member function as opposed to a C-style
  // naked function to access the private inherited MapReduce.
  static void handleVectorChunk(char* data,
                                int ordinal,
                                int chunkSize,
                                int count,
                                void* extra);
};



