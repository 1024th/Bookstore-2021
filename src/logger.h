#ifndef BOOKSTORE_SRC_LOGGER_H_
#define BOOKSTORE_SRC_LOGGER_H_

#include <iostream>

#include "block_storage.h"
#include "exceptions.h"

class Transaction {
 public:
  double amount;  // 正表示收入（buy），负表示支出（import）
  std::streamoff book_offset;
  int quantity;
  Transaction() = default;
  Transaction(double amount, std::streamoff book_offset, int quantity)
      : amount(amount), book_offset(book_offset), quantity(quantity) {}
#ifdef MyDebug
  friend std::ostream &operator<<(std::ostream &os, const Transaction &x) {
    return os << "Transcation amount: " << x.amount << " ,quantity: " << x.quantity
              << ", book_offset: " << x.book_offset;
  }
#endif  // MyDebug
};

class Bookstore;
class Logger {
  BlockStorage<Transaction> record;

 public:
  Bookstore *bookstore;
  Logger();
  void AddTransaction(const Transaction &transaction);
  void ShowFinance(int time = -1);  // 输出财务记录
  void ReportMyself();  // 输出员工自己的操作记录
  void ReportFinance();  // 生成财务记录报告
  void ReportWork();  // 生成全体员工工作情况报告
  void ReportEmployee();  // 生成员工工作情况表，记录其操作
  void ShowLog();  // 生成日志，包括谁干了什么，以及财务上每一笔交易
  void Log(const std::string &command);  // 写入日志文件
};

#endif