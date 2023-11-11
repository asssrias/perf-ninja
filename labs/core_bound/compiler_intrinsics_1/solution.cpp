
#include "solution.h"
#include <memory>

#if defined(__aarch64__) || defined(_M_ARM64)
#define USE_NEON
#include <arm_neon.h>
using input_vec_t = uint8x8_t;
using output_vec_t = uint16x8_t;
#else
#include <emmintrin.h>
using input_vec_t = __m128i;
using output_vec_t = __m128i;
#endif

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

  // 2. main loop.
  limit = size - radius;

  constexpr int step = 8;

  auto *lhsPtr = input.data() + pos - radius - 1;
  auto *rhsPtr = input.data() + pos + radius;
  auto *outPtr = reinterpret_cast<output_vec_t *>(output.data() + pos);

  for (auto p = pos; p < limit; p += step) {
#ifdef USE_NEON
    auto lhs = *reinterpret_cast<const input_vec_t *>(lhsPtr);
    auto rhs = *reinterpret_cast<const input_vec_t *>(rhsPtr);
    *outPtr = vsubl_u8(rhs, lhs);
#else
    *outPtr = _mm_cvtepu8_epi16(_mm_loadu_si64(rhsPtr)) - _mm_cvtepu8_epi16(_mm_loadu_si64(lhsPtr));
#endif
    lhsPtr+=step;
    rhsPtr+=step;
    outPtr++;
  }

  for (; pos < limit; ++pos) {
    currentSum += output[pos];
    output[pos] = currentSum;
  }


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
