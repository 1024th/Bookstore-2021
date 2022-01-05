#ifndef BOOKSTORE_CHAR_H_
#define BOOKSTORE_CHAR_H_

#include <cstring>
#include <iostream>

template<const int size = 64>
class Char {
  char content[size + 1];

 public:
  // 默认为空字符串
  Char() { content[0] = '\0'; }
  Char(const std::string &s) { strcpy(content, s.c_str()); }
  Char(const char *cstr) { strcpy(content, cstr); }
  operator std::string() const { return str(); }
  std::string str() const { return std::string(content); }
  char *c_str() { return content; }
  char &operator[](int index) { return content[index]; }
  char operator[](int index) const { return content[index]; }
  Char &operator=(const Char &that) {
    if (this == &that) return *this;
    strcpy(content, that.content);
    return *this;
  }
  bool empty() const { return content[0] == '\0'; }
  friend bool operator<(const Char<size> &a, const Char<size> &b) { return strcmp(a.content, b.content) < 0; }
  friend bool operator==(const Char<size> &a, const Char<size> &b) { return strcmp(a.content, b.content) == 0; }
  friend bool operator>(const Char<size> &a, const Char<size> &b) { return strcmp(a.content, b.content) > 0; }
  friend bool operator<=(const Char<size> &a, const Char<size> &b) { return !(operator>(a, b)); }
  friend bool operator>=(const Char<size> &a, const Char<size> &b) { return !(operator<(a, b)); }
  friend bool operator!=(const Char<size> &a, const Char<size> &b) { return !(operator==(a, b)); }
  friend std::istream &operator>>(std::istream &is, Char<size> &s) { return is >> s.content; }
  friend std::ostream &operator<<(std::ostream &os, const Char<size> &s) { return os << s.content; }
};

#endif  // BOOKSTORE_CHAR_H_