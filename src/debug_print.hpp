#ifndef BOOKSTORE_DEBUG_PRINT_HPP_
#define BOOKSTORE_DEBUG_PRINT_HPP_

#include <iostream>

template <class... Ts>
void Print(Ts const&... args) {
  std::cout << "[";
  ((std::cout << args << " "), ...);
  std::cout << "]";
  std::cout << std::endl;
}

template <class... Ts>
void PrintTight(Ts const&... args) {
  std::cout << "[";
  ((std::cout << args), ...);
  std::cout << "]";
  // std::cout << std::endl;
}

#endif  // BOOKSTORE_DEBUG_PRINT_HPP_