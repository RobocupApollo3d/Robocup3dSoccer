## 参数打包脚本使用说明

### paramsOut.py + formatOut.py

#### paramsOut.py

1. 将参数文件放入当前目录的`./params`文件夹中
2. 运行`python3 paramsOut.py`, 输入相应信息, 首先将default默认参数打出来, 输入`defaultParams.txt`和`d`会在当前目录下生成d文件
3. 运行`python3 paramsOut.py`, 输入相应信息, 根据机器人类型将参数打出来，e.g.1号类型: 输入`defaultParams_t1.txt`和`1`, 会在当前目录生成1文件
4. 如果需要多个类型的机器人, 则重复**3**, 生成多个文件

#### formatOut.py

1. 运行`formatOut.py`, 在提示的`需要覆盖的机器人参数文件:`中输入1, 就会生成cpp格式的语法文件在`result1.txt`中
2. 将`result1.txt`中的内容复制到项目`params.cc`的`load_params_t1`函数中即可



## skills打包

> 由于动作帧越来越多, 因此一次性全部读入内存比较困难, 因此仿照readFromFile: 采用调用多个函数的方式解决

### skills.py

1. 运行`skills.py`会将skills文件夹下的所有skl解析好, ①`writeToSKL()`函数生成`skl.cc`文件和`skl.h`，其中`skl.h`为声明的头文件, `skl.cc`代码为`void getSkillsFromLocal1(std::string& skillPart)`的函数实现; ②`write2naobehavior()`生成需要增加到naobehavior中的读取参数的函数内容
2. 输出结果在`./skillsOutput/`中