// NOTE: this lab is currently broken.
// After migrating to a new compiler version the speedup is no longer measurable consistently.
// You can still try to solve it to learn the concept, but the result is not guaranteed.

#include <array>
#include <cstdint>

// Assume those constants never change
constexpr int N = 10000;
constexpr int minRandom = 0;
constexpr int maxRandom = 100;

// FIXME: this data structure can be reduced in size
// #pragma pack(push,1)

#ifdef SOLUTION
struct S {
  float d;
  uint16_t l;
  uint8_t i;
  uint8_t s:7;
  bool b:1;

  bool operator<(const S &s) const { return this->i < s.i; }
};
#else
struct S {
  int i;
  long long l;
  short s;
  double d;
  bool b;

  bool operator<(const S &s) const { return this->i < s.i; }
};
#endif

// #pragma pack(pop)

// template<int N>
// class TD;
// TD<sizeof(S)> td;

void init(std::array<S, N> &arr);
S create_entry(int first_value, int second_value);
void solution(std::array<S, N> &arr);
