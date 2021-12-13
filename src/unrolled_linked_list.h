#ifndef BOOKSTORE_UNROLLED_LINKED_LIST_H_
#define BOOKSTORE_UNROLLED_LINKED_LIST_H_

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <vector>

#include "basic_file_io.h"

#ifdef MyDebug
#include "debug_print.hpp"
#endif  // MyDebug

const std::streamoff kBlockInfoLength = 2 * sizeof(std::streamoff) + sizeof(int);

template <typename KeyType, typename ValueType>
struct KeyValuePair {
  KeyType key;
  ValueType value;
  KeyValuePair() {}
  KeyValuePair(KeyType key_, ValueType value_) : key(key_), value(value_) {}
  KeyValuePair &operator=(const KeyValuePair &that);
  template <typename T1, typename T2>
  friend bool operator<(const KeyValuePair<T1, T2> &a, const KeyValuePair<T1, T2> &b);
  template <typename T1, typename T2>
  friend bool operator==(const KeyValuePair<T1, T2> &a, const KeyValuePair<T1, T2> &b);
  template <typename T1, typename T2>
  friend bool operator!=(const KeyValuePair<T1, T2> &a, const KeyValuePair<T1, T2> &b);
};

template <typename NodeType, const int kBlockSize>
struct Block {
  std::streamoff next, prev;
  int len;
  NodeType array[kBlockSize];
  Block() : len(0) {}
  Block(std::streamoff next_, std::streamoff prev_, int len_) : next(next_), prev(prev_), len(len_) {}
  Block &operator=(const Block &that);
#ifdef MyDebug
  template <typename T, const int size>
  friend std::ostream &operator<<(std::ostream &os, const Block<T, size> &block);
#endif  // MyDebug
};

template <typename KeyType, typename ValueType, const int kBlockSize = 320>
class UnrolledLinkedList {
  const std::string filename;
  BasicFileIO file;

 public:
  typedef KeyValuePair<KeyType, ValueType> NodeType;
  // 块内元素数量大于 kBlockSplitThreshold 时触发裂块
  const int kBlockSplitThreshold = kBlockSize - 10;
  // 裂块后当前块剩余元素数量
  const int kBlockSplitLeft = kBlockSize / 2;
  // 相邻两块元素数量之和小于 kBlockMergeThreshold 时触发并块
  const int kBlockMergeThreshold = kBlockSize / 5;

  UnrolledLinkedList(const std::string &filename_);
  void add(const KeyType &key, const ValueType &value);
  void remove(const KeyType &key, const ValueType &value);
  void find(const KeyType &key, std::vector<ValueType> &result);

  int FindBlock(const NodeType &key_value_pair, std::streamoff &block_pos, Block<NodeType, kBlockSize> &block);
  void CheckMerge(Block<NodeType, kBlockSize> &block, std::streamoff block_pos);
  void MergeBlock(Block<NodeType, kBlockSize> &block1, std::streamoff block1_pos,  //
                  Block<NodeType, kBlockSize> &block2, std::streamoff block2_pos);
  void DeleteBlock(Block<NodeType, kBlockSize> &block, std::streamoff block_pos);
  // 裂块，block 为被裂的块，block_pos 为 block 在文件中的位置
  void SplitBlock(Block<NodeType, kBlockSize> &block, std::streamoff block_pos);
  // 修改指定位置的 block 的 prev
  void SetPrev(std::streamoff block_pos, std::streamoff prev_block_pos);
  // 修改指定位置的 block 的 next
  void SetNext(std::streamoff block_pos, std::streamoff next_block_pos);
  std::streamoff GetFreeMemoryHead();
  void SetFreeMemoryHead(std::streamoff free_memory_head);
  // 写入到新块，返回新块的位置。需要确保传入的 fstream 已经打开了文件。
  std::streamoff WriteToNewBlock(const Block<NodeType, kBlockSize> &block);
};

template <typename T1, typename T2>
KeyValuePair<T1, T2> &KeyValuePair<T1, T2>::operator=(const KeyValuePair<T1, T2> &that) {
  key = that.key;
  value = that.value;
  return *this;
}

template <typename T1, typename T2>
bool operator<(const KeyValuePair<T1, T2> &a, const KeyValuePair<T1, T2> &b) {
  if (a.key < b.key) return true;
  if (a.key > b.key) return false;
  return a.value < b.value;
  // return a.key < b.key;
}
template <typename T1, typename T2>
bool operator==(const KeyValuePair<T1, T2> &a, const KeyValuePair<T1, T2> &b) {
  return a.key == b.key && a.value == b.value;
}

template <typename T1, typename T2>
bool operator!=(const KeyValuePair<T1, T2> &a, const KeyValuePair<T1, T2> &b) {
  return !(a == b);
}

template <typename T, const int size>
Block<T, size> &Block<T, size>::operator=(const Block<T, size> &that) {
  if (this == &that) return *this;
  next = that.next;
  prev = that.prev;
  len = that.len;
  for (int i = 0; i < that.len; ++i) array[i] = that.array[i];
  return *this;
}

#ifdef MyDebug
template <typename T, const int size>
std::ostream &operator<<(std::ostream &os, const Block<T, size> &block) {
  PrintTight("Block: next=", block.next, ", prev=", block.prev, ", len=", block.len);
  return os;
}
#endif  // MyDebug

template <typename T1, typename T2, const int size>
UnrolledLinkedList<T1, T2, size>::UnrolledLinkedList(const std::string &filename_) : file(filename_) {
#ifdef MyDebug
  std::filesystem::remove(filename);
  Print("book.dat deleted");
#endif  // MyDebug

  // 如果文件不存在则创建文件
  if (!std::filesystem::exists(filename)) {
#ifdef MyDebug
    Print(filename, "not exist, create it.");
#endif  // MyDebug
    // 文件结构：
    // std::streamoff head, tail, free_memory_head
    // Block ...
    // file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    file.open(std::ios::out | std::ios::binary);
    std::streamoff head = 0, tail = 0, free_memory_head = 0;
    file.Write(head);
    file.Write(tail);
    file.Write(free_memory_head);
    file.close();
  }
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::add(const T1 &key, const T2 &value) {
  KeyValuePair key_value_pair(key, value);
  file.open();
  Block<NodeType, size> block;
  std::streamoff block_pos;
  int pos = FindBlock(key_value_pair, block_pos, block);
  if (pos != -1) {
    // 往后移一位
    for (int i = block.len - 1; i >= pos; --i) block.array[i + 1] = block.array[i];
    block.array[pos] = key_value_pair;
    block.len += 1;
    // 把修改后的 block 写回文件
    file.Write(block, block_pos);
    // 判断是否需要裂块
    if (block.len > kBlockSplitThreshold) SplitBlock(block, block_pos);
  } else {
    Block<NodeType, size> new_block(0, 0, 1);
    new_block.array[0] = key_value_pair;
    std::streamoff new_block_pos = WriteToNewBlock(new_block);
    // 更新链表的头指针、尾指针
    file.seekp(0, std::ios::beg);
    file.Write(new_block_pos);  // head
    file.Write(new_block_pos);  // tail
  }
  file.close();
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::remove(const T1 &key, const T2 &value) {
  NodeType key_value_pair(key, value);
  file.open();
  Block<NodeType, size> block;
  std::streamoff block_pos;
  int pos = FindBlock(key_value_pair, block_pos, block);
#ifdef MyDebug
  Print("remove key:", key, ", value:", value, ", pos:", pos);
#endif  // MyDebug
  if (pos == -1 || block.array[pos] != key_value_pair) {
    file.close();
    return;
  }
  // 现在 pos 指向被删除的键值对
#ifdef MyDebug
  Print("remove key:", key, ", value:", value, ", pos:", pos);
#endif  // MyDebug
  for (int i = pos; i < block.len - 1; ++i) {
    block.array[i] = block.array[i + 1];
  }
  block.len -= 1;
  file.Write(block, block_pos);
  CheckMerge(block, block_pos);
  file.close();
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::find(const T1 &key, std::vector<T2> &result) {
  KeyValuePair<T1, T2> key_value_pair(key, -2147483648);  // TODO: replace it with min value of ValutType
  file.open();
  Block<NodeType, size> block;
  std::streamoff block_pos;
  int pos = FindBlock(key_value_pair, block_pos, block);
  if (pos == -1) {
    file.close();
    return;
  }
  while (true) {
    int i;
    for (i = pos; block.array[i].key == key && i < block.len; ++i) {
      result.push_back(block.array[i].value);
    }
    if (i < block.len - 1 || block.next == 0) break;
    // 查找下一个 block
    block_pos = block.next;
    file.Read(block, block_pos);
    pos = 0;
  }
  std::sort(result.begin(), result.end());
  file.close();
}

template <typename T1, typename T2, const int size>
int UnrolledLinkedList<T1, T2, size>::FindBlock(const NodeType &key_value_pair, std::streamoff &block_pos,
                                                Block<NodeType, size> &block) {
  file.Read(block_pos, 0);
  if (block_pos == 0) return -1;
  file.Read(block, block_pos);
  if (block.len == 0) return -1;

  // 遍历链表，找到第一个可能含有 key 的块
  while (block.array[block.len - 1] < key_value_pair) {
    if (block.next == 0) break;
    block_pos = block.next;
    file.Read(block, block.next);
  }
  int pos = std::lower_bound(block.array, block.array + block.len, key_value_pair) - block.array;
  return pos;
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::CheckMerge(Block<NodeType, size> &block, std::streamoff block_pos) {
  // if (block.prev) {
  //   Block prev_block;
  //   file.Read(prev_block, block.prev);
  //   if (block.len + prev_block.len < kBlockMergeThreshold) {
  //     MergeBlock(prev_block, block.prev, block, block_pos);
  //   }
  // }
  if (block.next) {
    Block<NodeType, size> next_block;
    file.Read(next_block, block.next);
    if (block.len + next_block.len < kBlockMergeThreshold) {
      MergeBlock(block, block_pos, next_block, block.next);
    }
  }
  if (block.len == 0) DeleteBlock(block, block_pos);
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::MergeBlock(Block<NodeType, size> &block1, std::streamoff block1_pos,
                                                  Block<NodeType, size> &block2, std::streamoff block2_pos) {
  for (int i = 0; i < block2.len; ++i) {
    block1.array[block1.len + i] = block2.array[i];
  }
  block1.len += block2.len;
  // block1.next = block2.next;
  // if (block1.next) SetPrev(block1.next, block1_pos);
  DeleteBlock(block2, block2_pos);
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::DeleteBlock(Block<NodeType, size> &block, std::streamoff block_pos) {
  if (block.prev)
    SetNext(block.prev, block.next);
  else
    file.Write(block.next, 0);  // head
  if (block.next)
    SetPrev(block.next, block.prev);
  else
    file.Write(block.prev, sizeof(std::streamoff));  // tail
  SetNext(block_pos, GetFreeMemoryHead());
  SetFreeMemoryHead(block_pos);
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::SplitBlock(Block<NodeType, size> &block, std::streamoff block_pos) {
  Block<NodeType, size> new_block;
  new_block.next = block.next;
  new_block.prev = block_pos;
  new_block.len = block.len - kBlockSplitLeft;
  block.len = kBlockSplitLeft;
  for (int i = 0; i < new_block.len; ++i) {
    new_block.array[i] = block.array[i + kBlockSplitLeft];
  }
  std::streamoff new_block_pos = WriteToNewBlock(new_block);
  // fs.seekp(block.next + sizeof(std::streamoff));
  // WriteToFile(new_block_pos);
  if (block.next) SetPrev(block.next, new_block_pos);  // 修改“block.next->prev”
  block.next = new_block_pos;
  file.Write(block, block_pos);  // 更新原块
#ifdef MyDebug
  Print("裂块了！被裂的块是：", block, "他的位置是：", block_pos, "新的块是：", new_block, "位置：", new_block_pos);
#endif  // MyDebug
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::SetPrev(std::streamoff block_pos, std::streamoff prev_block_pos) {
  file.Write(prev_block_pos, block_pos + sizeof(std::streamoff));
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::SetNext(std::streamoff block_pos, std::streamoff next_block_pos) {
  file.Write(next_block_pos, block_pos);
}

template <typename T1, typename T2, const int size>
std::streamoff UnrolledLinkedList<T1, T2, size>::GetFreeMemoryHead() {
  std::streamoff free_memory_head;
  file.Read(free_memory_head, 2 * sizeof(std::streamoff));
  return free_memory_head;
}

template <typename T1, typename T2, const int size>
void UnrolledLinkedList<T1, T2, size>::SetFreeMemoryHead(std::streamoff free_memory_head) {
  file.Write(free_memory_head, 2 * sizeof(std::streamoff));
}

template <typename T1, typename T2, const int size>
std::streamoff UnrolledLinkedList<T1, T2, size>::WriteToNewBlock(const Block<NodeType, size> &block) {
  std::streamoff free_memory_head = GetFreeMemoryHead(), new_block_pos;
  if (free_memory_head) {
    new_block_pos = free_memory_head;
    // 读取下一个 free_memory_head
    std::streamoff next = 0;
    file.Read(next, free_memory_head);
    // 写入块
    file.Write(block, free_memory_head);
    // 更新 free_memory_head
    file.Write(next, 2 * sizeof(std::streamoff));
  } else {
    // 在文件末尾创建新块
    file.seekp(0, std::ios_base::end);
    new_block_pos = file.tellp();
    file.Write(block);
  }
  return new_block_pos;
}

#endif  // BOOKSTORE_UNROLLED_LINKED_LIST_H_