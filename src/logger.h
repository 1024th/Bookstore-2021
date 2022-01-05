#ifndef BOOKSTORE_SRC_LOGGER_H_
#define BOOKSTORE_SRC_LOGGER_H_

#include <iostream>
#include <functional>

#include "char.h"
#include "block_storage.h"
#include "exceptions.h"

class Transaction {
 public:
  std::time_t time;
  double amount;  // 正表示收入（buy），负表示支出（import）
  Char<30> user_id;
  std::streamoff book_offset;
  int quantity;
  Transaction() = default;
  Transaction(std::time_t time, double amount, const Char<30> &user_id, std::streamoff book_offset, int quantity)
      : time(time), amount(amount), user_id(user_id), book_offset(book_offset), quantity(quantity) {}
#ifdef MyDebug
  friend std::ostream &operator<<(std::ostream &os, const Transaction &x) {
    return os << "Transcation amount: " << x.amount << " ,quantity: " << x.quantity
              << ", book_offset: " << x.book_offset;
  }
#endif  // MyDebug
};

class CommandLog {
 public:
  std::time_t time;
  Char<30> user_id;
  int priority;
  Char<1024> command;
  bool success;
  CommandLog() = default;
  CommandLog(std::time_t time, const Char<30> &user_id, int priority, const Char<1024> &command, bool success)
      : time(time), user_id(user_id), priority(priority), command(command), success(success) {}
};

class Bookstore;
class Logger {
  BlockStorage<Transaction> transaction_record;
  BlockStorage<CommandLog> command_record;

 public:
  Bookstore *bookstore;
  Logger();
  void AddTransaction(double amount, std::streamoff book_offset, int quantity);
  void ShowFinance(int time = -1);  // 输出财务记录
  void ReportMyself();  // 输出员工自己的操作记录
  void ReportFinance();  // 生成财务记录报告
  void ReportEmployee();  // 生成员工工作情况表，记录其操作
  void ShowLog();  // 生成日志，包括谁干了什么，以及财务上每一笔交易
  void Log(const Char<30> &user_id, int priority, const Char<1024> &command, bool success);  // 写入日志文件
  void PrintCommmandLog(std::function<bool(const CommandLog &)>);
  void PrintTransactions(bool (*filter)(const Transaction &));
};

#endif