# Robocup3dSoccer
> 南京邮电大学APOLLO机器人俱乐部 Robocup 3D开源Demo代码


1. autotest：18年在WIKI文档还没找到平台如何自动开球时， 用优化机器人发起开球命令的demo代码。目前有两个代替方式，但对理解start.sh和agent构造有所帮助
   - 修改/usr/local/share/rcssserver3d/rcssserver3d.rb
   - 使用UT开源的数据统计脚本：[/stats](https://github.com/Freedomisgood/utaustinvilla3d/tree/master/stats)目录下脚本
2. avoidDing：避免被固定位置人盯人的反击策略demo
3. Decision-Tree：包含了决策树C++基础代码， 以及将其应用到robocup中机器人判断是否踢球的demoCode
4. testSpeed：用于对比agent行走优化结果的测速的代码
5. params_tools: 参数加密打包脚本

