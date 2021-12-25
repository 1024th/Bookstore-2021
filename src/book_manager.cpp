#include "book_manager.h"

#include "bookstore.h"
#include "command_parser.h"
#include "logger.h"

BookManager::BookManager()
    : books("data/books.dat"),
      ISBN_index("data/book_ISBN.dat"),
      book_name_index("data/book_name.dat"),
      author_index("data/book_author.dat"),
      keyword_index("data/book_keyword.dat") {}

template <bool need_sort>
void BookManager::PrintBooks(std::vector<std::streamoff> &book_offsets) {
  if (book_offsets.empty()) {  // 无满足要求的图书时输出空行
    std::cout << '\n';
    return;
  }
  Book book;
  std::vector<Book> matched_books;
  for (auto i : book_offsets) {
    books.Get(book, i);
    matched_books.push_back(book);
  }
  if constexpr (need_sort) std::sort(matched_books.begin(), matched_books.end());  // 以 ISBN 字典升序输出图书信息
  for (auto &i : matched_books) {
    std::cout << i << '\n';
  }
}
void BookManager::ShowAllBooks() {
  bookstore->user_manager->CheckPermission(1);
  std::vector<std::streamoff> book_offsets;
  ISBN_index.GetAll(book_offsets);
  PrintBooks<false>(book_offsets);
}
void BookManager::ShowBook(const BookManager::Argument &arg) {
  bookstore->user_manager->CheckPermission(1);
  std::vector<std::streamoff> book_offsets;
  switch (arg.type) {
    case ArgType::ISBN:
      ISBN_index.FindAll(arg.value, book_offsets);
      break;
    case ArgType::NAME:
      book_name_index.FindAll(arg.value, book_offsets);
      break;
    case ArgType::AUTHOR:
      author_index.FindAll(arg.value, book_offsets);
      break;
    case ArgType::KEYWORD:
      keyword_index.FindAll(arg.value, book_offsets);
      break;
    default:
      throw BasicException();
  }
  PrintBooks<true>(book_offsets);
  //  if (arg.value == "bianco") {
  //    std::cout << "bianco!!! Print All Books:" << std::endl;
  //    std::vector<Book> all_books;
  //    books.GetAll(all_books);
  //    for (auto &i: all_books) {
  //      std::cout << i << std::endl;
  //    }
  //    std::cout << "[finished]";
  //  }
}

void BookManager::BuyBook(const std::string &ISBN, int quantity) {
  bookstore->user_manager->CheckPermission(1);
  std::vector<std::streamoff> book_offset;
  ISBN_index.FindAll(ISBN, book_offset);
  if (book_offset.empty()) throw NoBookMatched();
  Book book;
  books.Get(book, book_offset[0]);
  if (book.quantity < quantity) throw StockInsufficient();
  double total_cost = book.price * quantity;
  std::cout << std::fixed << std::setprecision(2) << total_cost << '\n';
  book.quantity -= quantity;
  books.Update(book, book_offset[0]);
  bookstore->logger->AddTransaction(Transaction(total_cost, book_offset[0], quantity));
}
void BookManager::SelectBook(const std::string &ISBN) {
  bookstore->user_manager->CheckPermission(3);
  std::vector<std::streamoff> book_offset;
  ISBN_index.FindAll(ISBN, book_offset);
  if (book_offset.empty()) {
    // 创建新书
    Book book(ISBN);
    std::streamoff offset = books.Add(book);
    ISBN_index.Add(book.ISBN, offset);
    bookstore->user_manager->SelectBook(offset);
  } else {
    bookstore->user_manager->SelectBook(book_offset[0]);
  }
}
void BookManager::ModifyBook(std::vector<Argument> &args) {
  bookstore->user_manager->CheckPermission(3);
  std::streamoff offset = bookstore->user_manager->GetSelectedBook();
  if (offset == 0) throw NoBookSelected();
  Book book;
  books.Get(book, offset);
  for (auto &arg : args) {
    // std::cout << arg << std::endl;
    switch (arg.type) {
      case ArgType::ISBN: {
        std::vector<std::streamoff> result;
        ISBN_index.FindAll(arg.value, result);
        if (!result.empty()) throw DuplicateISBN();
        ISBN_index.Remove(book.ISBN, offset);
        book.ISBN = arg.value;
        books.Update(book, offset);
        ISBN_index.Add(book.ISBN, offset);
        break;
      }
      case ArgType::NAME: {
        if (!book.name.empty()) book_name_index.Remove(book.name, offset);
        book.name = arg.value;
        books.Update(book, offset);
        book_name_index.Add(book.name, offset);
        break;
      }
      case ArgType::AUTHOR: {
        if (!book.author.empty()) author_index.Remove(book.author, offset);
        book.author = arg.value;
        books.Update(book, offset);
        author_index.Add(book.author, offset);
        break;
      }
      case ArgType::KEYWORD: {
        std::vector<std::string> keywords;
        bookstore->command_parser->SplitStr(arg.value, keywords, '|');
        for (int i = 0; i < keywords.size() - 1; ++i) {
          for (int j = 0; j < i; ++j) {
            if (keywords[i] == keywords[j]) throw SyntaxError();  // keyword 包含重复信息段则操作失败
          }
        }
        //        std::cout << "[keywords] ";
        //        for (auto i: keywords) {
        //          std::cout << i << " ";
        //        }
        //        std::cout << std::endl;
        if (!book.keyword.empty()) {
          std::vector<std::string> old_keywords;
          bookstore->command_parser->SplitStr(book.keyword, old_keywords, '|');
          for (auto &i : old_keywords) keyword_index.Remove(i, offset);
        }
        book.keyword = arg.value;
        books.Update(book, offset);
        for (auto &i : keywords) {
          keyword_index.Add(i, offset);
        }
        break;
      }
      case ArgType::PRICE: {
        book.price = arg.price;
        books.Update(book, offset);
        break;
      }
      default:
        throw BasicException();
    }
  }
}
void BookManager::ImportBook(int quantity, double total_cost) {
  // todo: log
  bookstore->user_manager->CheckPermission(3);
  std::streamoff offset = bookstore->user_manager->GetSelectedBook();
  if (offset == 0) throw NoBookSelected();
  Book book;
  books.Get(book, offset);
  book.quantity += quantity;
  books.Update(book, offset);
  bookstore->logger->AddTransaction(Transaction(-total_cost, offset, quantity));
}
