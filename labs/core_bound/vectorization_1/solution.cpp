#include "solution.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <immintrin.h>
#include <type_traits>


using simd_sequence_t = std::vector<uint8_t>;
using simd_matrix_t = std::array<simd_sequence_t, sequence_size_v>;

simd_matrix_t transpose(const std::vector<sequence_t>& sequences) {
  simd_matrix_t result{};
  for (int i = 0; i < result.size(); ++i) {
    for (int j = 0; j < sequences.size(); ++j) {
      result[i][j] = sequences[j][i];
    }
  }

  return result;
}

// The alignment algorithm which computes the alignment of the given sequence
// pairs.
result_t compute_alignment(std::vector<sequence_t> const &sequences1,
                           std::vector<sequence_t> const &sequences2) {
  result_t result{};

  auto simd_sequences1 = transpose(sequences1);
  auto simd_sequences2 = transpose(sequences2);

  // __m256i v_match = _mm256_set1_epi8(match);

  std::vector<__m256i> simd_sequence1_vec;
  std::vector<__m256i> simd_sequence2_vec;

  using score_t = int16_t;
  using column_t = std::array<score_t, sequence_size_v + 1>;

  score_t gap_open{-11};
  score_t gap_extension{-1};
  score_t match{6};
  score_t mismatch{-4};

  column_t score_column{};
  column_t horizontal_gap_column{};
  score_t last_vertical_gap{};

  __m256i simd_match = _mm256_set1_epi16(match);
  __m256i simd_mismatch = _mm256_set1_epi16(mismatch);

  std::array<__m256i, sequence_size_v + 1> simd_score_column{};
  std::array<__m256i, sequence_size_v + 1> simd_horizontal_gap_column{};
  __m256i simd_last_vertical_gap = _mm256_set1_epi16(gap_open);

  // Pray that compiler will vectorize this loop.
  for (int i = 0; i < sequence_size_v; ++i) {
    simd_horizontal_gap_column[i] = _mm256_set1_epi16(gap_open);
  }

  simd_horizontal_gap_column[0] = _mm256_set1_epi16(gap_open);
  last_vertical_gap = gap_open;

  for (size_t i = 1; i < score_column.size(); ++i) {
    simd_score_column[i] = _mm256_set1_epi16(last_vertical_gap);
    simd_horizontal_gap_column[i] = _mm256_set1_epi16(last_vertical_gap + gap_open);
    last_vertical_gap += gap_extension;
  }

  for (unsigned col = 1; col <= simd_sequence2_vec.size(); ++col) {
    __m256i simd_last_diagonal_score = _mm256_set1_epi16(score_column[0]);
    score_column[0] = horizontal_gap_column[0];
    last_vertical_gap = horizontal_gap_column[0] + gap_open;
    horizontal_gap_column[0] += gap_extension;
    for (unsigned row = 1; row <= simd_sequence1_vec.size(); ++row) {
      __m256i simd_compare_result = _mm256_cmpeq_epi8(simd_sequence1_vec[row - 1], simd_sequence2_vec[col - 1]);
      __m256i blend_result = _mm256_blendv_epi8(simd_mismatch, simd_match, simd_compare_result);
      __m256i simd_best_cell_score = _mm256_adds_epi16(simd_last_diagonal_score, blend_result);

      simd_best_cell_score = _mm256_max_epi16(simd_best_cell_score, simd_last_vertical_gap);
      simd_best_cell_score = _mm256_max_epi16(simd_best_cell_score, simd_horizontal_gap_column[row]);
      // last_diagonal_score
      simd_last_diagonal_score = simd_score_column[row];
      simd_score_column[row] = simd_best_cell_score;
      simd_best_cell_score = _mm256_add_epi16(simd_best_cell_score, _mm256_set1_epi16(gap_open));
      simd_last_vertical_gap = _mm256_add_epi16(simd_last_vertical_gap, _mm256_set1_epi16(gap_extension));

      horizontal_gap_column[row] += gap_extension;
    //     // Store optimum between gap open and gap extension.
    //     last_vertical_gap = std::max(last_vertical_gap, best_cell_score);
    //     horizontal_gap_column[row] =
    //         std::max(horizontal_gap_column[row], best_cell_score);
    //   }
    }

    // for (unsigned row = 1; row <= sequence1.size(); ++row) {
    //     // Compute next score from diagonal direction with match/mismatch.
    //     score_t best_cell_score =
    //         last_diagonal_score +
    //         (sequence1[row - 1] == sequence2[col - 1] ? match : mismatch);
    //     // Determine best score from diagonal, vertical, or horizontal
    //     // direction.
    //     best_cell_score = std::max(best_cell_score, last_vertical_gap);
    //     best_cell_score = std::max(best_cell_score, horizontal_gap_column[row]);
    //     // Cache next diagonal value and store optimum in score_column.
    //     last_diagonal_score = score_column[row];
    //     score_column[row] = best_cell_score;
    //     // Compute the next values for vertical and horizontal gap.
    //     best_cell_score += gap_open;
    //     last_vertical_gap += gap_extension;
    //     horizontal_gap_column[row] += gap_extension;
    //     // Store optimum between gap open and gap extension.
    //     last_vertical_gap = std::max(last_vertical_gap, best_cell_score);
    //     horizontal_gap_column[row] =
    //         std::max(horizontal_gap_column[row], best_cell_score);
    //   }
  }


  /* ------------------------------------------------ */
  for (size_t sequence_idx = 0; sequence_idx < sequences1.size();
       ++sequence_idx) {
    using score_t = int16_t;
    using column_t = std::array<score_t, sequence_size_v + 1>;

    sequence_t const &sequence1 = sequences1[sequence_idx];
    sequence_t const &sequence2 = sequences2[sequence_idx];

    /*
     * Initialise score values.
     */
    score_t gap_open{-11};
    score_t gap_extension{-1};
    score_t match{6};
    score_t mismatch{-4};

    /*
     * Setup the matrix.
     * Note we can compute the entire matrix with just one column in memory,
     * since we are only interested in the last value of the last column in the
     * score matrix.
     */
    column_t score_column{};
    column_t horizontal_gap_column{};
    score_t last_vertical_gap{};

    /*
     * Initialise the first column of the matrix.
     */
    horizontal_gap_column[0] = gap_open;
    last_vertical_gap = gap_open;

    for (size_t i = 1; i < score_column.size(); ++i) {
      score_column[i] = last_vertical_gap;
      horizontal_gap_column[i] = last_vertical_gap + gap_open;
      last_vertical_gap += gap_extension;
    }

    /*
     * Compute the main recursion to fill the matrix.
     */
    for (unsigned col = 1; col <= sequence2.size(); ++col) {
      score_t last_diagonal_score =
          score_column[0]; // Cache last diagonal score to compute this cell.
      score_column[0] = horizontal_gap_column[0];
      last_vertical_gap = horizontal_gap_column[0] + gap_open;
      horizontal_gap_column[0] += gap_extension;

      for (unsigned row = 1; row <= sequence1.size(); ++row) {
        // Compute next score from diagonal direction with match/mismatch.
        score_t best_cell_score =
            last_diagonal_score +
            (sequence1[row - 1] == sequence2[col - 1] ? match : mismatch);
        // Determine best score from diagonal, vertical, or horizontal
        // direction.
        best_cell_score = std::max(best_cell_score, last_vertical_gap);
        best_cell_score = std::max(best_cell_score, horizontal_gap_column[row]);
        // Cache next diagonal value and store optimum in score_column.
        last_diagonal_score = score_column[row];
        score_column[row] = best_cell_score;
        // Compute the next values for vertical and horizontal gap.
        best_cell_score += gap_open;
        last_vertical_gap += gap_extension;
        horizontal_gap_column[row] += gap_extension;
        // Store optimum between gap open and gap extension.
        last_vertical_gap = std::max(last_vertical_gap, best_cell_score);
        horizontal_gap_column[row] =
            std::max(horizontal_gap_column[row], best_cell_score);
      }
    }

    // Report the best score.
    result[sequence_idx] = score_column.back();
  }

  return result;
}
