#include <gtest/gtest.h>

// FIXME: This is only included because it transitively includes something that
// gives MPI_Init and MPI_Finalize
#include "deps/src/inc/mapreduce.h" 

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  MPI_Finalize();
  return result;
}
