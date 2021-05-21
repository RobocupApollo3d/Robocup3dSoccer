# coding: utf-8
"""
@author: Mrli
@date: 2020-11-12
将[参数的txt]转成指定规格的文件
"""
import os

param = []
value = []
namedparams = ''

directory = 'params'
inputFile = input("请输入params文件夹下txt文件名, 回车默认(defaultParams.txt)")
outputFile = input("请输入cc文件名, 回车默认(d)")
# number = input("生成namedParams_t?, 回车默认t0")		# 或者可以正则提取

if not outputFile:
    outputFile = "t_default.cc"
if not inputFile:
    inputFile = "defaultParams.txt"
#if not number:
#    number = "0"

wfile = open(outputFile, "w")  # outputFile file

with open(os.path.join(directory,inputFile), "r") as f:  # input file
    zhushi = False
    for each in f:
        result = each.strip()
        if len(result) == 0:
            continue
        if ( result.startswith("/*") ):
            zhushi=True
            continue
        if ( result.endswith("*/") ):
            zhushi=False
            continue
        if ( result.startswith("//") ):
            continue
        if zhushi == True:
            continue
            
        if result[0] == '#' :
            continue
        param.append(result.split('\t')[0])
        value.append(result.split('\t')[-1])
        
length_param = len(param)
length_value = len(value)
if length_param == length_value:
    for i in range(length_param):
        # namedparams = 'namedParams_{}["'.format(number) + param[i] + '"]' + ' = "' + value[i] + '";\n'
        namedparams = '{}\t{}\n'.format(param[i], value[i])
        # print(namedparams)
        wfile.write(namedparams)
wfile.close()
        

