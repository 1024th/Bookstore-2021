#ifndef BOOKSTORE_SRC_BLOCK_STORAGE_H_
#define BOOKSTORE_SRC_BLOCK_STORAGE_H_

#include <filesystem>
#include <vector>

#include "basic_file_io.h"

template <class T>
class BlockStorage : public BasicFileIO {
  std::streamoff info_size;

 public:
  explicit BlockStorage(const std::string &filename);
  std::streamoff Add(const T &element);
  void Update(const T &element, std::streamoff offset);
  void Get(T &element, std::streamoff offset);
  int GetSize();
  void GetLastN(int n, std::vector<T> &result);
  void GetAll(std::vector<T> &result);
  //  void Remove(std::streamoff offset);
  //  void GetAll(std::vector<T> &result);
};

template <class T>
BlockStorage<T>::BlockStorage(const std::string &filename) : BasicFileIO(filename) {
  info_size = sizeof(int) + sizeof(std::streamoff);
  if (!std::filesystem::exists(filename)) {
    // 文件结构：
    // std::streamoff size, free_memory_head
    // T ...
    open(std::ios_base::out | std::ios_base::binary);
    int size = 0;
    std::streamoff free_memory_head = 0;
    Write(size);
    Write(free_memory_head);
    close();
  }
}

template <class T>
std::streamoff BlockStorage<T>::Add(const T &element) {
  open();
  int size;
  Read(size, 0);
  ++size;
  Write(size, 0);
  seekp(0, std::ios_base::end);
  std::streamoff offset = tellp();
  Write(element);
  close();
  return offset;
}

template <class T>
void BlockStorage<T>::Update(const T &element, std::streamoff offset) {
  open();
  Write(element, offset);
  close();
}
template <class T>
int BlockStorage<T>::GetSize() {
  open();
  int size;
  Read(size, 0);
  close();
  return size;
}
template <class T>
void BlockStorage<T>::GetLastN(int n, std::vector<T> &result) {
  open();
  seekg(-n * sizeof(T), std::ios_base::end);
  for (int i = 0; i < n; ++i) {
    T tmp;
    Read(tmp);
    result.push_back(tmp);
  }
  close();
}
template <class T>
void BlockStorage<T>::GetAll(std::vector<T> &result) {
  open();
  int size;
  Read(size, 0);
  seekg(info_size, std::ios_base::beg);
  for (int i = 0; i < size; ++i) {
    T tmp;
    Read(tmp);
    result.push_back(tmp);
  }
  close();
}
template<class T>
void BlockStorage<T>::Get(T &element, std::streamoff offset) {
  open();
  Read(element, offset);
  close();
}


// template<class T>
// void BlockStorage<T>::Remove(std::streamoff offset) {
//   ;
// }

#endif  // BOOKSTORE_SRC_BLOCK_STORAGE_H_
