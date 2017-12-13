// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// BlockBuilder generates blocks where keys are prefix-compressed:
//
// When we store a key, we drop the prefix shared with the previous
// string.  This helps reduce the space requirement significantly.
// Furthermore, once every K keys, we do not apply the prefix
// compression and store the entire key.  We call this a "restart
// point".  The tail end of the block stores the offsets of all of the
// restart points, and can be used to do a binary search when looking
// for a particular key.  Values are stored as-is (without compression)
// immediately following the corresponding key.
//
// An entry for a particular key-value pair has the form:
//     shared_bytes: varint32
//     unshared_bytes: varint32
//     value_length: varint32
//     key_delta: char[unshared_bytes]
//     value: char[value_length]
// shared_bytes == 0 for restart points.
//
// The trailer of the block has the form:
//     restarts: uint32[num_restarts]
//     num_restarts: uint32
// restarts[i] contains the offset within the block of the ith restart point.

#include "table/block_builder.h"

#include <algorithm>
#include <assert.h>
#include "leveldb/comparator.h"
#include "leveldb/table_builder.h"
#include "util/coding.h"

namespace leveldb {

BlockBuilder::BlockBuilder(const Options* options)
    : options_(options),
      restarts_(),
      counter_(0),
      finished_(false) {
  assert(options->block_restart_interval >= 1);
  restarts_.push_back(0);       // First restart point is at offset 0
}

void BlockBuilder::Reset() {
  buffer_.clear();
  restarts_.clear();
  restarts_.push_back(0);       // First restart point is at offset 0
  counter_ = 0;
  finished_ = false;
  last_key_.clear();
}

//CurrentSizeEstimate则是简单的计算当前块需要的存储大小 = 已插入的KV对的大小 + 重启点个数 * 4 + 1 * 4（重启点计数器）。
//从此处可以看出已插入的KV对放在buffer_中
size_t BlockBuilder::CurrentSizeEstimate() const {
  return (buffer_.size() +                        // Raw data buffer
          restarts_.size() * sizeof(uint32_t) +   // Restart array
          sizeof(uint32_t));                      // Restart array length
}

Slice BlockBuilder::Finish() {
  //Finish的逻辑十分简单就是简单的将restart点信息和restart点个数分别以PutFixed32的格式写入数据最后；
  // Append restart array   
  for (size_t i = 0; i < restarts_.size(); i++) {
    PutFixed32(&buffer_, restarts_[i]);
  }
  //restart点个数分别以PutFixed32的格式写入Block最后
  PutFixed32(&buffer_, restarts_.size());
  finished_ = true;
  return Slice(buffer_);
}


//调用Add函数向当前Block中新加入一个k/v对{key, value}
//猜想：对于一个Block的构建调用多次Add后调用一次Finish
void BlockBuilder::Add(const Slice& key, const Slice& value) {
 //S1 保证新加入的key > 已加入的任何一个key,只需上次插入的key比当前key小
  Slice last_key_piece(last_key_);
  assert(!finished_);
  assert(counter_ <= options_->block_restart_interval);
  assert(buffer_.empty() // No values yet?
         || options_->comparator->Compare(key, last_key_piece) > 0);

  //S2  如果计数器counter < opions->block_restart_interval，则使用前缀算法压缩key，否则就把key作为一个重启点，无压缩存储；
  size_t shared = 0;//shared部分长度
  if (counter_ < options_->block_restart_interval) {
    // See how much sharing to do with previous string 计算key与last_key_的公共前缀的长度，存入shared
    const size_t min_length = std::min(last_key_piece.size(), key.size());
    while ((shared < min_length) && (last_key_piece[shared] == key[shared])) {
      shared++;
    }
  } else {  // 否则，完整存储key，并记录restart点
    // Restart compression
    restarts_.push_back(buffer_.size());
    counter_ = 0;
  }
  const size_t non_shared = key.size() - shared; //non_shared 部分长度

  // Add "<shared key 长度><non_shared key 长度><value_size>" to buffer_
  PutVarint32(&buffer_, shared);
  PutVarint32(&buffer_, non_shared);
  PutVarint32(&buffer_, value.size());

  // Add string delta（非共享数据），value to buffer_
  buffer_.append(key.data() + shared, non_shared);//key.data() + shared怎么理解？？ ，第二个参数是长度
  buffer_.append(value.data(), value.size());

  // Update state   last_key_ = key及计数器counter_ 
  last_key_.resize(shared); // 用共享部分的数据长度去resizelast_key_？
  last_key_.append(key.data() + shared, non_shared);
  assert(Slice(last_key_) == key);
  counter_++;
}

}  // namespace leveldb
