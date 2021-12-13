#ifndef BOOKSTORE_BASIC_FILE_IO_H_
#define BOOKSTORE_BASIC_FILE_IO_H_

#include <fstream>

#ifdef MyDebug
#include "debug_print.hpp"
#endif //MyDebug

class BasicFileIO {
  const std::string filename;
  std::fstream file;

 public:
  BasicFileIO(const std::string filename_);
  void open(std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary);
  void close();
  std::ostream &seekp(std::streampos pos, std::ios_base::seekdir dir);
  std::streampos tellp();

  // 从文件中读出 sizeof(T) 字节的内容放入所给变量，需要确保调用时已经打开了文件
  template <class T>
  void Read(T &content);

  // 从文件中读出 sizeof(T) 字节的内容放入所给变量，需要确保调用时已经打开了文件
  template <class T>
  void Read(T &content, std::streamoff pos);

  // 将 &T 开头的 sizeof(T) 字节的内容写入所给文件，需要确保调用时已经打开了文件
  template <class T>
  void Write(const T &content);

  // 将 &T 开头的 sizeof(T) 字节的内容写入所给文件，需要确保调用时已经打开了文件
  template <class T>
  void Write(const T &content, std::streamoff pos);
};

BasicFileIO::BasicFileIO(const std::string filename_) : filename(filename_) {}

void BasicFileIO::open(std::ios_base::openmode mode) {
  file.open(filename, mode);
}
void BasicFileIO::close() { file.close(); }
std::ostream &BasicFileIO::seekp(std::streampos pos, std::ios_base::seekdir dir) { return file.seekp(pos, dir); }
std::streampos BasicFileIO::tellp() { return file.tellp(); }

// 从文件中读出 sizeof(T) 字节的内容放入所给变量，需要确保调用时已经打开了文件
template <class T>
void BasicFileIO::Read(T &content) {
#ifdef MyDebug
  std::streamoff pos = file.tellg();
#endif  // MyDebug
  file.read(reinterpret_cast<char *>(&content), sizeof(T));
#ifdef MyDebug
  Print("Read from file pos:", pos, ", get:", content);
#endif  // MyDebug
}

// 从文件中读出 sizeof(T) 字节的内容放入所给变量，需要确保调用时已经打开了文件
template <class T>
void BasicFileIO::Read(T &content, std::streamoff pos) {
  file.seekg(pos);
  file.read(reinterpret_cast<char *>(&content), sizeof(T));
#ifdef MyDebug
  Print("Read from file pos:", pos, ", get:", content);
#endif  // MyDebug
}

// 将 &T 开头的 sizeof(T) 字节的内容写入所给文件，需要确保调用时已经打开了文件
template <class T>
void BasicFileIO::Write(const T &content) {
#ifdef MyDebug
  Print("Write to file pos:", file.tellp(), ", content:", content);
#endif  // MyDebug
  file.write(reinterpret_cast<const char *>(&content), sizeof(T));
}

// 将 &T 开头的 sizeof(T) 字节的内容写入所给文件，需要确保调用时已经打开了文件
template <class T>
void BasicFileIO::Write(const T &content, std::streamoff pos) {
#ifdef MyDebug
  Print("Write to file pos:", pos, ", content:", content);
#endif  // MyDebug
  file.seekp(pos);
  file.write(reinterpret_cast<const char *>(&content), sizeof(T));
}

#endif  // BOOKSTORE_BASIC_FILE_IO_H_