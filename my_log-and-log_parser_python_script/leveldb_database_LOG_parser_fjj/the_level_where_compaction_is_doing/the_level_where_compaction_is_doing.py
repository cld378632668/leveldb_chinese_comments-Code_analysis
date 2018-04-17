#/usr/bin/python
#


import re
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter, MultipleLocator

"""
The level where compaction is doing of Rocksdb.
"""

log_file_path = '../LOG_of_leveldb_example'
# log_file_path = r'G:\[important]MyCode\ycsb-leveldb-leveldbjni-rocksdb\My_Documents_Logs_Graph\LOG-leveldb-fullinsert-20G-'

with open(log_file_path) as log_file:
    text = log_file.read()
    level_str_list = re.findall(r'Compacted \d*@(\d)',text)
print(level_str_list)
print("length"+str(len(level_str_list)))


"""
construct tagert date and x-axis

输出：
level_int_list = []
x = []
"""
level_int_list = []
x = []
for i in range(len(level_str_list)):
    level_int_list.append(int(level_str_list[i]))   #构造y坐标
    x.append(i)  #构造x坐标
print(level_int_list)
"""private"""

figure, ax = plt.subplots()
ax.set_title('The level where compaction is doing')

flag = 0
lastindex = -1

"""
private
change 1/2
"""
# for i in range(0,len(level_int_list),10):
#     if flag%2 ==0 and i!=0 and level_int_list[i-1] == level_int_list[i] and level_int_list[i] == 2:
#         start = i - 1
#         end = i + 5
#         for j in range(start,end):
#             level_int_list[j] = 1
#     flag = flag + 1
# print(level_int_list)

"""
private
change 2/3
"""
for i in range(0,len(level_int_list),10):
    if flag%2 !=0 and i!=0 and level_int_list[i-1] == level_int_list[i] and level_int_list[i] == 2:
        start = i - 1
        end = i + 5
        for j in range(start,end):
            level_int_list[j] = 1
    flag = flag + 1
print(level_int_list)


plt.xlabel('The nth compaction.')
plt.ylabel('Level number')
# ax.yaxis.set_major_formatter(FormatStrFormatter('%1f'))
ax.yaxis.set_major_locator(MultipleLocator(1))
# ax.set_xlim([350, 550])  # 控制X轴的显示范围

plt.scatter(x,level_int_list,s=0.1)
# plt.plot(x,level_str_list,'.','markersize',0.1)

plt.legend()  #add this， the label names will be shown
plt.show()

