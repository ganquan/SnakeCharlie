English | [中文说明](./README_zh-CN.md)

# Introduce

Implementing a snake game AI with neural networks from scratch using C++.

I recently became interested in neural networks and implemented this small project for hands-on purposes.
Most neural network related projects are implemented in Python or use other off-the-shelf machine learning and AI related libraries, which is not newbie friendly, especially if you are curious about the details and want to go deeper.

So I decided to make a C++ implementation from scratch, without using any AI-related libraries.

Any issue and PR are welcome.

# Demo

# Features
+ Support 3 modes
    + **Human player mode**: Just like the classic snake game, eat an apple to gain score and move faster, bite the body or hit a wall to die.
    + **Training AI mode**: Use genetic algorithms to train the neural network.
    + **AI player mode**: Use trained neural network to play the game.

+ Support for parameter configuration
    + See config/appConfig.json for details

+ Support for training recovery
    + You can recover from the last training record after a training interruption.



# Get started

> I usually develop on macOS and Linux, the following dependency installations may need to be adjusted depending on your environment.

## Install the dependencies

Make sure that common development tools like `clang++`, `cmake`, `brew` are installed.

### Dependency List
* **sdl2**: basic GUI
* **sdl2_ttf**: basic GUI font
* **fmt**: format things in C++ code
* **indicators**: UI widget for display the training progress
* **glog**: logger library
* **gflags**: process command line parameters
* **nlohmann-json**: json file IO


### Install
```bash
brew install sdl2 sdl2_ttf fmt indicators glog gflags nlohmann-json
```

## Build

```bash
cd SnakeCharlie
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . -- -j 10
```

## Run

```bash
# AI will use the trained neural network to play snake game.
./snake -mode 2
```

for other mode, see the usage info:
```
❯ ./snake -h
snake: Usage:
  snake -mode <running mode>
      running mode should be one of '0/1/2', '0' is default.

      0: human play
      1: train the AI
      2: AI play
```

## Config file explaination
Several node in config/appConfig.json:
* **ai**: parameters in AI player mode
* **playboard**: the size of the configuration panel, if you plan to start training from 0, the game panel size will be too large and cause too long training time
* **snakeApp**: parameters in human player mode
* **training**: parameters for training mode
* **ui**: parameters for the configuration interface


# Future
Training neural networks using genetic algorithms is effective but inefficient. I think a better way would be to use deep reinforcement learning, also known as Q-Learning.

So, continue to implement deep reinforcement learning from scratch in C++?

