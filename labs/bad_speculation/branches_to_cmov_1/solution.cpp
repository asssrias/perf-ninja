#include "solution.hpp"
// #define SOLUTION
// Simulates N steps of the game for each starting grid 
// and return population count
std::vector<int> solution(const std::vector<Life::Grid>& grids) {
  std::vector<int> popCounts;
  popCounts.reserve(grids.size());

#ifdef SOLUTION
  std::vector<Life::Grid> grids_with_padding;
  grids_with_padding.resize(grids.size());

  for (int i = 0; i < grids_with_padding.size(); ++i) {
    grids_with_padding[i].resize(GridXDimension + 2);

    grids_with_padding[i][0].resize(GridYDimension + 2);
    grids_with_padding[i].back().resize(GridYDimension + 2);
    std::fill(grids_with_padding[i][0].begin(), grids_with_padding[i][0].end(), 0);
    std::fill(grids_with_padding[i].back().begin(), grids_with_padding[i].back().end(), 0);

    for (int j = 1; j < GridXDimension + 1; j++) {
      grids_with_padding[i][j].resize(GridYDimension + 2);
      grids_with_padding[i][j][0] = 0;
      grids_with_padding[i][j].back() = 0;
      std::copy(grids[i][j - 1].begin(), grids[i][j - 1].end(), grids_with_padding[i][j].begin() + 1);
    }
  }

  Life life;
  for (auto& grid : grids_with_padding) {
    life.reset(grid);
    for (int i = 0; i < NumberOfSims; i++)
      life.simulateNext();
    popCounts.push_back(life.getPopulationCount());
  }
#else

  Life life;
  for (auto& grid : grids) {
    life.reset(grid);
    for (int i = 0; i < NumberOfSims; i++)
      life.simulateNext();
    popCounts.push_back(life.getPopulationCount());
  }

#endif
  return popCounts;
}
