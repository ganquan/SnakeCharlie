#include "AppConfig.h"
#include "Utility.h"
#include <cctype>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

AppConfig::AppConfig() {

}

AppConfig::~AppConfig() { saveAppConfig(); }

void AppConfig::setDefaultValue() {

    // snakeapp
    initMoveInterval = 250;
    minMoveInterval = 150;
    interval = 10;

    // snakeNode
    snakeBodyWidth = 40;
    snakeBodyHeight = 40;

    // playboardNode
    playboardRow = 10;
    playboardCol = 10;
    playboardDrawGrid = false;
    playboardGridSize = 40;

    // UINode
    margin = 20;
    padding = 20;

    lableFontPath = std::string("../assets/fonts/Monaco.ttf");
    lableFontSize = 13;

    windowTitle = std::string("SnakeApp");

    // AINode
    nnFilename = std::string("../config/network.json");
    playManually = false;
    AI_initMoveInterval = 100;
    AI_minMoveInterval = 100;
    AI_interval = 10;

    // training Node
    maxGeneration = 100;
    populationSize = 500;
    sampleSize = 50;

    reportFrequency = 100;
    saveFrequency = 100;
    wanderThreshold = playboardRow * playboardCol * 1.5;
    strictWander = false;

    taskName = std::string("happysnake");
    topology = std::vector<int>{28, 8, 4};
    trainingDataPath = std::string("../config/training");
    latestSaveGeneration = 0;
    latestSaveTimestamp = std::string("");
}

void AppConfig::initAppConfig() {
    if (std::filesystem::exists(defaultConfigFileName)) {
        fmt::print("{} exist, load app config.\n", defaultConfigFileName);
        loadAppConfig();
    } else {
        fmt::print("{} NOT exist. Use default app config and save.\n", defaultConfigFileName);
        setDefaultValue();
        saveAppConfig();
    }
}

void AppConfig::saveAppConfig(const std::string &filename) {

    json j = {};

    json snakeAppNode;

    json snakeAppSpeedNode;
    snakeAppSpeedNode["initMoveInterval"] = this->initMoveInterval;
    snakeAppSpeedNode["minMoveInterval"] = this->minMoveInterval;
    snakeAppSpeedNode["intervalStep"] = this->interval;
    snakeAppNode["speed"] = snakeAppSpeedNode;

    json snakeNode;
    snakeNode["snakeBodyWidth"] = this->snakeBodyWidth;
    snakeNode["snakeBodyHeight"] = this->snakeBodyHeight;

    json playboardNode;
    playboardNode["playboardRow"] = this->playboardRow;
    playboardNode["playboardCol"] = this->playboardCol;
    playboardNode["drawGrid"] = this->playboardDrawGrid;
    playboardNode["girdSize"] = this->playboardGridSize;

    json UI_node;
    UI_node["padding"] = this->padding;
    UI_node["margin"] = this->margin;
    UI_node["lableFontPath"] = this->lableFontPath;
    UI_node["lableFontSize"] = this->lableFontSize;
    UI_node["windowTitle"] = this->windowTitle;

    json training_node;
    training_node["maxGeneration"] = this->maxGeneration;
    training_node["populationSize"] = this->populationSize;
    training_node["sampleSize"] = this->sampleSize;

    training_node["reportFrequency"] = this->reportFrequency;
    training_node["saveFrequency"] = this->saveFrequency;
    training_node["wanderThreshold"] = this->wanderThreshold;
    training_node["strictWander"] = this->strictWander;

    training_node["taskName"] = this->taskName;
    training_node["topology"] = this->topology;
    training_node["trainingDataPath"] = this->trainingDataPath;
    training_node["latestSaveGeneration"] = this->latestSaveGeneration;
    training_node["latestSaveTimestamp"] = this->latestSaveTimestamp;

    json AI_node;
    AI_node["nnFile"] = this->nnFilename;
    AI_node["playManually"] = this->playManually;
    
    json AI_SpeedNode;
    AI_SpeedNode["initMoveInterval"] = this->AI_initMoveInterval;
    AI_SpeedNode["minMoveInterval"] = this->AI_minMoveInterval;
    AI_SpeedNode["intervalStep"] = this->AI_interval;
    AI_node["speed"] = AI_SpeedNode;

    j["snakeApp"] = snakeAppNode;
    j["snake"] = snakeNode;
    j["playboard"] = playboardNode;
    j["ui"] = UI_node;
    j["ai"] = AI_node;
    j["training"] = training_node;

    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;

    o.close();
}

void AppConfig::loadAppConfig(const std::string &filename) {
    std::ifstream i(filename);
    json jappconfig;
    i >> jappconfig;
    i.close();

    json snakeAppNode = jappconfig["snakeApp"];

    json snakeAppSpeedNode;
    snakeAppSpeedNode = snakeAppNode["speed"];

    this->initMoveInterval = snakeAppSpeedNode["initMoveInterval"];
    this->minMoveInterval = snakeAppSpeedNode["minMoveInterval"];
    this->interval = snakeAppSpeedNode["intervalStep"];

    json snakeNode = jappconfig["snake"];
    this->snakeBodyWidth = snakeNode["snakeBodyWidth"];
    this->snakeBodyHeight = snakeNode["snakeBodyHeight"];

    json playboardNode = jappconfig["playboard"];
    this->playboardRow = playboardNode["playboardRow"];
    this->playboardCol = playboardNode["playboardCol"];
    this->playboardDrawGrid = playboardNode["drawGrid"];
    this->playboardGridSize = playboardNode["girdSize"];

    json UI_node = jappconfig["ui"];
    this->padding = UI_node["padding"];
    this->margin = UI_node["margin"];
    this->lableFontPath = UI_node["lableFontPath"];
    this->lableFontSize = UI_node["lableFontSize"];
    this->windowTitle = UI_node["windowTitle"];

    json AI_node = jappconfig["ai"];
    this->nnFilename = AI_node["nnFile"];
    this->playManually = AI_node["playManually"];

    json AI_SpeedNode;
    AI_SpeedNode = AI_node["speed"];
    this->AI_initMoveInterval = AI_SpeedNode["initMoveInterval"];
    this->AI_minMoveInterval = AI_SpeedNode["minMoveInterval"];
    this->AI_interval = AI_SpeedNode["intervalStep"];

    json training_node = jappconfig["training"];
    this->maxGeneration = training_node["maxGeneration"];
    this->populationSize = training_node["populationSize"];
    this->sampleSize = training_node["sampleSize"];

    this->reportFrequency = training_node["reportFrequency"];
    this->saveFrequency = training_node["saveFrequency"];
    this->wanderThreshold = training_node["wanderThreshold"];
    this->strictWander = training_node["strictWander"];

    this->taskName = training_node["taskName"];
    this->topology = training_node["topology"].get<std::vector<int>>();
    this->trainingDataPath = training_node["trainingDataPath"];
    this->latestSaveGeneration = training_node["latestSaveGeneration"];
    this->latestSaveTimestamp = training_node["latestSaveTimestamp"];
}