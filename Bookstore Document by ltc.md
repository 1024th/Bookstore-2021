# 书店管理系统开发文档

作者：林田川

代码风格：[Google 开源项目风格指南](https://zh-google-styleguide.readthedocs.io/en/latest/)

## 文件结构

- `main.cpp` 入口点
- `src/`
  - `command_parser.h` 指令解析
  - `command_parser.cpp`
  - `user_manager.h` 用户管理
  - `user_manager.cpp`
  - `book_manager.h` 图书管理
  - `book_manager.cpp`
  - `logger.h` 日志系统
  - `logger.cpp`
  - `exceptions.h` 异常处理
  - `exceptions.cpp`
  - `basic_file_io.h` 基础文件读写类
  - `basic_file_io.cpp`
  - `unrolled_linked_list.h` 块状链表
  - `unrolled_linked_list.cpp`
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
 private:
  // 储存与该类关联的 UserManager 和 BookManager
  UserManager &user_manager;
  BookManager &book_manager;
  Logger &logger;
  // 根据指令的第一个单词查找对应的函数，供 run 函数使用
  std::unordered_map<std::string, std::function<int(const char*)>> mapFunction;

 public:
  void Run();  // 循环读入指令并解析，直到遇到 quit 或 exit
  // 构造 CommandParser，将其与所给的 UserManager，BookManager 和 Logger 关联起来
  CommandParser(UserManager &user_manager_, BookManager &book_manager_, Logger &logger_);

  void ParseSu(const char *cmd);  // 解析 su [User-ID] ([Password])?
  void ParseLogout(const char *cmd);  // 解析 logout
  void ParseRegister(const char *cmd);  // 解析 register [User-ID] [Password] [User-Name]
  void ParsePasswd(const char *cmd);  // 解析 passwd [User-ID] ([Old-Password])? [New-Password]
  void ParseUseradd(const char *cmd);  // 解析 useradd [User-ID] [Password] [Priority] [User-Name]
  void ParseDelete(const char *cmd);  // 解析 delete [User-ID]

  void ParseShow(const char *cmd);  // 解析 show，调用 showBook 或 showFinance

  void ParseShowBook(const char *cmd);  // 解析 show (-ISBN=[ISBN] | -name="[Book-Name]" | -author="[Author]" | -keyword="[Keyword]")?
  void ParseBuy(const char *cmd);  // 解析 buy [ISBN] [Quantity]
  void ParseSelect(const char *cmd);  // 解析 select [ISBN]
  void ParseModify(const char *cmd);  // 解析 modify (-ISBN=[ISBN] | -name="[Book-Name]" | -author="[Author]" | -keyword="[Keyword]" | -price=[Price])+
  void ParseImport(const char *cmd);  // 解析 import [Quantity] [Total-Cost]

  void ParseReport(const char *cmd);  // 解析 report myself 或 report finance 或 report employee
  void ParseShowFinance(const char *cmd);  // 解析 show finance ([Time])?
  void ParseLog(const char *cmd);  // 解析 log
};
```

### 字符串类

包装了 `char` 数组，重载运算符以支持比较。


```cpp
template <const int size = 64>
class Char {
  char content[size + 1];  // 多开一位用来放 '\0'

 public:
  Char();
  Char(const std::string &s);
  Char(const char *cstr);
  operator std::string() const;
  std::string str() const;

  Char &operator=(const Char &that);
  friend bool operator<(const Char<size> &a, const Char<size> &b);
  friend bool operator==(const Char<size> &a, const Char<size> &b);
  friend bool operator>(const Char<size> &a, const Char<size> &b);
  friend bool operator<=(const Char<size> &a, const Char<size> &b);
  friend bool operator>=(const Char<size> &a, const Char<size> &b);
  friend bool operator!=(const Char<size> &a, const Char<size> &b);
  friend std::istream &operator>>(std::istream &is, Char<size> &s);
  friend std::ostream &operator<<(std::ostream &os, const Char<size> &s);
};
```

### 用户管理

`UserManager` 需要通过 `UnrolledLinkedList` 读写 `users.dat`。

```cpp
class UserManager;
class User {
  friend class UserManager;

 private:
  Char<30> password;

 public:
  Char<30> user_ID;
  Char<30> user_name;
  int priority;  // 权限，可以取 7 或 3 或 1
};

class UserManager {
 private:
  std::vector<std::pair<User, int>> user_stack;  // 用户栈，储存登录的用户和他所选的图书的 offset

 public:
  int Login(const char* user_id);  // 登录用户
  int Logout();  // 退出登录
  int ChangePassword(const char* user_id, const char* old_password, const char* new_password);  // 修改密码
  int CreateUser(const char* user_id, const char* password, const int priority, const char* user_name);  // 创建用户
  int Register(const char* user_id, const char* password, const char* user_name);  // 注册账户
  int Remove(const char* user_id);  // 删除账户
};
```

### 图书管理

图书类

```cpp
class Book {
 public:
  Char<20> ISBN;
  Char<60> name, author;
  Char<60> keyword;
  int quantity, price;
};
```

图书管理类

图书信息存放于 `book.dat`，无序。检索图书时需要的索引存放于 `*.index`。

```cpp
class BookManager {
 public:
  enum ParaType {ISBN, NAME, AUTHOR, KEYWORD};  // 参数类型
  void ShowBook(ParaType para_type, const char *arg);  // 检索图书
  void BuyBook(const char *ISBN, int quantity);  // 购买图书
  void SelectBook(const char *ISBN);  // 以当前账户选中指定图书
  void ModifyBook(ParaType para_type, const char *arg);  // 更新选中图书的信息
  void ModifyBook(ParaType para_type, int price);  // 更新选中图书的信息
  void ImportBook(int quantity, double total_cost);  // 指定交易总额购入指定数量的选中图书
};
```

### 日志系统

日志格式 `[time] [user-id] (success|fail) [command]`。

其中 `[time]` 格式为 `%Y-%m-%d %H:%M:%S`，例如 `2021-12-07 20:05:25`；若当前没有已登陆的用户，则 `[user-id]` 为 `<guest>`；`(success|fail)` 表示该指令是否正常执行；`[command]` 为输入的指令。

对于 `import` 和 `modify` 指令，如果当前有被选中的书，以 `[command] [ISBN] [args]` 格式输出，例如：

```plain
2021-09-01 08:30:00 root success select 978-7-115-42935-3
2021-09-01 08:30:01 root success import 978-7-115-42935-3 33 233.33
2021-09-01 08:30:05 root success modify 978-7-115-42935-3 -name="C++ Program Design" -author="why, yyu" -price=49.8
```

```cpp
class Logger {
  UserManager &user_manager;
  BookManager &book_manager;
 public:
  Logger(UserManager &user_manager_, BookManager &book_manager_);  // 与所给的 UserManager 和 BookManager 关联
  void ShowFinance(int time = -1);  // 输出财务记录
  void ReportMyself();  // 输出员工自己的操作记录
  void ReportFinance();  // 生成财务记录报告 
  void ReportWork();  // 生成全体员工工作情况报告
  void ReportEmployee();  // 生成员工工作情况表，记录其操作
  void ShowLog();  // 生成日志，包括谁干了什么，以及财务上每一笔交易
  void Log(const char *command);  // 写入日志文件
};
```

### 基础文件读写

类似于 Memory River 的包含空间回收的文件读写类。用于存储 `books.dat` 中的图书信息。也用于写入 `finance.dat` 和 `log.dat`。

```cpp
template <class ValueType, std::size_t info_len>
class BasicFileIO {
 public:
  BasicFileIO(const string &file_name) : file_name(file_name) {}
  template <typename T>
  void ReadInfo(T *info, std::size_t size, int index);  // 从信息区的指定位置读取信息
  template <typename T>
  void WriteInfo(T &info, std::size_t size, int index);  // 将信息写入信息区指定位置

  int Write(ValueType &t);  // 新建并写入对象，返回 index
  void Update(ValueType &value, const int index);  // 用 value 的值更新位置 index 对应的对象
  void Read(ValueType &value, const int index);  // 读出 index 对应的对象并赋给 value
  void Remove(int index);  // 删除 index 对应的对象
};
```

### 块状链表

有序地存放数据。用于读写 `*.index` 和 `users.dat`。

```cpp
template <typename KeyType, typename ValueType>
class UnrolledLinkedList {
 public:
  UnrolledLinkedList(const char *filename);
  void Add(const KeyType &key, const ValueType &value);  // 添加键值对
  void Remove(const KeyType &key, const ValueType &value);  // 删除键值对
  void Find(const KeyType &key, std::vector<Value>) const;  // 查找 key，如果找不到，返回 end()
};
```

### 异常处理

#### 基础异常类

作为其他异常类的基类

```cpp
class BasicException : public std::exception {
  const char *message;
  BasicException(const char *message_);
  const char *what() { return message; }
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