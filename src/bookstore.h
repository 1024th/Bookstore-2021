#ifndef BOOKSTORE_SRC_BOOKSTORE_H_
#define BOOKSTORE_SRC_BOOKSTORE_H_

class BookManager;
class CommandParser;
class Logger;
class UserManager;
class Bookstore {
  friend Logger;
  friend UserManager;
  friend BookManager;
  friend CommandParser;
  Logger *logger;
  UserManager *user_manager;
  BookManager *book_manager;
  CommandParser *command_parser;

 public:
  Bookstore();
  ~Bookstore();
  void Run();
};

#endif //BOOKSTORE_SRC_BOOKSTORE_H_
