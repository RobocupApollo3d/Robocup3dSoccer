# coding: utf-8

"""
@author: Mrli
@date: 2020-11-12
从指定规格的文件中评出需要的文件
"""
import os

dist = {}
defualt = "d"
paramsOutPath = "./paramsOutPath"


def run():
    with open(defualt, "r") as f:
        for line in f:
            param, val = line.strip().split('\t')
            dist[param] = val

    paramTypeFile = input("需要覆盖的机器人参数文件:")
    with open(paramTypeFile, "r") as f:
        for line in f:
            param, val = line.strip().split('\t')
            dist[param] = val

    with open(os.path.join(paramsOutPath, "result{}.txt".format(paramTypeFile)), 'w') as f:
        f.write("void Load_param_t{}".format(paramTypeFile) + "(){\n")
        for k, v in dist.items():
            f.write('namedParams_t{}["{}"]="{}";\n'.format(paramTypeFile, k, v))
        f.write("}\n")


if __name__ == "__main__":
    if not os.path.exists(paramsOutPath):
        os.mkdir(paramsOutPath)
    run()
