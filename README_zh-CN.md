[English](./README.md) | 中文说明

# 介绍
使用C++从零实现一个具备神经网络的贪吃蛇游戏AI.

最近我开始对神经网络感兴趣，出于动手实践的目的，实现了这个小项目。
多数神经网络相关的项目都是用Python实现的，或是使用了其他现成的机器学习、人工智能相关的库，这对新手并不友好，尤其是当你好奇细节想深入时。

所以我决定用C++从零开始实现，不使用任何人工智能相关的库。

欢迎任何的issue和PR。

# Demo

# 特性
+ 支持3种模式
    + 人类玩家模式：和经典的贪吃蛇游戏一样，吃苹果则得分加速，咬到身体或撞墙则死亡。
    + 训练AI模式：使用遗传算法训练蛇的神经网络。
    + AI玩家模式：使用训练好的神经网络来玩游戏。

+ 支持参数配置
    + 支持参数配置，详见config/appConfig.json

+ 支持训练恢复
    + 训练中断后可以继续从上次训练记录中恢复。


# 使用说明

> 我通常在macOS和Linux进行开发，以下依赖安装可能需要根据你的环境进行调整。


## 安装依赖
请确保`clang++`, `cmake`, `brew`之类的常见开发工具已经安装好。

### 依赖清单
* **sdl2**: 基础的GUI
* **sdl2_ttf**: 字体显示
* **fmt**: 格式化
* **indicators**: 显示训练进度的进度条
* **glog**: 日志库
* **gflags**: CLI参数处理
* **nlohmann-json**: json文件处理

### 安装
```bash
brew install sdl2 sdl2_ttf fmt indicators glog gflags nlohmann-json
```

## 编译
```bash
cd SnakeCharlie
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . -- -j 10
```

## 运行
```bash
# AI使用训练好的神经网络玩游戏
./snake -mode 2

# 帮助
❯ ./snake -h
snake: Usage:
  snake -mode <running mode>
      running mode should be one of '0/1/2', '0' is default.

      0: human play
      1: train the AI
      2: AI play
```
## 配置说明

config/appConfig.json中的几个node：
* **ai**: AI玩家模式下的参数
* **playboard**: 配置面板的大小，如果打算从0开始训练的话，游戏面板尺寸太大会导致训练时间过长
* **snakeApp**: 人类玩家模式下的参数
* **training**: 训练模式下的参数
* **ui**: 配置界面相关的参数



# 后续
使用遗传算法训练神经网络，有效但低效。我想，更好的方式应该是使用深度强化学习，也就是用Q-Learnning来进行训练。

那么，继续用C++从零开始实现深度强化学习吗？


