# CMakeLists.txt
cmake_minimum_required(VERSION "3.2")

project(SnakeCharlie)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  #set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# SET(CHECK_DATARACE_FLAG "-fsanitize=thread")
SET(CHECK_DATARACE_FLAG "")

# SET(CHECK_LEAK_FLAG "-fsanitize=address")
SET(CHECK_LEAK_FLAG "")

# run as below to generate sanitize report
# NOTE: all sanitize detect are very expensive, will reduce performance and increase memory consumption significantly
# ./snake > sanitize.log 2>&1
SET(COMPILE_WARNNING_FLAG "-Wall -Wextra")

# export the compile commands json file by default
# lsp need this file
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${COMPILE_WARNNING_FLAG} ${CHECK_DATARACE_FLAG} ${CHECK_LEAK_FLAG}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  ADD_DEFINITIONS(-DDEBUG_BINARY)
endif()
#build a release binary
#cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . -- -j 10

set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BIN_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(glog REQUIRED)
find_package(gflags REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(indicators REQUIRED)

message(STATUS "SDL2_FOUND: ${SDL2_FOUND}")
message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")

message(STATUS "SDL2_ttf_FOUND: ${SDL2_ttf_FOUND}")
message(STATUS "SDL2_ttf_LIBRARIES: ${SDL2_ttf_LIBRARIES}")

message(STATUS "glog_FOUND: ${glog_FOUND}")
message(STATUS "glog_LIBRARIES: ${glog_LIBRARIES}")

message(STATUS "gflags_FOUND: ${gflags_FOUND}")
message(STATUS "gflags_LIBRARIES: ${gflags_LIBRARIES}")

message(STATUS "fmt_FOUND: ${fmt_FOUND}")
message(STATUS "fmt_LIBRARIES: ${fmt_LIBRARIES}")

message(STATUS "indicators_FOUND: ${indicators_FOUND}")
message(STATUS "indicators_LIBRARIES: ${indicators_LIBRARIES}")


# add extra include directories
include_directories(include)

add_executable(snake
  src/main.cpp
  src/AppRunMode.cpp
  src/AppConfig.cpp
  src/SnakeApp.cpp

  src/BlockModel.cpp
  src/SnakeModel.cpp
  src/SnakeDirection.cpp
  src/SnakeState.cpp
  src/PlayboardModel.cpp
  src/InfoboardModel.cpp

  src/FPSTimer.cpp
  src/UIManager.cpp
  src/Entity.cpp
  src/EntityManager.cpp
  src/TexturedRectangle.cpp
  src/TextLabel.cpp

  src/Utility.cpp

  src/Thread/ThreadPool.cpp
  
  src/SnakeBrain.cpp
  src/TrainApp.cpp

  src/NeuralNetwork/Activate.cpp
  src/NeuralNetwork/Neuron.cpp
  src/NeuralNetwork/Layer.cpp
  src/NeuralNetwork/Matrix.cpp
  src/NeuralNetwork/NeuralNetwork.cpp
  src/NeuralNetwork/Utils.cpp
)

add_custom_command(
  TARGET snake POST_BUILD
  COMMAND cp ./compile_commands.json ../
  VERBATIM)

target_link_libraries(snake
  SDL2::SDL2
  SDL2_ttf
  glog::glog
  gflags
  nlohmann_json::nlohmann_json
  fmt::fmt
  indicators::indicators
)

INSTALL(TARGETS snake DESTINATION ${BIN_ROOT})
