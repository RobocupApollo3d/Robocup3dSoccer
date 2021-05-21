#!usr/bin/python
import os
import re

'''
将当前路径下所有文件夹里的start.sh的上人个数改成1个
'''

path = os.path.dirname(__file__)
folderList = os.listdir(path)

print("当前路径",path)
print("路径下文件/文件夹:",folderList)

for folder in folderList:
	filenpath = os.path.join(path,folder)
	if os.path.isdir(filenpath):
		filename = os.path.join(filenpath,'start.sh')
		if os.path.exists(filename):
			with open(filename,'r+') as f:
				name = filename.split('/')[-2]
				content = f.read()
				# print(type(content))
				if re.findall('NUM_PLAYERS=(\d+|\$\d+)',content):
					content = re.sub( 'NUM_PLAYERS=(\d+|\$\d+)','NUM_PLAYERS=11',content,)
					#for ((i=11;i<=$NUM_PLAYERS;i++)); do
					content = re.sub('for \(\(i=(\d+);i<=\$NUM_PLAYERS;i\+\+\)\); do','for ((i=1;i<=$NUM_PLAYERS;i++)); do',content)
					f.seek(0)
					f.truncate()
					f.write(content)
					# _ , name = os.path.split(filename)
					
					# print(name)
					print('{}/start.sh 修改成功'.format(name))
				else:
					print('{}/start.sh 不是UT框架'.format(name))
				# print(content)
	else:
		print( filenpath + "不是二进制文件夹" )
