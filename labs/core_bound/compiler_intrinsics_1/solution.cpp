
#include "solution.h"
#include <emmintrin.h>
#include <immintrin.h>
#include <memory>
#include <cmath>

// vectorization width: 32bit
// need to align to 32bit
void cal_prefix_256bit(std::vector<uint32_t>& data) {
  // short path
  if (data.size() < 8) {
    for (int i = 1; i < data.size(); i++) {
      data[i] += data[i - 1];
      return;
    }
  }
  for (int i = 0; i + 8 < data.size(); i += 8) {
    // prefix sum here
    // load data
    __m256i data_256 = _mm256_load_si256((const __m256i *)data.data() + i);
    data_256 = _mm256_add_epi32(data_256, _mm256_slli_si256(data_256, 4));
    data_256 = _mm256_add_epi32(data_256, _mm256_slli_si256(data_256, 8));
    _mm256_storeu_si256((__m256i *)data.data() + i, data_256);
  }

  int j = 3;
  for (; j + 4 < data.size(); j += 4) {
    auto s = _mm_set1_epi32(data[j]);
    auto data_128 = _mm_loadu_si128((__m128i *)data.data() + j + 1);
    data_128 = _mm_add_epi32(data_128, s);
    _mm_storeu_si128((__m128i *)data.data() + j + 1, data_128);
  }
}

void imageSmoothing(const InputVector &input, uint8_t radius,
                    OutputVector &output) {
  int pos = 0;
  int currentSum = 0;
  int size = static_cast<int>(input.size());

  // 1. left border - time spend in this loop can be ignored, no need to
  // optimize it
  for (int i = 0; i < std::min<int>(size, radius); ++i) {
    currentSum += input[i];
  }

  int limit = std::min(radius + 1, size - radius);
  for (pos = 0; pos < limit; ++pos) {
    currentSum += input[pos + radius];
    output[pos] = currentSum;
  }

#ifdef SOLUTION
#define UNROLL_FACTOR 32
  limit = size - radius;

  const int INTERLEAVE_WIDTH = 16;

  int begin = pos;
  int end = limit;

  // for (int i = INTERLEAVE_WIDTH; i < )
  int i = begin;
  for (; i + INTERLEAVE_WIDTH < end - 1; i += INTERLEAVE_WIDTH) {
    __m256i current_sum_simd = _mm256_set1_epi16((uint16_t)currentSum);
    pos = i;
    auto left_input = _mm256_loadu_si256((const __m256i*)(input.data() + pos - radius - 1));
    auto right_input = _mm256_loadu_si256((const __m256i*)(input.data() + pos + radius));

    // prefix phase
    left_input = _mm256_subs_epi16(right_input, left_input);
    auto tmp = left_input;
    left_input = _mm256_add_epi16(left_input, _mm256_slli_si256(left_input, 2));
    left_input = _mm256_add_epi16(left_input, _mm256_slli_si256(left_input, 4));
    left_input = _mm256_add_epi16(left_input, _mm256_slli_si256(left_input, 8));


    // accumulate phase
    __m128i highpart = _mm256_extracti128_si256(left_input, 1);
    auto rightmost_left_input = _mm256_extract_epi16(left_input, INTERLEAVE_WIDTH / 2 - 1);
    __m128i d = _mm_set1_epi16(rightmost_left_input);
    highpart = _mm_add_epi16(highpart, d);
    left_input = _mm256_inserti128_si256(left_input, highpart, 1);
    current_sum_simd = _mm256_add_epi16(current_sum_simd, left_input);
    _mm256_storeu_si256((__m256i *)(output.data() + i), current_sum_simd);

    currentSum = output[i + INTERLEAVE_WIDTH - 1];
    // 1 2 3 4 5 6 7 8

    // 2  3   4   5   6   7   8   0
    // 6  9   2   6   8   3   5   0
    // 8  12  6   11  14  10  13  0
  }

  for (; i < end; ++i) {
    currentSum -= input[i - radius - 1];
    currentSum += input[i + radius];
    output[i] = currentSum;
  }
  pos = i;
  

#else

  // 2. main loop.
  limit = size - radius;
  for (; pos < limit; ++pos) {
    currentSum -= input[pos - radius - 1];
    currentSum += input[pos + radius];
    output[pos] = currentSum;

    // output[pos] = output[pos - 1] - input[pos - radius - 1] + input[pos + radius];
  }
#endif
  // 3. special case, executed only if size <= 2*radius + 1
  limit = std::min(radius + 1, size);
  for (; pos < limit; pos++) {
    output[pos] = currentSum;
  }

  // 4. right border - time spend in this loop can be ignored, no need to
  // optimize it
  for (; pos < size; ++pos) {
    currentSum -= input[pos - radius - 1];
    output[pos] = currentSum;
  }
}
