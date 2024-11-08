#include "solution.hpp"
#include <cstdint>

#define SIMD
#define SOLUTION

#ifdef SOLUTION
#ifdef SIMD

// 1101
// 

// 111 7
// 110 6
// 1101 13
// 0110
// hand craft simd version checksum
#include <immintrin.h>
uint16_t checksum(const Blob &blob) {
  uint32_t acc = 0;

  for (int i = 0; i < N; ++i) {
    acc += blob[i];
  }

  while (acc >> 16)
    acc = (acc >> 16) + (acc & 0xffff); 
  
  return (uint16_t)acc;
}

#else // SIMD

#define UNROLL_FACTOR 8

// auto vectorization version checksum
uint16_t checksum(const Blob &blob) {
  uint16_t acc = 0;
  uint32_t temp_acc = 0;

  for (int i = 0; i < N; ++i) {
    temp_acc += blob[i];
  }
  
  while (temp_acc >> 16)
    temp_acc = (temp_acc >> 16) + (temp_acc & 0xffff); 
  
  acc = (uint16_t)temp_acc;
  return acc;
}

#endif // SIMD
#else // SOLUTION
uint16_t checksum(const Blob &blob) {
  uint16_t acc = 0;
  for (auto value : blob) {
    acc += value;
    acc += acc < value; // add carry
  }
  return acc;
}

#endif // SOLUTION