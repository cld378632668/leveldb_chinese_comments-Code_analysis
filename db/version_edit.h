// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_VERSION_EDIT_H_
#define STORAGE_LEVELDB_DB_VERSION_EDIT_H_

#include <set>
#include <utility>
#include <vector>
#include "db/dbformat.h"

namespace leveldb {

class VersionSet;

struct FileMetaData {
  int refs;
  int allowed_seeks;          // Seeks allowed until compaction
  uint64_t number;
  uint64_t file_size;         // File size in bytes
  InternalKey smallest;       // Smallest internal key served by table
  InternalKey largest;        // Largest internal key served by table

  FileMetaData() : refs(0), allowed_seeks(1 << 30), file_size(0) { }
};

class VersionEdit { //VersionEdit就相当于MANIFEST文件中的一条记录。
 public:
  VersionEdit() { Clear(); }
  ~VersionEdit() { }

  void Clear();

  void SetComparatorName(const Slice& name) {
    has_comparator_ = true;
    comparator_ = name.ToString();
  }
  void SetLogNumber(uint64_t num) {
    has_log_number_ = true;
    log_number_ = num;
  }
  void SetPrevLogNumber(uint64_t num) {
    has_prev_log_number_ = true;
    prev_log_number_ = num;
  }
  void SetNextFile(uint64_t num) {
    has_next_file_number_ = true;
    next_file_number_ = num;
  }
  void SetLastSequence(SequenceNumber seq) {
    has_last_sequence_ = true;
    last_sequence_ = seq;
  }
  void SetCompactPointer(int level, const InternalKey& key) {
    compact_pointers_.push_back(std::make_pair(level, key));
  }

  // definition and implemention
  // S1、新添加的sstable的文件number、大小、smallestkey和largest逐一赋值给FileMetaData
  // S2、将level和FileMetaData信息作为一个pair添加到VersionEdit的new_files_中
  // std::vector< std::pair<int, FileMetaData> > new_files_;
  // REQUIRES: This version has not been saved (see VersionSet::SaveTo)
  // REQUIRES: "smallest" and "largest" are smallest and largest keys in file
  void AddFile(int level, uint64_t file,
               uint64_t file_size,
               const InternalKey& smallest,
               const InternalKey& largest) {
	// S1、新添加的sstable的文件number、大小、smallestkey和largest逐一赋值给FileMetaData
    FileMetaData f;
    f.number = file;
    f.file_size = file_size;
    f.smallest = smallest;
    f.largest = largest;
	// S2、将level和FileMetaData信息作为一个pair添加到VersionEdit的new_files_中
    new_files_.push_back(std::make_pair(level, f));
  }

  //完成 从指定的level删除指定number的sst  的信息预记录。 Delete the specified "file" from the specified "level".
  void DeleteFile(int level, uint64_t file) {
    deleted_files_.insert(std::make_pair(level, file));
  }

  // 将VersionEdit(this)的信息Encode到一个string中，作为一个record，便于之后写入manifest文件 
  void EncodeTo(std::string* dst) const; 
  // 从Slice中Decode出VersionEdit的信息  
  Status DecodeFrom(const Slice& src);

  std::string DebugString() const;

 private:
  friend class VersionSet;

  typedef std::set< std::pair<int, uint64_t> > DeletedFileSet;

  std::string comparator_; // key comparator名字  
  uint64_t log_number_; // 当前versionEdit所属（bin）log编号**   
  uint64_t prev_log_number_; // 前一个日志编号,为了兼容老版本，新版本已弃用，一直为0  
  uint64_t next_file_number_; // 下一个文件编号  指的是manifest文件吗？印象中manifest只会有一个
  SequenceNumber last_sequence_; // db中最大的seq（序列号），即最后一对kv事务操作的序列号
  //Manifest文件中首先存储的是coparator名、log编号、前一个log编号、下一个文件编号、上一个序列号。
  //这些都是日志、sstable文件使用到的重要信息，这些字段不一定必然存在。
  bool has_comparator_;
  bool has_log_number_;
  bool has_prev_log_number_;
  bool has_next_file_number_;
  bool has_last_sequence_;

  std::vector< std::pair<int, InternalKey> > compact_pointers_; //记录每一层下次合并的起始key
  DeletedFileSet deleted_files_; //在此次VersionEdit（delta）中删除的文件集合。在save时实际执行添加操作**
  std::vector< std::pair<int, FileMetaData> > new_files_; //在此次VersionEdit（delta）中新添加的sst集合。 
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_VERSION_EDIT_H_
