// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_DB_ITER_H_
#define STORAGE_LEVELDB_DB_DB_ITER_H_

#include <stdint.h>
#include "leveldb/db.h"
#include "db/dbformat.h"

namespace leveldb {

class DBImpl;

// Return a new iterator that converts internal keys (yielded by 由什么什么得到**
// "*internal_iter") that were live at 住在，存储在** the specified "sequence" number
// into appropriate user keys.
//Return a new iterator that converts internal keys into appropriate user keys.
//The internal keys were live at the specified "sequence" number
//The analysis is at my blog:http://www.jianshu.com/p/94774e9fb58f
extern Iterator* NewDBIterator(
    DBImpl* db,
    const Comparator* user_key_comparator,
    Iterator* internal_iter,
    SequenceNumber sequence,
    uint32_t seed);

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_DB_ITER_H_
