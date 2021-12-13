#include <iostream>

#include "char.h"
#include "unrolled_linked_list.h"

UnrolledLinkedList<Char<>, int> ull("./book.dat");

using std::cin, std::cout, std::endl;

int main() {
  std::ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  int n;
  cin >> n;

  std::string command;
  Char<> bookname;
  int value;
  // [index] 和 [value] 均可能重复，但同一个 [index] 对应的 [value] 不可重复。
  while (n--) {
    cin >> command;
    if (command == "insert") {
      cin >> bookname >> value;
      ull.add(bookname, value);
    } else if (command == "find") {
      cin >> bookname;
      std::vector<int> result;
      ull.find(bookname, result);
      if (result.empty()) {
        cout << "null";
      } else {
        for (int i : result) cout << i << " ";
      }
      cout << "\n";
    } else if (command == "delete") {
      cin >> bookname >> value;
      ull.remove(bookname, value);
    }
  }
  return 0;
}