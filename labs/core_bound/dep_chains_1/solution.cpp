#include "solution.hpp"
#include <array>
#include <iostream>

unsigned getSumOfDigits(unsigned n) {
  unsigned sum = 0;
  while (n != 0) {
    sum = sum + n % 10;
    n = n / 10;
  }
  return sum;
}

// Task: lookup all the values from l2 in l1.
// For every found value, find the sum of its digits.
// Return the sum of all digits in every found number.
// Both lists have no duplicates and elements placed in *random* order.
// Do NOT sort any of the lists. Do NOT store elements in a hash_map/sets.

// Hint: Traversing a linked list is a long data dependency chain:
//       to get the node N+1 you need to retrieve the node N first.
//       Think how you can execute multiple dependency chains in parallel.
#ifdef SOLUTION
const int M = 4;
unsigned solution(List *l1, List *l2) {
  unsigned retVal = 0;

  List *head2 = l2;
  List *head1 = l1;
  // O(N^2) algorithm:

  size_t l1_len = 0;
  while (l1) {
    l1_len++;
    l1 = l1->next;
  }

  l1 = head1;

  for (int i = 0; i < l1_len / M; i++) {
    std::array<unsigned, M> dofs;
    for (int j = 0; j < M; ++j) {
      dofs[j] = l1->value;
      l1 = l1->next;
    }
    l2 = head2;
    int found = 0;
    while (l2) {
      unsigned v = l2->value;
      for (int j = 0; j < M; ++j) {
        if (dofs[j] == v) {
          retVal += getSumOfDigits(v);
          if (++found == M)
            break;
        }
      }
      l2 = l2->next;
    }
  }

  while (l1) {
    unsigned v = l1->value;
    l2 = head2;
    while (l2) {
      if (l2->value == v) {
        retVal += getSumOfDigits(v);
        break;
      }
      l2 = l2->next;
    }
    l1 = l1->next;
  }

  return retVal;
}

#else
unsigned solution(List *l1, List *l2) {
  unsigned retVal = 0;

  List *head2 = l2;
  // O(N^2) algorithm:
  while (l1) {
    unsigned v = l1->value;
    l2 = head2;
    while (l2) {
      if (l2->value == v) {
        retVal += getSumOfDigits(v);
        break;
      }
      l2 = l2->next;
    }
    l1 = l1->next;
  }

  return retVal;
}
#endif