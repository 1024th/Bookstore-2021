#include <iostream>
#include <sstream>
#include <unordered_map>

#include "book_manager.h"
#include "logger.h"
#include "user_manager.h"
#include "exceptions.h"

class CommandParser {
 private:
  // 储存与该类关联的 UserManager 和 BookManager
  UserManager &user_manager;
  BookManager &book_manager;
  Logger &logger;
  // 根据指令的第一个单词查找对应的函数，供 Run 函数使用
  std::unordered_map<std::string, void (CommandParser::*)(const std::vector<std::string> &)>
      map_function{{"su", &CommandParser::ParseSu},
                   {"logout", &CommandParser::ParseLogout},
                   {"register", &CommandParser::ParseRegister},
                   {"passwd", &CommandParser::ParsePasswd},
                   {"useradd", &CommandParser::ParseUseradd},
                   {"delete", &CommandParser::ParseDelete},
                   {"show", &CommandParser::ParseShow},
                   {"buy", &CommandParser::ParseBuy},
                   {"select", &CommandParser::ParseSelect},
                   {"modify", &CommandParser::ParseModify},
                   {"import", &CommandParser::ParseImport},
                   {"report", &CommandParser::ParseReport},
                   {"log", &CommandParser::ParseLog}};
 public:
  void Run();  // 循环读入指令并解析，直到遇到 quit 或 exit
  // 构造 CommandParser，将其与所给的 UserManager，BookManager 和 Logger 关联起来
  CommandParser(UserManager &user_manager_, BookManager &book_manager_, Logger &logger_);

  void ParseSu(const std::vector<std::string> &args);  // 解析 su [User-ID] ([Password])?
  void ParseLogout(const std::vector<std::string> &args);  // 解析 logout
  void ParseRegister(const std::vector<std::string> &args);  // 解析 register [User-ID] [Password] [User-Name]
  void ParsePasswd(const std::vector<std::string> &args);  // 解析 passwd [User-ID] ([Old-Password])? [New-Password]
  void ParseUseradd(const std::vector<std::string> &args);  // 解析 useradd [User-ID] [Password] [Priority] [User-Name]
  void ParseDelete(const std::vector<std::string> &args);  // 解析 delete [User-ID]

  void ParseShow(const std::vector<std::string> &args);  // 解析 show，调用 showBook 或 showFinance

  void ParseShowBook(
      const std::vector<std::string> &args);  // 解析 show (-ISBN=[ISBN] | -name="[Book-Name]" | -author="[Author]" | -keyword="[Keyword]")?
  void ParseBuy(const std::vector<std::string> &args);  // 解析 buy [ISBN] [Quantity]
  void ParseSelect(const std::vector<std::string> &args);  // 解析 select [ISBN]
  void ParseModify(const std::vector<std::string> &args);  // 解析 modify (-ISBN=[ISBN] | -name="[Book-Name]" | -author="[Author]" |
  // -keyword="[Keyword]" | -price=[Price])+
  void ParseImport(const std::vector<std::string> &args);  // 解析 import [Quantity] [Total-Cost]

  void ParseReport(const std::vector<std::string> &args);  // 解析 report myself 或 report finance 或 report employee
  void ParseShowFinance(const std::vector<std::string> &args);  // 解析 show finance ([Time])?
  void ParseLog(const std::vector<std::string> &args);  // 解析 log
 private:
  static void SplitStr(const std::string &s, std::vector<std::string> &fragments, char delim = ' ');
  static bool Check(const std::string &s, bool (*validator)(char), int max_len);
  static bool CheckUserID(const std::string &);
  static bool CheckPrintable(const std::string &s, int max_len);
  static bool CheckInt(const std::string &s, int max_len);
  static bool CheckPassword(const std::string &s);
  static bool CheckPriority(const std::string &s);
  static bool IsPrintable(char c);
  static bool CheckISBN(const std::string &s);
  static bool CheckUserName(const std::string &s);
  static bool CheckBookName(const std::string &s);
  static bool CheckAuthor(const std::string &s);
  static bool CheckKeyword(const std::string &s);
  static bool CheckFloat(const std::string &s);
  static std::unordered_map<std::string, BookManager::ArgType> map_book_argument;
  static int ParseInt(const std::string &s);
  static std::unordered_map<BookManager::ArgType, bool (*)(const std::string &)> map_book_argument_checker;
  template<bool no_price, bool no_multiple_keyword>
  static BookManager::Argument ParseFilter(const std::string &s);
};