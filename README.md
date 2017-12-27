# Extra Featurs more than the origin levedb project
1、

statistic script for database LOG （）next step

# Notice

Log Dir

source_code_analysis Dir 


# Todo List



# Reading List
db
include
table
  blockbuilder
util

cmake_minimum_required(VERSION 3.8)
project(leveldb_v1_3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        db/builder.cc
        db/builder.h
        db/c.cc
        db/c_test.c
        db/db_bench.cc
        db/db_impl.cc
        db/db_impl.h
        db/db_iter.cc 1
        db/db_iter.h  1
        db/db_test.cc
        db/dbformat.cc
        db/dbformat.h
        db/dumpfile.cc
        db/filename.cc
        db/filename.h
        db/filename_test.cc
        db/leveldbutil.cc
        db/log_format.h
        db/log_reader.cc
        db/log_reader.h
        db/log_test.cc
        db/log_writer.cc
        db/log_writer.h
        db/memtable.cc
        db/memtable.h
        db/repair.cc
        db/skiplist.h
        db/snapshot.h
        db/table_cache.cc
        db/table_cache.h
        db/version_edit.cc
        db/version_edit.h
        db/version_set.cc
        db/version_set.h
        db/version_set_test.cc
        db/write_batch.cc
        db/write_batch_internal.h# Reading List
db
include
table
  blockbuilder
util
cmake_minimum_required(VERSION 3.8)
project(leveldb_v1_3)

set(CMAKE_CXX_STANDARD 11)

 ```
set(SOURCE_FILES
        db/builder.cc
        db/builder.h
        db/c.cc
        db/c_test.c
        db/db_bench.cc
        db/db_impl.cc
        db/db_impl.h
        db/db_iter.cc
        db/db_iter.h
        db/db_test.cc
        db/dbformat.cc
        db/dbformat.h
        db/dumpfile.cc
        db/filename.cc
        db/filename.h
        db/filename_test.cc
        db/leveldbutil.cc
        db/log_format.h
        db/log_reader.cc
        db/log_reader.h
        db/log_test.cc
        db/log_writer.cc
        db/log_writer.h
        db/memtable.cc
        db/memtable.h
        db/repair.cc
        db/skiplist.h
        db/snapshot.h
        db/table_cache.cc
        db/table_cache.h
        db/version_edit.cc
        db/version_edit.h
        db/version_set.cc
        db/version_set.h
        db/version_set_test.cc
        db/write_batch.cc
        db/write_batch_internal.h
        helpers/memenv/memenv.cc
        helpers/memenv/memenv.h
        include/leveldb/c.h
        include/leveldb/cache.h
        include/leveldb/comparator.h
        include/leveldb/db.h
        include/leveldb/dumpfile.h
        include/leveldb/env.h
        include/leveldb/export.h
        include/leveldb/filter_policy.h
        include/leveldb/iterator.h
        include/leveldb/options.h
        include/leveldb/slice.h
        include/leveldb/status.h
        include/leveldb/table.h
        include/leveldb/table_builder.h
        include/leveldb/write_batch.h
        port/win/stdint.h
        port/atomic_pointer.h
        port/port.h
        port/port_example.h
        port/port_posix.cc
        port/port_posix.h
        port/thread_annotations.h
        table/block.cc
        table/block.h
        table/block_builder.cc
        table/block_builder.h
        table/filter_block.cc
        table/filter_block.h
        table/filter_block_test.cc
        table/format.cc
        table/format.h
        table/iterator.cc
        table/iterator_wrapper.h
        table/merger.cc
        table/merger.h
       1 table/table.cc
       1 table/table_builder.cc
        table/table_test.cc
        table/two_level_iterator.cc
        table/two_level_iterator.h
        util/arena.cc
        util/arena.h
        util/arena_test.cc
        util/bloom.cc
        util/bloom_test.cc
        util/cache.cc
        util/cache_test.cc
        util/coding.cc
        util/coding.h
        util/coding_test.cc
        util/comparator.cc
        util/crc32c.cc
        util/crc32c.h
        util/crc32c_test.cc
        util/env.cc
        util/env_posix.cc
        util/env_posix_test.cc
        util/env_posix_test_helper.h
        util/env_test.cc
        util/filter_policy.cc
        util/hash.cc
        util/hash.h
        util/hash_test.cc
        util/histogram.cc
        util/histogram.h
        util/logging.cc
        util/logging.h
        util/mutexlock.h
        util/options.cc
        util/posix_logger.h
        util/random.h
        util/status.cc
        util/testharness.cc
        util/testharness.h
        util/testutil.cc
        util/testutil.h)

add_executable(leveldb_v1_3 ${SOURCE_FILES})

```
        helpers/memenv/memenv.cc
        helpers/memenv/memenv.h
        include/leveldb/c.h
        include/leveldb/cache.h
        include/leveldb/comparator.h
        include/leveldb/db.h
        include/leveldb/dumpfile.h
        include/leveldb/env.h
        include/leveldb/export.h
        include/leveldb/filter_policy.h
        include/leveldb/iterator.h
        include/leveldb/options.h
        include/leveldb/slice.h
        include/leveldb/status.h
        include/leveldb/table.h
        include/leveldb/table_builder.h
        include/leveldb/write_batch.h
        port/win/stdint.h
        port/atomic_pointer.h
        port/port.h
        port/port_example.h
        port/port_posix.cc
        port/port_posix.h
        port/thread_annotations.h
        table/block.cc
        table/block.h
        table/block_builder.cc
        table/block_builder.h
        table/filter_block.cc
        table/filter_block.h
        table/filter_block_test.cc
        table/format.cc
        table/format.h
        table/iterator.cc
        table/iterator_wrapper.h
        table/merger.cc
        table/merger.h
        table/table.cc
        table/table_builder.cc
        table/table_test.cc
        table/two_level_iterator.cc
        table/two_level_iterator.h
        util/arena.cc
        util/arena.h
        util/arena_test.cc
        util/bloom.cc
        util/bloom_test.cc
        util/cache.cc
        util/cache_test.cc
        util/coding.cc
        util/coding.h
        util/coding_test.cc
        util/comparator.cc
        util/crc32c.cc
        util/crc32c.h
        util/crc32c_test.cc
        util/env.cc
        util/env_posix.cc
        util/env_posix_test.cc
        util/env_posix_test_helper.h
        util/env_test.cc
        util/filter_policy.cc
        util/hash.cc
        util/hash.h
        util/hash_test.cc
        util/histogram.cc
        util/histogram.h
        util/logging.cc
        util/logging.h
        util/mutexlock.h
        util/options.cc
        util/posix_logger.h
        util/random.h
        util/status.cc
        util/testharness.cc
        util/testharness.h
        util/testutil.cc
        util/testutil.h)

add_executable(leveldb_v1_3 ${SOURCE_FILES})
=======

