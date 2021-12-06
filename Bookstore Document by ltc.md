# 书店管理系统开发文档

作者：林田川

代码风格：[Google 开源项目风格指南](https://zh-google-styleguide.readthedocs.io/en/latest/)

## 文件结构

- `main.cpp` 入口点
- `src/`
  - `CommandParser.h` 指令解析
  - `CommandParser.cpp`
  - `AccountManager.h` 用户管理
  - `AccountManager.cpp`
  - `BookManager.h` 图书管理
  - `BookManager.cpp`
  - `Logger.h` 日志系统
  - `Logger.cpp`
  - `Exceptions.h` 异常处理
  - `Exceptions.cpp`
  - `BasicFileIO.h` 基础文件读写类
  - `BasicFileIO.cpp`
  - `UnrolledLinkedList.h` 块状链表
  - `UnrolledLinkedList.cpp`
- `data/`
  - `books.dat` 储存的书本信息
  - `ISBN.index` 按 ISBN 升序储存对应书本的在 `books.dat` 中的位置
  - `bookname.index` 按书名升序储存上述内容
  - `author.index` 按作者升序储存上述内容
  - `keyword.index` 按 keyword 升序储存上述内容
  - `users.dat` 储存用户信息
  - `finance.dat` 储存收支情况
  - `log.dat` 储存操作日志

## 类的接口

### 指令解析

用 `CommandParser` 类来解析指令。

`run` 函数表示运行书店管理系统，即循环读入指令并解析，然后调用相应的函数。

`run` 函数只匹配指令的第一个单词（假设为 `abc`），然后调用相应的函数 `ParseAbc()`。`ParseAbc()` 只会判断指令在格式上是否合法，若非法则抛出 `SyntexError` 异常，合法则调用相对应的类的成员函数。

```cpp
class CommandParser {
 public:
  void run();  // 循环读入指令并解析，直到遇到 quit 或 exit
  // 构造 CommandParser，将其与所给的 AccountManager 和 BookManager 关联起来
  CommandParser(AccountManager &account_manager_, BookManager &book_manager_):
    account_manager(account_manager_), book_manager(book_manager_) {}

 private:
  // 储存与该类关联的 AccountManager 和 BookManager
  AccountManager &account_manager;
  BookManager &book_manager;

  // 根据指令的第一个单词查找对应的函数，供 run 函数使用
  std::unordered_map<std::string, std::function<int(const char*)>> mapFunction;

  // 以下函数供 run 函数调用，返回 0 表示指令合法，返回其他数字表示指令非法
  void ParseSu(const char* cmd);  // 解析 su [User-ID] ([Password])?
  void ParseLogout(const char* cmd);  // 解析 logout
  void ParseRegister(const char* cmd);  // 解析 register [User-ID] [Password] [User-Name]
  void ParsePasswd(const char* cmd);  // 解析 passwd [User-ID] ([Old-Password])? [New-Password]
  void ParseUseradd(const char* cmd);  // 解析 useradd [User-ID] [Password] [Priority] [User-Name]
  void ParseDelete(const char* cmd);  // 解析 delete [User-ID]

  void ParseShow(const char* cmd);  // 解析 show，调用 showBook 或 showFinance

  void ParseShowBook(const char* cmd);  // 解析 show (-ISBN=[ISBN] | -name="[Book-Name]" | -author="[Author]" | -keyword="[Keyword]")?
  void ParseBuy(const char* cmd);  // 解析 buy [ISBN] [Quantity]
  void ParseSelect(const char* cmd);  // 解析 select [ISBN]
  void ParseModify(const char* cmd);  // 解析 modify (-ISBN=[ISBN] | -name="[Book-Name]" | -author="[Author]" | -keyword="[Keyword]" | -price=[Price])+
  void ParseImport(const char* cmd);  // 解析 import [Quantity] [Total-Cost]

  void ParseReport(const char* cmd);  // 解析 report myself 或 report finance 或 report employee
  void ParseShowFinance(const char* cmd);  // 解析 show finance ([Time])?
  void ParseLog(const char* cmd);  // 解析 log
};
```

### 图书管理

图书类

```cpp
class Book {
 public:
  char ISBN[21];
  char name[61], author[61];
  char keyword[61];
  int quantity, price;
};
```

图书管理类

```cpp
class BookManager {
 private:
  Book selected_book;
  Logger& logger;
 public:
  BookManager(Logger& logger_);  // 与所给的 Logger 关联
  enum ParaType {ISBN, NAME, AUTHOR, KEYWORD};  // 参数类型
  void ShowBook(ParaType para_type, const char* arg);  // 检索图书
  void BuyBook(const char* ISBN, int quantity);  // 购买图书
  void SelectBook(const char* ISBN);  // 以当前账户选中指定图书
  void ModifyBook(ParaType para_type, const char* arg);  // 更新选中图书的信息
  void ModifyBook(ParaType para_type, int price);  // 更新选中图书的信息
  void ImportBook(int quantity, double total_cost);  // 指定交易总额购入指定数量的选中图书
};
```

### 日志系统

```cpp
class Logger {
  AccountManager& account_manager;
 public:
  Logger(AccountManager& account_manager_);  // 与所给的 AccountManager 关联
  void ShowFinance(int time = -1);  // 输出财务记录
  void ReportMyself();  // 输出员工自己的操作记录
  void ReportFinance();  // 生成财务记录报告 
  void ReportWork();  // 生成全体员工工作情况报告
  void ReportEmployee();  // 生成员工工作情况表，记录其操作
  void ShowLog();  // 生成日志，包括谁干了什么，以及财务上每一笔交易
};
```

### 基础文件读写

类似于 Memory River 的包含空间回收的文件读写类。

```cpp
template <class ValueType, std::size_t info_len>
class BasicFileIO {
 public:
  BasicFileIO(const string &file_name) : file_name(file_name) {}
  template <typename T>
  void read_info(T *info, std::size_t size, int index);  // 从信息区的指定位置读取信息
  template <typename T>
  void write_info(T &info, std::size_t size, int index);  // 将信息写入信息区指定位置

  int write(ValueType &t);  // 新建并写入对象，返回 index
  void update(ValueType &value, const int index);  // 用 value 的值更新位置 index 对应的对象
  void read(ValueType &value, const int index);  // 读出 index 对应的对象并赋给 value
  void remove(int index);  // 删除 index 对应的对象
};
```

### 块状链表

```cpp
template <typename KeyType, typename ValueType>
class UnrolledLinkedList {
 public:
  // 迭代器类型
  struct iterator {
    iterator &operator++();  // 自增运算符
    const std::pair<KeyType, ValueType> &operator*() const;  // 解引用运算符
    bool operator!=(const iterator &other) const;
  };

  UnrolledLinkedList(const char *filename);
  iterator begin() const;  // 返回第一个元素的迭代器
  iterator end() const;
  void add(const KeyType &key, const ValueType &value);  // 添加键值对
  void remove(const KeyType &index, const ValueType &value);  // 删除键值对
  iterator find(const KeyType &index) const;  // 查找 key，如果找不到，返回 end()
};
```

### 异常处理

#### 基础异常类

作为其他异常类的基类

```cpp
class BasicException : public std::exception {
  const char* message;
  BasicException(const char* message_);
  const char* what() { return message; }
};
```

#### 指令解析异常

```cpp
// 格式错误
class SyntaxError : public BasicException {};
```

#### 用户管理异常

```cpp
// 密码错误
class IncorrectPassword : public BasicException {};
// 用户名已存在
class DuplicateUsername : public BasicException {};
// 用户名不存在
class UserNotExist : public BasicException {};
// 权限错误
class PermissionError : public BasicException {};
```

#### 图书管理异常

```cpp
// 没有匹配的图书
class NoBookMatched : public BasicException {};
// 未选择图书
class NoBookSelected : public BasicException {};
```

#### 日志异常

```cpp
// 超过历史交易总笔数
class EntryNumExceeded : public BasicException {};
```