#/usr/bin/python
#


##
#File_number_changes_of_different_levels_over_time__when_inserting_5_GB
##

# Parse :

# LOG_of_leveldb_example.
# 2017/12/06-16:04:24.340579 7f8b9a470700 compacted to: files[ 0 30 19 0 0 0 0 ]
# 2017/12/06-16:04:22.263034 7f8b9a470700 compacted to: files[ 9 14 16 0 0 0 0 ]
#
# To:
#
# [[0,9 ...],[30,14 ...],[],[],[],[],[]]
#

import re
import matplotlib.pyplot as plt


list = []

list_list = [[],[],[],[],[],[],[]] # num of files in L0,L1,L2..,L6

log_file_path = 'LOG_of_leveldb_example'

with open(log_file_path) as log_file:
    text = log_file.read()
    tuple_list = re.findall(r'\[ (\d*) (\d*) (\d*) (\d*) (\d*) (\d*) (\d*) \]',text)
    print(tuple_list)

max_x = len(tuple_list)
max_y = 0

for i in range(len(tuple_list)):
    tuple = tuple_list[i]
    for j in range(len(tuple)):
        list_list[j].append(int(tuple[j]))
        if int(tuple[j]) > max_y:
            max_y = int(tuple[j])

print(list_list)
#nowe，  list_list = [[0,9 ...],[30,14 ...],[],[],[],[],[]]


plt.figure(figsize=(60, 45))  # 跟已有的某句代码冲突
fig,ax = plt.subplots()

ax.set_title('The number of SSTables in each level changes over time \n when inserting 5 GB')

"""设置坐标轴的label"""
plt.xlabel(' Time(sec) ')
plt.ylabel(' SStable files number ')


#plt.axis([0,max_x,0,max_y])
print(max_x,max_y)
# for i in range(len(list_list)):

marker = ['1' , '' , '' , '' , '']
# linestyle = ['None', '--', '-', '-.', ':']
linestyle = ['None', '-', ':', '-.', '--']
linestyle = ['None', '-', '-.', '--', ':']
for i in range(4):
    plt.plot(list_list[i+1], label='L'+str(i+1), linestyle = linestyle[i+1], marker = marker[i+1]);
# marker = marker[i]
#plt.axis([0,max_x,0,max_y])

plt.legend()  #add this， the label names will be shown
plt.show()

