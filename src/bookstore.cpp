#include "bookstore.h"
#include <filesystem>
#include "book_manager.h"
#include "command_parser.h"
#include "logger.h"
#include "user_manager.h"

Bookstore::Bookstore() {
  if (!std::filesystem::exists("data"))
    std::filesystem::create_directory("data");
  logger = new Logger;
  user_manager = new UserManager;
  book_manager = new BookManager;
  command_parser = new CommandParser;
  logger->bookstore = this;
  user_manager->bookstore = this;
  book_manager->bookstore = this;
  command_parser->bookstore = this;
}
Bookstore::~Bookstore() {
  delete logger;
  delete user_manager;
  delete book_manager;
  delete command_parser;
}
void Bookstore::Run() {
  command_parser->Run();
}