#ifndef BOOKSTORE_SRC_COMMAND_PARSER_H_
#define BOOKSTORE_SRC_COMMAND_PARSER_H_

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "book_manager.h"

class Bookstore;
class CommandParser {
 private:
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
  Bookstore *bookstore;
  void Run();  // 循环读入指令并解析，直到遇到 quit 或 exit

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
  template<bool input_mode = false>
  static void SplitStr(const std::string &s, std::vector<std::string> &fragments, char delim = ' ');
 private:
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

// input_mode：用于读入指令，忽略字符串头尾多余的分隔符（空格），且将多个连续分隔符（空格）视为一个
template<bool input_mode>
void CommandParser::SplitStr(const std::string &s, std::vector<std::string> &fragments, const char delim) {
  int i = 0;
  if constexpr (input_mode)
    while (s[i] == delim) ++i;
  int j = i;

  for (; j < s.length(); ++j) {
    if (s[j] == delim) {
      fragments.push_back(s.substr(i, j - i));
      i = j + 1;
      if constexpr (input_mode) {
        while (s[i] == delim) ++i;
        j = i;
      }
    }
  }
  if constexpr (input_mode) {
    if (i < s.length()) fragments.push_back(s.substr(i, j - i));
  } else {
    fragments.push_back(s.substr(i, j - i));
  }
}

#endif