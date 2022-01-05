#include "user_manager.h"
#include "bookstore.h"

User::User(const Char<30> &user_ID, const Char<30> &password, int priority, const Char<30> &user_name)
    : user_ID(user_ID), password(password), priority(priority), user_name(user_name) {}

bool operator<(const User &a, const User &b) {
  return a.user_ID < b.user_ID;
//  if (a.user_ID < b.user_ID) return true;
//  if (a.user_ID > b.user_ID) return false;
//  if (a.user_name < b.user_name) return true;
//  if (a.user_name > b.user_name) return false;
//  return a.priority < b.priority;
}
bool operator==(const User &a, const User &b) {
  return a.user_ID == b.user_ID;  // todo: check
}

UserManager::UserManager() : user_data("data/user.dat") {
  std::vector<User> result;
  user_data.FindAll("root", result);
  if (result.empty()) user_data.Add("root", User("root", "sjtu", 7, "root"));
  user_stack.emplace_back(User("<guest>", "", 0, "<guest>"), 0);
}

void UserManager::Login(const Char<30> &user_id, const Char<30> &password) {
  std::vector<User> result;
  user_data.FindAll(user_id, result);
  if (result.empty()) throw UserNotExist();
  if (password.empty()) {
    if (user_stack.back().first.priority <= result[0].priority) throw IncorrectPassword();
    else user_stack.emplace_back(result[0], 0);
  } else {
//    std::cout << "Login, pw=" << result[0].password << ", input pw=" << password << std::endl;
    if (result[0].password != password) throw IncorrectPassword();
    else user_stack.emplace_back(result[0], 0);
  }
}

void UserManager::Logout() {
  if (user_stack.size() == 1) throw NoLoggedInUser();
  user_stack.pop_back();
}

void UserManager::ChangePassword(const Char<30> &user_id, const Char<30> &new_password) {
  if (user_stack.back().first.priority != 7) throw PermissionError();
  std::vector<User> result;
  user_data.FindAll(user_id, result);
  if (result.empty()) throw UserNotExist();
  result[0].password = new_password;
  user_data.Update(user_id, result[0]);
}

void UserManager::ChangePassword(const Char<30> &user_id, const Char<30> &old_password, const Char<30> &new_password) {
  CheckPermission(1);
  std::vector<User> result;
  user_data.FindAll(user_id, result);
  if (result.empty()) throw UserNotExist();
  if (result[0].password != old_password) throw IncorrectPassword();
  result[0].password = new_password;
  user_data.Update(user_id, result[0]);
}

void UserManager::CreateUser(const Char<30> &user_id, const Char<30> &password,
                             const int priority, const Char<30> &user_name) {
  CheckPermission(3);
  std::vector<User> result;
  user_data.FindAll(user_id, result);
  if (!result.empty()) throw DuplicateUserID();
  if (priority >= user_stack.back().first.priority) throw PermissionError();
  user_data.Add(user_id, User(user_id, password, priority, user_name));
}

void UserManager::Register(const Char<30> &user_id, const Char<30> &password, const Char<30> &user_name) {
  std::vector<User> result;
  user_data.FindAll(user_id, result);
  if (!result.empty()) throw DuplicateUserID();
  user_data.Add(user_id, User(user_id, password, 1, user_name));
}

void UserManager::Remove(const Char<30> &user_id) {
  CheckPermission(7);
  std::vector<User> result;
  user_data.FindAll(user_id, result);
  if (result.empty()) throw UserNotExist();
  for (int i = user_stack.size() - 1; i >= 1; --i) {
    if (user_stack[i].first.user_ID == user_id) throw DeleteMyself();
  }
  user_data.Remove(user_id, result[0]);
}
void UserManager::CheckPermission(int priority) const {
  if (user_stack.back().first.priority < priority) throw PermissionError();
}
void UserManager::SelectBook(std::streamoff book_offset) {
  user_stack.back().second = book_offset;
}
std::streamoff UserManager::GetSelectedBook() const {
  return user_stack.back().second;
}
const Char<30> &UserManager::GetCurrentUserID() {
  return user_stack.back().first.user_ID;
}
const User &UserManager::GetCurrentUser() {
  return user_stack.back().first;
}
