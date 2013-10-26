#include "src/vector/vector.h"

#include <gtest/gtest.h>

#include <boost/scoped_ptr.hpp>
#include <vector>

#define CHUNK_SIZE 1
#define NUM_PROCS 1

using boost::scoped_ptr;
using std::vector;

TEST(VectorTest, TestFrom) {
  char data[] = "1.0 2.0 3.0 4.0 5.0";
  scoped_ptr<Vector> vec(Vector::from(data, CHUNK_SIZE, NUM_PROCS));
  vector<float> values = vec->values();

  ASSERT_EQ(values.size(), 5);
  ASSERT_EQ(values[0], 1.0);
  ASSERT_EQ(values[1], 2.0);
  ASSERT_EQ(values[2], 3.0);
  ASSERT_EQ(values[3], 4.0);
  ASSERT_EQ(values[4], 5.0);
}

TEST(VectorTest, TestAddSameLength) {
  char data1[] = "1.0 2.0 3.0";
  char data2[] = "4.0 5.0 6.0";
  scoped_ptr<Vector> vec1(Vector::from(data1, CHUNK_SIZE, NUM_PROCS));
  scoped_ptr<Vector> vec2(Vector::from(data2, CHUNK_SIZE, NUM_PROCS));
  scoped_ptr<Vector> vecSum(vec1->add(vec2.get()));
  vector<float> values = vecSum->values();

  ASSERT_EQ(values.size(), 3);
  ASSERT_EQ(values[0], 5.0);
  ASSERT_EQ(values[1], 7.0);
  ASSERT_EQ(values[2], 9.0);
}
