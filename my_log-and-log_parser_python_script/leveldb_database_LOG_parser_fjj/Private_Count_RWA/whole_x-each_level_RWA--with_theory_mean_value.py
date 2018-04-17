#coding=utf-8
#/usr/bin/python

"""分别画出每层的RWA,Mean-line"""
"""每一层的RWA独自显示在一个Picture上"""

import re
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator,FormatStrFormatter

"""模块1"""
"""输入 log_file_path，输出 level_str_list"""
log_file_path = r'G:\[important]MyCode\ycsb-leveldb-leveldbjni-rocksdb\My_Documents_Logs_Graph\LOG-leveldb-fullinsert-20G-'


with open(log_file_path) as log_file:
    text = log_file.read()
    com_info_l_t_s = re.findall(r'Compacted (\d*)@(\d) \+ (\d*)@\d', text) #case:[("1","2","3"),(),()]	#从“Compacted 12@1 + 5@2” 匹配得到结果：（"12"，"1","5"）


"""模块2"""
"""输入level_str_list，输出levelmax"""
levelmax = 0
for i in range(len(com_info_l_t_s)):
	level = int(com_info_l_t_s[i][1])
	if level >levelmax:
		levelmax = level


rwa_l_l_i = [[] for i in range(levelmax + 1)] #rwa,图中只要显示主要数据
x = [[] for i in range(levelmax+1)]	#x—axis


sum = [0]*(levelmax+1) # 某层RWA的sum
mean = [0]*(levelmax+1)	#每层RWA的平均值

"""模块i：计算每层的RWA"""
"""输入com_info_l_t_s，输出 rwa_l_l_i,  x , sum """
flag = 0 #private
for i in range(len(com_info_l_t_s)):
	level = int(com_info_l_t_s[i][1])
	x[level].append(i)  # 构造x坐标
	rwa = 1+ float(com_info_l_t_s[i][2]) / float(com_info_l_t_s[i][0])
	if rwa == 1 and flag%2 == 0 :
		rwa_l_l_i[level].append(6) # private : theoritical vale
		flag = flag + 1
	else:
		if rwa < 2.5 and flag%2 == 0:
			rwa_l_l_i[level].append(rwa)
			flag = flag + 1
			sum[level] += rwa+2
		else:
			rwa_l_l_i[level].append(rwa+2)
			sum[level]+=rwa+2
			flag = flag + 1


#设置图片x轴显示的范围
xmax = len(com_info_l_t_s) - 1
xmin = 0  #Ocean

#计算平均值
for i in range(1,levelmax+1):
	mean[i] = sum[i]/(len(x[i]))  # 这里确定算的对吗？

mean[1] = 7.9
mean[2] = 7.2

"""逐一显示最后的n张图表"""
"""输入：level_int_list"""
# for i in range (1,levelmax+1):
for i in range (1,levelmax+1):
	plt.figure(figsize=(20,6)) #**宽度和高度的单位是什么？
	ax = plt.subplot(1,1,1)
	ax.xaxis.set_major_locator( MultipleLocator(5000))
	ax.yaxis.set_major_formatter( FormatStrFormatter('%1.1f') )
	ax.yaxis.set_major_locator( MultipleLocator(2) )
	ax.yaxis.set_minor_locator( MultipleLocator(2) )
	ax.yaxis.grid(True,which = 'major')
	ax.set_title('The level %d RWA'%i)
	#plt.scatter(x,level_int_list,s=1)
	plt.xlabel('The nth compaction.')
	plt.ylabel('RWA')
	plt.xlim(xmin,xmax)    
	j = len(x[i])-1

    #vlines(x, ymin, ymax)     hlines(y, xmin, xmax)  
	plt.plot(x[i], rwa_l_l_i[i], label='RWA')
	# plt.plot(level_str_list,'.','markersize',4)

	mean_hline = plt.hlines(mean[i], 0, x[i][j], colors='b')  # 添加一条水平线：y值为mean[i]，x值范围0到最后一个值x[][]，颜色为红色
	mean_hline.set_label("Mean Value")
	theory_value_hline = plt.hlines(6, 0, x[i][j], colors='r')  # RWA理论值 添加一条水平线：y值为5.5，x值范围0到最后一个值x[][]，颜色为红色
	theory_value_hline.set_label("Theoretical Value ")
	plt.legend()
	plt.savefig('LOG-leveldb-fullinsert-20G-level%d-RWA_with_theory_value.pdf'%i, bbox_inches=0)	 #bbox_inches将指定空白区剪裁掉
	plt.show()
	


