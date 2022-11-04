#pragma once

#include "Thread/ThreadPool.h"
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

namespace fs = std::filesystem;
using json = nlohmann::json;

class SnakeApp;

class TrainApp {

public:
    TrainApp();
    ~TrainApp();

    int start();

private:
    void training();
    void trainingImpl();
    void evaluate();
    void evaluateImpl();
    void samplingEvaluateResult();
    void selection();
    void selectionImpl();
    void crossover();
    void crossoverImpl();
    void mutate();
    void mutateImpl();

private:
    // helpers
    void initMutateTable();
    void initPopulation();
    void initSamples();

    bool waitDieOut();
    bool waitCrossover(int crossoverCount);
    int RouletteWheelSelection(const std::vector<std::shared_ptr<SnakeApp>> &population, const long double &fitnessSum);
    void saveSamples();

    void createTrainingTaskDir();
    void createGenerationDir(int gen, fs::path &generationDir);

    void reportGenerationInfo(int generation);
    void buildTrainingInfo(json &authorInfo, const std::shared_ptr<SnakeApp> &snakeApp);
    void selectPopulationTo(std::vector<std::shared_ptr<SnakeApp>> &target, int size);
    void saveTrainingResults(const std::vector<std::shared_ptr<SnakeApp>> &src, int gen);

    void updateLatestSaveGenInfo(int gen);
    void updateLatestNNFile(int gen);
    void updateLatestNNFile(std::string &filepath);

    void tryRestoreLastTraining();
    void restoreFromSavedSamples();

private:
    int m_maxGeneration;
    int m_populationSize;
    int m_sampleSize;

    int m_generation;

    long double m_samplesFitnessSum = 0;

    const double c_mutateValueStep = 0.005;
    const double c_mutateValueRangeLow = -0.5;
    const double c_mutateValueRangeHigh = 0.5;
    const int c_mutateValueTableSize = (c_mutateValueRangeHigh - c_mutateValueRangeLow) / c_mutateValueStep;

    std::vector<double> m_mutateValueTable;

    std::vector<std::shared_ptr<SnakeApp>> m_population;
    std::vector<std::shared_ptr<SnakeApp>> m_samples;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_trainStartTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_trainEndTime;
    std::chrono::duration<double, std::milli> m_trainDuration;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_generationStartTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_generationEndTime;
    std::chrono::duration<double, std::milli> m_generationDuration;

    ThreadPool *m_pool;
    bool m_trainTaskDone;

    fs::path m_trainingDataPath;
    fs::path m_trainingTaskName;
    int m_latestSaveGeneration;
    std::string m_genDirNameFormat;
    std::string m_TrainResultFileNameFormat;
};
