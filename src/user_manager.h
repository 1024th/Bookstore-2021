#ifndef BOOKSTORE_SRC_USER_MANAGER_H_
#define BOOKSTORE_SRC_USER_MANAGER_H_

#include <vector>

#include "char.h"
#include "exceptions.h"
#include "unrolled_linked_list.h"

class UserManager;
class User {
  friend class UserManager;

 private:
  Char<30> password;

 public:
  Char<30> user_ID;
  Char<30> user_name;
  int priority;  // 权限，可以取 7 或 3 或 1（或 0，表示游客）
  friend bool operator<(const User &a, const User &b);
  friend bool operator==(const User &a, const User &b);
  User() = default;
  User(const Char<30> &user_ID, const Char<30> &password, int priority, const Char<30> &user_name);
#ifdef MyDebug
  friend std::ostream &operator<<(std::ostream &os, const User &x) { return os << "User user_id: " << x.user_ID; }
#endif  // MyDebug
};
class Bookstore;
class UserManager {
 public:
  Bookstore *bookstore;
  UserManager();
  void Login(const Char<30> &user_id, const Char<30> &password = "");  // 登录用户
  void Logout();  // 退出登录
  void ChangePassword(const Char<30> &user_id, const Char<30> &new_password);  // 修改密码
  void ChangePassword(const Char<30> &user_id, const Char<30> &old_password, const Char<30> &new_password);  // 修改密码
  void CreateUser(const Char<30> &user_id, const Char<30> &password, const int priority,
                  const Char<30> &user_name);  // 创建用户
  void Register(const Char<30> &user_id, const Char<30> &password, const Char<30> &user_name);  // 注册账户
  void Remove(const Char<30> &user_id);  // 删除账户
  void CheckPermission(int priority) const;
  void SelectBook(std::streamoff book_offset);
  std::streamoff GetSelectedBook() const;

  const Char<30> &GetCurrentUserID();
  const User &GetCurrentUser();
 private:
  std::vector<std::pair<User, std::streamoff>> user_stack;  // 用户栈，储存登录的用户和他所选的图书的 offset
  UnrolledLinkedList<Char<30>, User> user_data;
};

#endif