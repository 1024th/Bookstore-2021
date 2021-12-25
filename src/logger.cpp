#include "logger.h"

Logger::Logger() : record("data/finance.dat") {}

void Logger::ShowFinance(int time) {
  if (time == 0) {
    std::cout << '\n';
  } else if (time == -1) {
    std::vector<Transaction> result;
    record.GetAll(result);
    double income = 0, outcome = 0;
    for (auto &i: result) {
      if (i.amount > 0) income += i.amount;
      else outcome += -i.amount;
    }
    std::cout << "+ " << income << " - " << outcome << '\n';
  } else {
    int record_size = record.GetSize();
    if (record_size < time) throw EntryNumExceeded();
    std::vector<Transaction> result;
    record.GetLastN(time, result);
    double income = 0, outcome = 0;
    for (auto &i: result) {
      if (i.amount > 0) income += i.amount;
      else outcome += -i.amount;
    }
    std::cout << "+ " << income << " - " << outcome << '\n';
  }
}
void Logger::ReportMyself() {

}
void Logger::ReportFinance() {

}
void Logger::ReportWork() {

}
void Logger::ReportEmployee() {

}
void Logger::ShowLog() {

}
void Logger::Log(const std::string &command) {

}
void Logger::AddTransaction(const Transaction &transaction) {
  record.Add(transaction);
}

