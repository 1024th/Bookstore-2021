#ifndef BOOKSTORE_SRC_EXCEPTIONS_H_
#define BOOKSTORE_SRC_EXCEPTIONS_H_

class BasicException : public std::exception {
  const char *message;
 public:
  explicit BasicException(const char *message_ = "") : message(message_) {}
  const char *what() const noexcept override { return message; }
};

// 指令太长（超出一行 1024 字节的限制）
class CommandTooLong : public BasicException {
 public:
  explicit CommandTooLong(const char *message = "Command too long.") : BasicException(message) {}
};
// 格式错误
class SyntaxError : public BasicException {
 public:
  explicit SyntaxError(const char *message = "Syntax error.") : BasicException(message) {}
};

// ## 用户管理异常

// 密码错误
class IncorrectPassword : public BasicException {
 public:
  explicit IncorrectPassword(const char *message = "Incorrect password.") : BasicException(message) {}
};
// 没有已登录用户
class NoLoggedInUser : public BasicException {
 public:
  explicit NoLoggedInUser(const char *message = "No logged-in user.") : BasicException(message) {}
};
// 用户已存在
class DuplicateUserID : public BasicException {
 public:
  explicit DuplicateUserID(const char *message = "Duplicate User-ID.") : BasicException(message) {}
};
// 用户不存在
class UserNotExist : public BasicException {
 public:
  explicit UserNotExist(const char *message = "This user does not exist.") : BasicException(message) {}
};
// 权限错误
class PermissionError : public BasicException {
 public:
  explicit PermissionError(const char *message = "Permission denied.") : BasicException(message) {}
};
// 删除已登录的用户
class DeleteMyself : public BasicException {
 public:
  explicit DeleteMyself(const char *message = "Cannot delete logged-in user.") : BasicException(message) {}
};

// ## 图书管理异常

// 没有匹配的图书
class NoBookMatched : public BasicException {
 public:
  explicit NoBookMatched(const char *message = "No book matched.") : BasicException(message) {}
};
// 未选择图书
class NoBookSelected : public BasicException {
 public:
  explicit NoBookSelected(const char *message = "No book selected.") : BasicException(message) {}
};
// 库存不足
class StockInsufficient : public BasicException {
 public:
  explicit StockInsufficient(const char *message = "Stock is insufficient.") : BasicException(message) {}
};
// ISBN 已存在
class DuplicateISBN : public BasicException {
 public:
  explicit DuplicateISBN(const char *message = "Duplicate ISBN.") : BasicException(message) {}
};

// ## 日志异常

// 超过历史交易总笔数
class EntryNumExceeded : public BasicException {
 public:
  explicit EntryNumExceeded(const char *message = "Entry num exceeded.") : BasicException(message) {}
};

#endif //BOOKSTORE_SRC_EXCEPTIONS_H_
