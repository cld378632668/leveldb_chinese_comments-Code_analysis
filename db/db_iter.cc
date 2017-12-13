// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/db_iter.h"

#include "db/filename.h"
#include "db/db_impl.h"
#include "db/dbformat.h"
#include "leveldb/env.h"
#include "leveldb/iterator.h"
#include "port/port.h"
#include "util/logging.h"
#include "util/mutexlock.h"
#include "util/random.h"

namespace leveldb {

#if 0
static void DumpInternalIter(Iterator* iter) {
  for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
    ParsedInternalKey k;
    if (!ParseInternalKey(iter->key(), &k)) {
      fprintf(stderr, "Corrupt '%s'\n", EscapeString(iter->key()).c_str());
    } else {
      fprintf(stderr, "@ '%s'\n", k.DebugString().c_str());
    }
  }
}
#endif

namespace {

//Leveldb数据库的MemTable和sstable文件的存储格式都是InternalKey(userkey, seq, type) = > uservalue。
//DBIter把同一个userkey在DB中的多条记录合并为一条，综合考虑了userkey的序号、删除标记、和写覆盖等等因素。
//DBIter只会把userkey最新（seq最大的就是最新的，相同userkey的老记录（seq较小的）不会让上层看到）的一条记录展现给用户，
//另外如果这条最新的记录是删除类型，则会跳过该记录，否则，遍历时会把已删除的key列举出来。
class DBIter: public Iterator {
 public:
  // Which direction is the iterator currently moving?
  // (1) When moving forward, the internal iterator is positioned at
  //     the exact entry that yields this->key(), this->value()
  // (2) When moving backwards, the internal iterator is positioned
  //     just before all entries whose user key == this->key().
  enum Direction {
    kForward,
    kReverse
  };

  DBIter(DBImpl* db, const Comparator* cmp, Iterator* iter, SequenceNumber s,
         uint32_t seed)
      : db_(db),
        user_comparator_(cmp),
        iter_(iter),
        sequence_(s),
        direction_(kForward),
        valid_(false),
        rnd_(seed),
        bytes_counter_(RandomPeriod()) {
  }
  virtual ~DBIter() {
    delete iter_;
  }
  virtual bool Valid() const { return valid_; }
  virtual Slice key() const {
    assert(valid_);
    return (direction_ == kForward) ? ExtractUserKey(iter_->key()) : saved_key_;
  }
  virtual Slice value() const {
    assert(valid_);
    return (direction_ == kForward) ? iter_->value() : saved_value_;
  }
  virtual Status status() const {
    if (status_.ok()) {
      return iter_->status();
    } else {
      return status_;
    }
  }

  virtual void Next();
  virtual void Prev();
  virtual void Seek(const Slice& target);
  virtual void SeekToFirst();
  virtual void SeekToLast();

 private:
  void FindNextUserEntry(bool skipping, std::string* skip);
  void FindPrevUserEntry();
  bool ParseKey(ParsedInternalKey* key);

  inline void SaveKey(const Slice& k, std::string* dst) {
    dst->assign(k.data(), k.size());
  }

  inline void ClearSavedValue() {
    if (saved_value_.capacity() > 1048576) {
      std::string empty;
      swap(empty, saved_value_);
    } else {
      saved_value_.clear();
    }
  }

  // Pick next gap with average value of config::kReadBytesPeriod.
  ssize_t RandomPeriod() {
    return rnd_.Uniform(2*config::kReadBytesPeriod);
  }

  DBImpl* db_;
  const Comparator* const user_comparator_;//比较iter间userkey
  Iterator* const iter_;//是一个MergingIterator，是DBImpl::NewInternalIterator()创建的结果。
  //通过MergingIterator可以实现多个有序数据集合的归并操作。其中包含多个child iterator组成的集合。
  SequenceNumber const sequence_;//DBIter只能访问到比sequence_小的kv对
  //这就方便了老版本（快照）数据库的遍历

  Status status_;
  std::string saved_key_;     // == current key when direction_==kReverse //用于反向遍历 direction_==kReverse时才有效
  std::string saved_value_;   // == current raw value when direction_==kReverse//用于反向遍历 direction_==kReverse时才有效
  Direction direction_;
  bool valid_;

  Random rnd_;
  ssize_t bytes_counter_;

  // No copying allowed
  DBIter(const DBIter&);
  void operator=(const DBIter&);
};

inline bool DBIter::ParseKey(ParsedInternalKey* ikey) {
  Slice k = iter_->key();
  ssize_t n = k.size() + iter_->value().size();
  bytes_counter_ -= n;
  while (bytes_counter_ < 0) {
    bytes_counter_ += RandomPeriod();
    db_->RecordReadSample(k);
  }
  if (!ParseInternalKey(k, ikey)) {
    status_ = Status::Corruption("corrupted internal key in DBIter");
    return false;
  } else {
    return true;
  }
}

void DBIter::Next() {
  assert(valid_);

  if (direction_ == kReverse) {  // Switch directions?
    direction_ = kForward;
    // iter_ is pointing just before the entries for this->key(),
    // so advance into the range of entries for this->key() and then
    // use the normal skipping code below.
    if (!iter_->Valid()) {
      iter_->SeekToFirst();
    } else {
      iter_->Next();
    }
    if (!iter_->Valid()) {
      valid_ = false;
      saved_key_.clear();
      return;
    }
    // saved_key_ already contains the key to skip past.
  } else {
    // Store in saved_key_ the current key so we skip it below.
    SaveKey(ExtractUserKey(iter_->key()), &saved_key_);
  }

  FindNextUserEntry(true, &saved_key_);
}


//FindNextUserEntry和FindPrevUserEntry的功能就是循环跳过下一个 / 前一个delete的记录，直到遇到kValueType的记录。
//FindNextUserKey移动方向是kForward，DBIter在向kForward移动时，借用了saved key作为临时缓存。
//FindNextUserKey确保定位到的entry的sequence不会大于指定的sequence，并跳过被删除标记覆盖的旧记录。
//参数@skipping表明是否要跳过userkey和skip相等的记录；
//参数@skip临时存储空间，保存seek时要跳过的key；
//在进入FindNextUserEntry时，iter_刚好定位在this->key(), this->value()这条记录上。
void DBIter::FindNextUserEntry(bool skipping, std::string* skip) {//skipping等于true的时候表示要向后找到第一个和skip不相等的userkey才行
  // Loop until we hit an acceptable entry to yield
  assert(iter_->Valid());
  assert(direction_ == kForward);
  do {
    ParsedInternalKey ikey;
    if (ParseKey(&ikey) && ikey.sequence <= sequence_) {//跳过不符合sequence_要求的kv对
      switch (ikey.type) {
        case kTypeDeletion://如果遇到iter userkey的删除操作，则说明后面该userkey都是无效的，因此需要跳过
          // Arrange to skip all upcoming entries for this key since
          // they are hidden by this deletion.
          SaveKey(ikey.user_key, skip);
          skipping = true;
          break;
        case kTypeValue:
          if (skipping &&
              user_comparator_->Compare(ikey.user_key, *skip) <= 0) {//如果当前key与要跳过的key相等就继续向后
            // Entry hidden
          } else {
            valid_ = true;//如果skipping为true的话，此处iter为第一个不等于save的userkey,而且不是删除类型
            saved_key_.clear();
            return;
          }
          break;
      }
    }
    iter_->Next();
  } while (iter_->Valid());
  saved_key_.clear();
  valid_ = false;
}

void DBIter::Prev() {
  assert(valid_);

  if (direction_ == kForward) {  // Switch directions?
    // iter_ is pointing at the current entry.  Scan backwards until
    // the key changes so we can use the normal reverse scanning code.
    assert(iter_->Valid());  // Otherwise valid_ would have been false
    SaveKey(ExtractUserKey(iter_->key()), &saved_key_);
    while (true) {
      iter_->Prev();
      if (!iter_->Valid()) {
        valid_ = false;
        saved_key_.clear();
        ClearSavedValue();
        return;
      }
      if (user_comparator_->Compare(ExtractUserKey(iter_->key()),
                                    saved_key_) < 0) {
        break;
      }
    }
    direction_ = kReverse;
  }

  FindPrevUserEntry();
}

//在进入FindPrevUserEntry时，iter_刚好位于saved key对应的所有记录之前*FindPrevUserKey根据指定的sequence，依次检查前一个entry，直到遇到user key小于saved key，并且类型不是Delete的entry。
//如果entry的类型是Delete，就清空saved key和saved value，这样在依次遍历前一个entry的循环中，只要类型不是Delete，就是要找的entry。
//这就是Prev的语义。
void DBIter::FindPrevUserEntry() {
	assert(direction_ == kReverse);

	ValueType value_type = kTypeDeletion;//这个很关键，后面的循环至少执行一次Prev操作 

	if (iter_->Valid()) {
		do {
			ParsedInternalKey ikey;
			if (ParseKey(&ikey) && ikey.sequence <= sequence_) {
				if ((value_type != kTypeDeletion) &&//第一次是不会进入该分支的，因为进入循环前先把value_type = kTypeDeletion;
					user_comparator_->Compare(ikey.user_key, saved_key_) < 0) {
					// We encountered a non-deleted value in entries for previous keys,
					break;
				}
				//根据类型，如果是Deletion则清空saved key和saved value  
				//否则，把iter_的user key和value赋给saved key和saved value  
				value_type = ikey.type;
				if (value_type == kTypeDeletion) {
					saved_key_.clear();
					ClearSavedValue();
				}
				else {
					Slice raw_value = iter_->value();
					if (saved_value_.capacity() > raw_value.size() + 1048576) {
						std::string empty;
						swap(empty, saved_value_);
					}//save一直在变，直到遇到iter和save的userkey不一样，说明save是最新的
					SaveKey(ExtractUserKey(iter_->key()), &saved_key_);
					saved_value_.assign(raw_value.data(), raw_value.size());
				}
			}
			iter_->Prev();
		} while (iter_->Valid());
	}

	if (value_type == kTypeDeletion) {
		// End
		valid_ = false;
		saved_key_.clear();
		ClearSavedValue();
		direction_ = kForward;
	}
	else {
		valid_ = true;
	}
}

void DBIter::Seek(const Slice& target) {
	direction_ = kForward; // 向前seek  
						   // 清空saved value和saved key，并根据target设置saved key  
	ClearSavedValue();
	saved_key_.clear();
	AppendInternalKey( // kValueTypeForSeek(1) > kDeleteType(0)  
		&saved_key_, ParsedInternalKey(target, sequence_, kValueTypeForSeek));
	iter_->Seek(saved_key_); // iter seek到saved key  
							 //可以定位到合法的iter，还需要跳过Delete的entry  
	if (iter_->Valid()) FindNextUserEntry(false, &saved_key_); //是false 
	else valid_ = false;
}

void DBIter::SeekToFirst() {
	direction_ = kForward; // 向前seek  
						   // 清空saved value，首先iter_->SeekToFirst，然后跳过Delete的entry  
	ClearSavedValue();
	iter_->SeekToFirst();
	if (iter_->Valid()) FindNextUserEntry(false, &saved_key_ /*临时存储*/);
	else valid_ = false;
}

void DBIter::SeekToLast() { // 更简单  
	direction_ = kReverse;
	ClearSavedValue();
	iter_->SeekToLast();
	FindPrevUserEntry();
}

}  // anonymous namespace

Iterator* NewDBIterator(
    DBImpl* db,
    const Comparator* user_key_comparator,
    Iterator* internal_iter,
    SequenceNumber sequence,
    uint32_t seed) {
  return new DBIter(db, user_key_comparator, internal_iter, sequence, seed);
}

}  // namespace leveldb
