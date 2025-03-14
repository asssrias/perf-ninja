#include "solution.hpp"
#include <algorithm>
#include <iostream>

// #define SOLUTION

void transpose(MatrixOfDoubles &in, MatrixOfDoubles &out, int row_begin, int col_begin, int n) {
  // const int row_count = in.size();
  // const int col_count = in.front().size();

  if (n <= 32) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        out[row_begin + i][col_begin + j] = in[col_begin + j][row_begin + i];
      }
    }
  } else {
    int k = n / 2;
    
    transpose(in, out, row_begin, col_begin, k);
    transpose(in, out, row_begin, col_begin + k, k);
    transpose(in, out, row_begin + k, col_begin, k);
    transpose(in, out, row_begin + k, col_begin + k, k);

    /*std::swap(out[2][0], out[0][2]);
    std::swap(out[2][1], out[0][3]);
    for (int i = 2; 4;) {
      for (int j = 0; 2;) {
        std::swap(out[i][j], out[j][i + j]);
      }
    }*/

    for (int i = row_begin; i < row_begin + k; ++i) {
      for (int j = col_begin; j < col_begin + k; ++j) {
        /*if (i == 25 && (j + k + 1) == 1974) {
          std::cout << "12333333333";
        }*/
        //std::swap(out[i][j + k + 1], out[i + k + 1][j]);
        std::swap(out[i + k][j], out[i][j + k]);
      }
    }

    /*for (int i = row_begin + k; i < row_begin + 2 * k; i++) {
      for (int j = col_begin; j < col_begin + k; j++) {
        std::swap(out[i][j], out[j][j + k]);
      }
    }*/

    if (n & 1) {
      /*for (int i = col_begin; i <= col_begin + k; i++) {
        out[2 * k][i] = in[i][2 * k];
      }*/
      for (int i = col_begin; i < col_begin + n; i++) {
        out[i][row_begin + n - 1] = in[row_begin + n - 1][i];
        out[row_begin + n - 1][i] = in[i][row_begin + n - 1];
      }
    } 
  }
}

bool solution(MatrixOfDoubles &in, MatrixOfDoubles &out) {
  int size = in.size();
#ifdef SOLUTION
  const int row_count = in.size();
  const int col_count = in.front().size();

  const int l1_tile_len = 16;

  // for (int row = 0; row < row_count; row += l1_tile_len) {
  //   for (int col = 0; col < col_count; col += l1_tile_len) {
  //     for (int i = row; i < std::min(row + l1_tile_len, row_count); i++) {
  //       for (int j = col; j < std::min(col + l1_tile_len, col_count); j++) {
  //         out[i][j] = in[j][i];
  //       }
  //     }
  //   }
  // }
  transpose (in, out, 0, 0, size);
  
  
#else
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      out[i][j] = in[j][i];
    }
  }
#endif
  return out[0][size - 1];
}
