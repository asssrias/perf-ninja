#include "solution.hpp"
#include <memory>
#include <utility>
#include <vector>

// #define SOLUTION
#ifdef SOLUTION
static std::size_t mapToBucket(std::size_t v) {
  static std::unique_ptr<std::vector<int>> buckets_ptr;

  if (!buckets_ptr) {
    buckets_ptr = std::make_unique<std::vector<int>>(100);
    for (int i = 13; i < 29; i++) buckets_ptr->at(i) = 1;
    for (int i = 29; i < 41; i++) buckets_ptr->at(i) = 2;
    for (int i = 41; i < 53; i++) buckets_ptr->at(i) = 3;
    for (int i = 53; i < 71; i++) buckets_ptr->at(i) = 4;
    for (int i = 71; i < 83; i++) buckets_ptr->at(i) = 5;
    for (int i = 83; i < 100; i++) buckets_ptr->at(i) = 6;
  }

  if (v < 100) return buckets_ptr->at(v);
  return DEFAULT_BUCKET;
}
#else
static std::size_t mapToBucket(std::size_t v) {
                              //   size of a bucket
  if      (v < 13)  return 0; //   13
  else if (v < 29)  return 1; //   16
  else if (v < 41)  return 2; //   12
  else if (v < 53)  return 3; //   12
  else if (v < 71)  return 4; //   18
  else if (v < 83)  return 5; //   12
  else if (v < 100) return 6; //   17
  return DEFAULT_BUCKET;
}
#endif

std::array<std::size_t, NUM_BUCKETS> histogram(const std::vector<int> &values) {
  std::array<std::size_t, NUM_BUCKETS> retBuckets{0};
  for (auto v : values) {
    retBuckets[mapToBucket(v)]++;
  }
  return retBuckets;
}
