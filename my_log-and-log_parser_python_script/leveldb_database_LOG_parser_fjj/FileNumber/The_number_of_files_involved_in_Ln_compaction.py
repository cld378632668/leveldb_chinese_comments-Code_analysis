#coding=utf-8
#/usr/bin/python
#计算每层compaction参与的文件数，并作图。显示部分


import re
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator,FormatStrFormatter
log_file_path = r'..\LOG_of_leveldb_example'
# log_file_path = r'G:\[important]MyCode\ycsb-leveldb-leveldbjni-rocksdb\My_Documents_Logs_Graph\LOG-leveldb-fullinsert-20G-'


with open(log_file_path) as log_file:
    text = log_file.read()
    level_str_list = re.findall(r'Compacted (\d*)@(\d) \+ (\d*)@\d',text)  #case:[("1","2","3"),(),()]	#从“Compacted 12@1 + 5@2” 匹配得到结果：（"12"，"1","5"）


levelmax = 0
for i in range(len(level_str_list)):
	level = int(level_str_list[i][1]) #记录compaction层数
	if level >levelmax:
		levelmax = level

num_of_files_in_nth_ln_compaction = [[] for i in range(levelmax + 1)]
x = [[] for i in range(levelmax+1)]
for i in range(len(level_str_list)):
	level = int(level_str_list[i][1])
	#print i,level,level_str_list[i][2],level_str_list[i][0]
	num_of_files_in_nth_ln_compaction[level].append(int(level_str_list[i][2]) + int(level_str_list[i][0]))
	x[level].append(i)  #构造x坐标
print(levelmax)
print(num_of_files_in_nth_ln_compaction)
print(level_str_list)


#设置图片x轴显示的范围
xmax = len(level_str_list)-1
xmin = xmax/9*7


#主刻度和次刻度 一个作为坐标轴的精度划分 一个作为网格线的划分
xmajorLocator = MultipleLocator(3000)
#将x轴主刻度标签设置为50的倍数
'''
ymajorFormatter = FormatStrFormatter('%1.1f')
#设置y轴主刻度标签文本的格式，浮点数保留一位小数
ymajorLocator = MultipleLocator(0.5)
'''
#y轴次刻度作为网格标记的刻度
yminorLocator = MultipleLocator(5)

color = ['b','k','r']
line = [':','--','-','-.']

plt.figure(figsize=(80,15))
ax = plt.subplot(1,1,1)

# ax.set_title('The all level RWA')
ax.set_title('The number of files involved in Ln compaction ')
ax.xaxis.set_major_locator(xmajorLocator)
#ax.yaxis.set_major_formatter(ymajorFormatter)
#ax.yaxis.set_major_locator(ymajorLocator)
ax.yaxis.set_minor_locator(yminorLocator)
ax.yaxis.grid(True,which = 'minor')
plt.xlabel('The nth compaction.')
plt.ylabel('The number of files')


for i in range (0,4):
	#plt.scatter(x,level_int_list,s=1)
	plt.plot(x[i], num_of_files_in_nth_ln_compaction[i], label ='level %d' % i, color = color[(i - 1) % 3], linestyle = line[(i - 1) % 4])
	#linestyle = line[(i-1)%4]
	#plt.plot(level_str_list,'.','markersize',4)
	plt.legend()  #add this， the label names will be shown
plt.xlim(0,xmax)
#plt.savefig('The_number_of_files_involved_in_Ln_compaction.pdf', bbox_inches=0)#bbox_inches将指定空白区剪裁掉
plt.show()#show()要写到最后，否则保存的图片为空白
	


