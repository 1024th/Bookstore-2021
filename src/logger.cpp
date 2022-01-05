#include "logger.h"

#include "bookstore.h"
#include "user_manager.h"
#include "book_manager.h"
#include "ansi.h"

Logger::Logger() : transaction_record("data/finance.dat"), command_record("data/command_log.dat") {}

void Logger::ShowFinance(int time) {
  bookstore->user_manager->CheckPermission(7);
  if (time == 0) {
    std::cout << '\n';
  } else if (time == -1) {
    std::vector<Transaction> result;
    transaction_record.GetAll(result);
    double income = 0, outcome = 0;
    for (auto &i: result) {
      if (i.amount > 0) income += i.amount;
      else outcome += -i.amount;
    }
    std::cout << std::fixed << std::setprecision(2) << "+ " << income << " - " << outcome << '\n';
  } else {
    int record_size = transaction_record.GetSize();
    if (record_size < time) throw EntryNumExceeded();
    std::vector<Transaction> result;
    transaction_record.GetLastN(time, result);
    double income = 0, outcome = 0;
    for (auto &i: result) {
      if (i.amount > 0) income += i.amount;
      else outcome += -i.amount;
    }
    std::cout << std::fixed << std::setprecision(2) << "+ " << income << " - " << outcome << '\n';
  }
}
void Logger::ReportMyself() {
  bookstore->user_manager->CheckPermission(3);
  const Char<30> &user_id = bookstore->user_manager->GetCurrentUserID();
  PrintCommmandLog([&user_id](const CommandLog &x) { return x.user_id == user_id; });
}
void Logger::ReportFinance() {
  bookstore->user_manager->CheckPermission(7);
  PrintTransactions([](const Transaction &) { return true; });
}
void Logger::ReportEmployee() {
  bookstore->user_manager->CheckPermission(7);
  PrintCommmandLog([](const CommandLog &x) { return x.priority == 3; });
}
void Logger::ShowLog() {
  bookstore->user_manager->CheckPermission(7);

  PrintCommmandLog([](const CommandLog &) { return true; });
  std::cout << std::endl;
  PrintTransactions([](const Transaction &) { return true; });
}
void Logger::AddTransaction(double amount, std::streamoff book_offset, int quantity) {
  const Char<30> &user_id = bookstore->user_manager->GetCurrentUserID();
  transaction_record.Add(Transaction(std::time(nullptr), amount, user_id, book_offset, quantity));
}
void Logger::Log(const Char<30> &user_id, int priority, const Char<1024> &command, bool success) {
  command_record.Add(CommandLog(std::time(nullptr), user_id, priority, command, success));
}
void Logger::PrintCommmandLog(std::function<bool(const CommandLog &)> filter) {
  std::vector<CommandLog> command_logs;
  command_record.GetAll(command_logs);
  std::cout << ANSI::BOLD << ANSI::REVERSE << " COMMAND LOG " << ANSI::RESET << std::endl;
  char time_str[100];
  bool command_log_empty = true;
  for (auto &i: command_logs) {
    if (!filter(i)) continue;
    if (command_log_empty) command_log_empty = false;
    std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&i.time));
    std::cout << ANSI::DIM << "[" << time_str << "]" << ANSI::RESET << " ";
    std::cout << ANSI::BOLD << i.user_id << ANSI::RESET << " ";
    if (i.success) {
      std::cout << ANSI::FG_GREEN << "success" << ANSI::RESET << " ";
    } else {
      std::cout << ANSI::FG_RED << "fail" << ANSI::RESET << " ";
    }
    Char<1024> command = i.command;
    char *second = command.c_str();
    for (; *second; ++second) {
      if (*second == ' ') {
        *second = '\0';
        second++;
        break;
      }
    }
    std::cout << ANSI::BOLD << command << ANSI::RESET << " ";
    std::cout << second << std::endl;
  }
  if (command_log_empty) std::cout << "< EMPTY >" << std::endl;
}
void Logger::PrintTransactions(bool (*filter)(const Transaction &)) {
  std::vector<Transaction> transactions;
  transaction_record.GetAll(transactions);
  std::cout << ANSI::BOLD << ANSI::REVERSE << " FINANCE REPORT " << ANSI::RESET << std::endl;
  if (transactions.empty()) std::cout << "< EMPTY >" << std::endl;
  char time_str[100];
  for (auto &i: transactions) {
    if (!filter(i)) continue;
    std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&i.time));
    std::cout << ANSI::DIM << "[" << time_str << "]" << ANSI::RESET << " ";
    if (i.amount > 0) {
      std::cout << ANSI::FG_GREEN << ANSI::BOLD << "+ " << i.amount << ANSI::RESET << " ";
    } else {
      std::cout << ANSI::FG_RED << ANSI::BOLD << "- " << -i.amount << ANSI::RESET << " ";
    }
    std::cout << ANSI::BOLD << i.user_id << ANSI::RESET << " ";
    std::cout << (i.amount > 0 ? "buy " : "import ");
    std::cout << ANSI::UNDERLINED << bookstore->book_manager->GetISBN(i.book_offset) << ANSI::RESET << " ";
    std::cout << "x" << ANSI::BOLD << i.quantity << ANSI::RESET << std::endl;
  }
}

