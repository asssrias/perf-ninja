#include "solution.hpp"
#include <cstdio>
#include <immintrin.h>
#include <iostream>
#include <vector>
#include <numeric>

#define SOLUTION

// Find the longest line in a file.
// Implementation uses ternary operator with a hope that compiler will
// turn it into a CMOV instruction.
// The code inside the inner loop is equivalent to:
/*
if (s == '\n') {
  longestLine = std::max(curLineLength, longestLine);
  curLineLength = 0;
} else {
  curLineLength++;
}*/
unsigned solution(const std::string &inputContents) {
  unsigned longestLine = 0;
  unsigned curLineLength = 0;

#ifdef SOLUTION
const int VECTORIZATION_FACTOR = 32;
  size_t i = 0;
  __m256i newline_256 = _mm256_set1_epi8('\n');
  std::vector<int> res_vec;
  res_vec.reserve(inputContents.size() / 32 + 1);
  for (; i + VECTORIZATION_FACTOR < inputContents.size(); i += VECTORIZATION_FACTOR) {
    auto str_256 = _mm256_loadu_si256((const __m256i*) (inputContents.data() + i));
    auto cmp_result = _mm256_cmpeq_epi8(str_256, newline_256);
    auto mask = _mm256_movemask_epi8(cmp_result);
    res_vec.push_back(mask);
    // if (mask) {
    //   for (int j = 0; j < VECTORIZATION_FACTOR; j++) {
    //     if (mask & (1 << j)) {
    //       longestLine = std::max(curLineLength, longestLine);
    //       curLineLength = 0;
    //     } else
    //       curLineLength++;
    //   }
    // } else {
    //   curLineLength += VECTORIZATION_FACTOR;
    // }
  }

  for (int i : res_vec) {
    if (!i) {
      curLineLength += VECTORIZATION_FACTOR;
      continue;
    } else {
      while (i) {
        int temp = i;
        int leading_zero_len = _lzcnt_u32(temp);
        curLineLength += leading_zero_len;
        longestLine = std::max(curLineLength, longestLine);
        i <<= (leading_zero_len + 1);
        curLineLength = 0;
      }
    }
  }
#else
  for (auto s : inputContents) {
    curLineLength = (s == '\n') ? 0 : curLineLength + 1;
    longestLine = std::max(curLineLength, longestLine);
  }
#endif
  return longestLine;
}
