#include "command_parser.h"

#include "bookstore.h"
#include "user_manager.h"
#include "logger.h"
#include "exceptions.h"

void CommandParser::SplitStr(const std::string &s, std::vector<std::string> &fragments, const char delim) {
  int i = 0;
  while (s[i] == delim) ++i;
  int j = i;

  for (; j < s.length(); ++j) {
    if (s[j] == delim) {
      fragments.push_back(s.substr(i, j - i));
      i = j;
      while (s[i] == delim) ++i;
      j = i;
    }
  }
  if (i < s.length()) fragments.push_back(s.substr(i, j - i));
}

bool CommandParser::Check(const std::string &s, bool (*validator)(char), int max_len) {
  int len = s.length();
  if (len > max_len) return false;
  for (int i = 0; i < len; ++i) {
    if (!validator(s[i])) return false;
  }
  return true;
}

bool CommandParser::CheckUserID(const std::string &s) {
  return Check(s, [](char c) {
    return (('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || ('0' <= c && c <= '9')
        || c == '_');
  }, 30);
}

bool CommandParser::CheckPassword(const std::string &s) {
  return CheckUserID(s);
}

bool CommandParser::IsPrintable(char c) { return 31 < c && c < 127; }

bool CommandParser::CheckPrintable(const std::string &s, int max_len) {
  return Check(s, IsPrintable, max_len);
}

bool CommandParser::CheckPriority(const std::string &s) {
//  return s == "7" || s == "3" || s == "1" || s == "0";
  return s == "7" || s == "3" || s == "1";
}

bool CommandParser::CheckInt(const std::string &s, int max_len) {
  return Check(s, [](char c) { return '0' <= c && c <= '9'; }, max_len);
}

bool CommandParser::CheckUserName(const std::string &s) {
  return CheckPrintable(s, 30);
}

bool CommandParser::CheckISBN(const std::string &s) {
  return CheckPrintable(s, 20);
}

bool CommandParser::CheckBookName(const std::string &s) {
  return Check(s, [](char c) { return IsPrintable(c) && c != '"'; }, 60);
}
bool CommandParser::CheckAuthor(const std::string &s) { return CheckBookName(s); }
bool CommandParser::CheckKeyword(const std::string &s) { return CheckBookName(s); }

bool CommandParser::CheckFloat(const std::string &s) {
  int pos = s.find('.');
  if (pos == std::string::npos) return CheckInt(s, 13);
  else return s.length() <= 13 && CheckInt(s.substr(0, pos), 13) && CheckInt(s.substr(pos + 1), 13);
}

void CommandParser::Run() {
  std::string line;
  while (std::getline(std::cin, line)) {
    try {
      if (line.length() > 1024) throw CommandTooLong();
      std::vector<std::string> args;
      SplitStr(line, args);
      if (args.empty()) continue;  // 空行
      if (args[0] == "quit" || args[0] == "exit") {
        if (args.size() == 1) break;
        else throw SyntaxError();
      }
      auto it = map_function.find(args[0]);
      if (it == map_function.end()) throw SyntaxError();
      (this->*(it->second))(args);
    } catch (const BasicException &e) {
      std::cout << "Invalid\n";
#ifdef MyDebug
      std::cout << e.what() << std::endl;
      std::cout << "Invalid command is: " << line << std::endl;
#endif  // MyDebug
    }
  }
}

//CommandParser::CommandParser(UserManager &user_manager_, BookManager &book_manager_, Logger &logger_) : user_manager(
//    user_manager_), book_manager(book_manager_), logger(logger_) {
//}

void CommandParser::ParseSu(const std::vector<std::string> &args) {
  int len = args.size();
  if (len < 2 || !CheckUserID(args[1])) throw SyntaxError();
  if (len == 2) bookstore->user_manager->Login(args[1]);
  else if (len == 3) bookstore->user_manager->Login(args[1], args[2]);
  else throw SyntaxError();
}

void CommandParser::ParseLogout(const std::vector<std::string> &args) {
  if (args.size() != 1) throw SyntaxError();
  bookstore->user_manager->Logout();
}

void CommandParser::ParseRegister(const std::vector<std::string> &args) {
  if (args.size() != 4) throw SyntaxError();
  if (!CheckUserID(args[1]) || !CheckPassword(args[2]) || !CheckUserName(args[3])) throw SyntaxError();
  bookstore->user_manager->Register(args[1], args[2], args[3]);
}

void CommandParser::ParsePasswd(const std::vector<std::string> &args) {
  int len = args.size();
  if (len < 3 || len > 4) throw SyntaxError();
  if (!CheckUserID(args[1]) || !CheckPassword(args[2]) || (len == 4 && !CheckPassword(args[3]))) throw SyntaxError();
  if (len == 3) bookstore->user_manager->ChangePassword(args[1], args[2]);
  if (len == 4) bookstore->user_manager->ChangePassword(args[1], args[2], args[3]);
}

void CommandParser::ParseUseradd(const std::vector<std::string> &args) {
  if (args.size() != 5) throw SyntaxError();
  if (!CheckUserID(args[1]) || !CheckPassword(args[2]) || !CheckPriority(args[3]) || !CheckUserName(args[4]))
    throw SyntaxError();
  bookstore->user_manager->CreateUser(args[1], args[2], std::stoi(args[3]), args[4]);
}

void CommandParser::ParseDelete(const std::vector<std::string> &args) {
  if (args.size() != 2 || !CheckUserID(args[1])) throw SyntaxError();
  bookstore->user_manager->Remove(args[1]);
}

void CommandParser::ParseShow(const std::vector<std::string> &args) {
  if (args.size() >= 2 && args[1] == "finance") ParseShowFinance(args);
  else ParseShowBook(args);
}

std::unordered_map<std::string, BookManager::ArgType>
    CommandParser::map_book_argument = {
    {"-ISBN", BookManager::ArgType::ISBN},
    {"-name", BookManager::ArgType::NAME},
    {"-author", BookManager::ArgType::AUTHOR},
    {"-keyword", BookManager::ArgType::KEYWORD},
    {"-price", BookManager::ArgType::PRICE},
};

std::unordered_map<BookManager::ArgType, bool (*)(const std::string &)>
    CommandParser::map_book_argument_checker = {
    {BookManager::ArgType::ISBN, CommandParser::CheckISBN},
    {BookManager::ArgType::NAME, CommandParser::CheckBookName},
    {BookManager::ArgType::AUTHOR, CommandParser::CheckAuthor},
    {BookManager::ArgType::KEYWORD, CommandParser::CheckKeyword},
    {BookManager::ArgType::PRICE, CommandParser::CheckFloat},
};

template<bool no_price, bool no_multiple_keyword>
BookManager::Argument CommandParser::ParseFilter(const std::string &s) {
  std::vector<std::string> filter;
  SplitStr(s, filter, '=');
  if (filter.size() != 2) throw SyntaxError();
  if constexpr (no_price) {  // 禁止出现 price 参数（用于 show）
    if (filter[0] == "-price") throw SyntaxError();
  }
  auto it = map_book_argument.find(filter[0]);
  if (it == map_book_argument.end()) throw SyntaxError();
  if constexpr (no_multiple_keyword) {  // 禁止出现多个关键词（用于 show）
    if (it->second == BookManager::ArgType::KEYWORD && filter[1].find('|') != std::string::npos)
      throw SyntaxError();
  }
  if (it->second != BookManager::ArgType::ISBN && it->second != BookManager::ArgType::PRICE) {  // 去除双引号
    if (filter[1].front() != '"' || filter[1].back() != '"') throw SyntaxError();
    filter[1] = filter[1].substr(1, filter[1].length() - 2);
  }
  if (filter[1].empty()) throw SyntaxError();  // 附加参数内容为空则操作失败
  if (!map_book_argument_checker[it->second](filter[1])) throw SyntaxError();
  if (it->second == BookManager::ArgType::PRICE) return BookManager::Argument(it->second, std::stod(filter[1]));
  return BookManager::Argument(it->second, filter[1]);
}

void CommandParser::ParseShowBook(const std::vector<std::string> &args) {
  if (args.size() == 1) {  // 无附加参数时，所有图书均满足要求
    bookstore->book_manager->ShowAllBooks();
    return;
  }
  if (args.size() != 2) throw SyntaxError();
  bookstore->book_manager->ShowBook(ParseFilter<true, true>(args[1]));
}

int CommandParser::ParseInt(const std::string &s) {
  if (!CheckInt(s, 10)) throw SyntaxError();
  long long quantity = std::stoll(s);
  if (quantity > 2147483647) throw SyntaxError();
  return quantity;
}

void CommandParser::ParseBuy(const std::vector<std::string> &args) {
  if (args.size() != 3) throw SyntaxError();
  if (!CheckISBN(args[1])) throw SyntaxError();
  bookstore->book_manager->BuyBook(args[1], ParseInt(args[2]));
}

void CommandParser::ParseSelect(const std::vector<std::string> &args) {
  if (args.size() != 2) throw SyntaxError();
  if (!CheckISBN(args[1])) throw SyntaxError();
  bookstore->book_manager->SelectBook(args[1]);
}

void CommandParser::ParseModify(const std::vector<std::string> &args) {
  int len = args.size();
  if (len == 1) throw SyntaxError();  // 附加参数内容为空则操作失败
  std::vector<BookManager::Argument> book_args;
  BookManager::ArgType arg_type;
  bool arg_type_used[5] = {false, false, false, false, false};
  for (int i = 1; i < len; ++i) {
    BookManager::Argument book_arg = ParseFilter<false, false>(args[i]);
    if (arg_type_used[book_arg.type]) throw SyntaxError();  // 有重复附加参数为非法指令
    arg_type_used[book_arg.type] = true;
    book_args.push_back(book_arg);
  }
  bookstore->book_manager->ModifyBook(book_args);
}

void CommandParser::ParseImport(const std::vector<std::string> &args) {
  if (args.size() != 3) throw SyntaxError();
  int quantity = ParseInt(args[1]);
  if (!CheckFloat(args[2])) throw SyntaxError();
  bookstore->book_manager->ImportBook(quantity, std::stod(args[2]));
}

void CommandParser::ParseReport(const std::vector<std::string> &args) {
  if (args.size() != 2) throw SyntaxError();
  if (args[1] == "myself") bookstore->logger->ReportMyself();
  else if (args[1] == "finance") bookstore->logger->ReportFinance();
  else if (args[1] == "employee") bookstore->logger->ReportEmployee();
  else throw SyntaxError();
}

void CommandParser::ParseShowFinance(const std::vector<std::string> &args) {
  if (args.size() == 2) {
    bookstore->logger->ShowFinance();
  } else if (args.size() == 3) {
    bookstore->logger->ShowFinance(ParseInt(args[2]));
  } else {
    throw SyntaxError();
  }
}

void CommandParser::ParseLog(const std::vector<std::string> &args) {
  if (args.size() != 1) throw SyntaxError();
  bookstore->logger->ShowLog();
}