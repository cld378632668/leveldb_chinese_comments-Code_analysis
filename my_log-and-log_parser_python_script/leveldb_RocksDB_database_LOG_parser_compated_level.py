#/usr/bin/python
#


import re
import matplotlib.pyplot as plt


#log_file_path = 'LOG_of_leveldb_example'
log_file_path = r'G:\1-RocksDB\Slot实验\LOG_rocksdb_origin-ycsb_1G_50G_onlyrun_cld'

with open(log_file_path) as log_file:
    text = log_file.read()
    level_str_list = re.findall(r'Compacted \d*@(\d)',text)

level_int_list = []
print(level_str_list)



x = []
for i in range(len(level_str_list)):
    level_int_list.append(int(level_str_list[i]))   #构造y坐标
    x.append(i)  #构造x坐标

figure, ax = plt.subplots()
ax.set_title('The level where compaction is doing of Rocksdb.')


plt.xlabel('The nth compaction.')
plt.ylabel('Level number')

plt.scatter(x,level_str_list,s=1)
# plt.plot(level_str_list,'.','markersize',4)

plt.legend()  #add this， the label names will be shown
plt.show()

