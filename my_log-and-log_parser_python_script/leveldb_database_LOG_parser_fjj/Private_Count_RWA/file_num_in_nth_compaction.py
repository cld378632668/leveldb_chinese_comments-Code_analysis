#coding=utf-8
#/usr/bin/python

"""file_num_in_nth_compaction"""
"""每一层的RWA独自显示在一个Picture上"""

import re
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator,FormatStrFormatter

"""模块1"""
"""输入 log_file_path，输出 com_info_l_t_s"""
log_file_path = r'../LOG_of_leveldb_example'

with open(log_file_path) as log_file:
    text = log_file.read()
    com_info_l_t_s = re.findall(r'Compacted (\d*)@(\d) \+ (\d*)@\d', text) #case:[("1","2","3"),(),()]	#从“Compacted 12@1 + 5@2” 匹配得到结果：（"12"，"1","5"）


"""模块2"""
"""输入com_info_l_t_s，输出levelmax"""
levelmax = 0
for i in range(len(com_info_l_t_s)):
	level = int(com_info_l_t_s[i][1])
	if level >levelmax:
		levelmax = level





"""模块i"""
"""计算图中要显示的主要数据"""
"""输入：com_info_l_t_s"""
"""输出：num_of_files_in_nth_ln_compaction_l_l_i 和对应的 x"""
x = [[] for i in range(levelmax+1)]	#x—axis
num_of_files_in_nth_ln_compaction_l_l_i = [[] for i in range(levelmax + 1)]
#print levelmax
#print level_int_list
#print com_info_l_t_s
for i in range(len(com_info_l_t_s)):
	level = int(com_info_l_t_s[i][1])
	#print i,level,com_info_l_t_s[i][2],com_info_l_t_s[i][0]
	num_of_files_in_nth_ln_compaction_l_l_i[level].append(int(com_info_l_t_s[i][2]) + int(com_info_l_t_s[i][0]))
	x[level].append(i)  #构造x坐标



#设置图片x轴显示的范围
xmax = len(com_info_l_t_s) - 1
xmin = 0  #Ocean
"""逐一显示图表"""
"""输入：level_int_list"""
# for i in range (1,levelmax+1):
for i in range (1,3):
	plt.figure(figsize=(20,6)) #**宽度和高度的单位是什么？
	ax = plt.subplot(1,1,1)
	ax.xaxis.set_major_locator( MultipleLocator(150))
	ax.yaxis.set_major_formatter( FormatStrFormatter('%1.1f') )
	ax.yaxis.set_major_locator( MultipleLocator(0.5) )
	ax.yaxis.set_minor_locator( MultipleLocator(2) )
	ax.yaxis.grid(True,which = 'major')
	ax.set_title('The number of files involved in nth compaction.')
	#plt.scatter(x,level_int_list,s=1)
	plt.xlabel('The nth compaction.')
	plt.ylabel('The number of files')
	plt.xlim(xmin,xmax)    
	j = len(x[i])-1
	theory_value_hline = plt.hlines(12, 0, x[i][j], colors='r')  #involved_file_numl理论值，y值为2+10，x值范围0到最后一个值x[][]，颜色为红色
	theory_value_hline.set_label("Theoretical Value ")

    #vlines(x, ymin, ymax)     hlines(y, xmin, xmax)  
	plt.plot(x[i], num_of_files_in_nth_ln_compaction_l_l_i[i], label='The number of files involved in L%d'%i)
	# plt.plot(com_info_l_t_s,'.','markersize',4)
	plt.legend()
	plt.savefig('level%d-RWA_with_theory_value.pdf'%i, bbox_inches=0)	 #bbox_inches将指定空白区剪裁掉
	plt.show()
	


