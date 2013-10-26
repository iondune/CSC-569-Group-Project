#include "src/vector/vector.h"

#include <gtest/gtest.h>

#include <vector>

#define CHUNK_SIZE 1
#define NUM_PROCS 1

using std::string;
using std::vector;

static Vector* vec(const string& data) {
  string copy = string(data);
  return Vector::from(const_cast<char*>(copy.c_str()), CHUNK_SIZE, NUM_PROCS);
}

// from with no data doesn't crash
TEST(VectorTest, TestFromNoData) {
  vec("");
}

// from works as expected
TEST(VectorTest, TestFrom) {
  Vector* v = vec("1.0 2.0 3.0 4.0 5.0");
  vector<float> values = v->values();

  ASSERT_EQ(values.size(), 5);
  ASSERT_EQ(values[0], 1.0);
  ASSERT_EQ(values[1], 2.0);
  ASSERT_EQ(values[2], 3.0);
  ASSERT_EQ(values[3], 4.0);
  ASSERT_EQ(values[4], 5.0);
}

// add does not modify either argument's values.
TEST(VectorTest, TestAddDoesntModifyArgs) {
  Vector* v1 = vec("1.0 2.0 3.0");
  Vector* v2 = vec("4.0 5.0 6.0");
  v1->add(v2);
  vector<float> values1 = v1->values();
  vector<float> values2 = v2->values();

  ASSERT_EQ(values1.size(), 3);
  ASSERT_EQ(values1[0], 1.0);
  ASSERT_EQ(values1[1], 2.0);
  ASSERT_EQ(values1[2], 3.0);

  ASSERT_EQ(values2.size(), 3);
  ASSERT_EQ(values2[0], 4.0);
  ASSERT_EQ(values2[1], 5.0);
  ASSERT_EQ(values2[2], 6.0);
}

// adding two vectors of unequal lengths results in NULL
TEST(VectorTest, TestAddUnequalLength) {
  Vector* v1 = vec("1.0 2.0 3.0");
  Vector* v2 = vec("4.0 5.0");
  Vector* sum = v1->add(v2);
  ASSERT_FALSE(sum);
}

// adding two vectors of equal lengths works as expected
TEST(VectorTest, TestAddSameLength) {
  Vector* v1 = vec("1.0 2.0 3.0");
  Vector* v2 = vec("4.0 5.0 6.0");
  Vector* sum = v1->add(v2);
  vector<float> values = sum->values();

  ASSERT_EQ(values.size(), 3);
  ASSERT_EQ(values[0], 5.0);
  ASSERT_EQ(values[1], 7.0); ASSERT_EQ(values[2], 9.0);
}

// max works as expected
TEST(VectorTest, TestMax) {
  Vector* v = vec("1.0 5.0 2.0");
  ASSERT_EQ(v->max(), 5.0);
}
