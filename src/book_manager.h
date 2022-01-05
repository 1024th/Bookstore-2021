#ifndef BOOKSTORE_SRC_BOOK_MANAGER_H_
#define BOOKSTORE_SRC_BOOK_MANAGER_H_

#include <algorithm>
#include <iostream>
#include <vector>

#include "block_storage.h"
#include "char.h"
#include "logger.h"
#include "unrolled_linked_list.h"
#include "user_manager.h"

class Book {
 public:
  Char<20> ISBN;
  Char<60> name, author;
  Char<60> keyword;
  int quantity = 0;
  double price = 0.0;
  Book() = default;
  explicit Book(const std::string &ISBN) : ISBN(ISBN) {}
  friend bool operator<(const Book &a, const Book &b) { return a.ISBN < b.ISBN; }
  friend std::ostream &operator<<(std::ostream &os, const Book &book) {
    return os << book.ISBN << '\t' << book.name << '\t' << book.author << '\t' << book.keyword << '\t' << std::fixed
              << std::setprecision(2) << book.price << '\t' << book.quantity;
  }
};

// Note: ull for keyword may be 3e6, adjust block size
class Bookstore;
class BookManager {
 public:
  Bookstore *bookstore;
  enum ArgType { ISBN, NAME, AUTHOR, KEYWORD, PRICE };  // 参数类型
  struct Argument {
    ArgType type;
    const std::string value;
    double price;
    Argument(ArgType type, const std::string &value) : type(type), value(value), price() {}
    Argument(ArgType type, double price) : type(type), price(price), value() {}
    friend std::ostream &operator<<(std::ostream &os, const Argument &x) {
      return os << x.type << " " << x.value << " " << x.price;
    }
  };
  BookManager(/*UserManager &user_manager, Logger &logger*/);
  void ShowAllBooks();  // 输出所有图书
  void ShowBook(const Argument& arg);  // 检索图书
  void BuyBook(const std::string &ISBN, int quantity);  // 购买图书
  void SelectBook(const std::string &ISBN);  // 以当前账户选中指定图书
  void ModifyBook(std::vector<Argument> &args);  // 更新选中图书的信息
  void ImportBook(int quantity, double total_cost);  // 指定交易总额购入指定数量的选中图书
  Char<20> GetISBN(std::streamoff offset);
 private:
  BlockStorage<Book> books;
  UnrolledLinkedList<Char<20>, std::streamoff> ISBN_index;
  UnrolledLinkedList<Char<60>, std::streamoff> book_name_index;
  UnrolledLinkedList<Char<60>, std::streamoff> author_index;
  UnrolledLinkedList<Char<60>, std::streamoff> keyword_index;
  template <bool need_sort = true>
  void PrintBooks(std::vector<std::streamoff> &book_offsets);
};

#endif