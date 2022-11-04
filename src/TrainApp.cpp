#include "TrainApp.h"

#include "AppConfig.h"
#include "NeuralNetwork/Matrix.h"
#include "NeuralNetwork/NeuralNetwork.h"
#include "SnakeApp.h"
#include "SnakeModel.h"
#include "Thread/ThreadPool.h"
#include "Utility.h"
#include <chrono>
#include <filesystem>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <fstream>
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <thread>
#include <vector>

using namespace indicators;

TrainApp::TrainApp() {

    m_trainTaskDone = false;

    m_maxGeneration = AppConfig::MaxGeneration();
    m_populationSize = AppConfig::PopulationSize();
    m_sampleSize = AppConfig::SampleSize();

    m_generation = 1;

    m_samplesFitnessSum = 0;

    m_trainingDataPath = AppConfig::TrainingDataPath();
    m_trainingTaskName = AppConfig::TrainingTaskName();
    m_latestSaveGeneration = AppConfig::LatestSaveGeneration();

    m_genDirNameFormat = std::string("gen_{:07}");
    m_TrainResultFileNameFormat = std::string("nn_{:05}.json");

    m_pool = new ThreadPool();

    initMutateTable();
    initPopulation();
    initSamples();
}

TrainApp::~TrainApp() {

    LOG(INFO) << fmt::format("pool busy = {}\n", m_pool->isBusy());
    if (m_pool->isBusy()) {
        m_pool->done();
    }

    LOG(INFO) << fmt::format("after pool->done, pool busy = {}\n", m_pool->isBusy());

    delete m_pool;
}

int TrainApp::start() {

    tryRestoreLastTraining();

    if (m_trainTaskDone) {
        LOG(INFO) << "Training task done, won't start.";
        return 0;
    }

    LOG(INFO) << fmt::format("Start training: Generation begin = {}, target = {}, populationSize = {}, sampleSize = {}\n",
                             m_generation,
                             m_maxGeneration,
                             m_populationSize,
                             m_sampleSize);

    fmt::print("Training start. Generation begin = {}, target = {}.\n", m_generation, m_maxGeneration);

    utility::time::measure(m_trainStartTime, m_trainEndTime, [&]() {
        training();
    });

    m_trainDuration = m_trainEndTime - m_trainStartTime;
    fmt::print("Training complete. Cost {}\n", utility::time::formatToString(m_trainDuration));

    // save the last generation samples
    // avoid save twice
    if (AppConfig::LatestSaveGeneration() != m_maxGeneration) {
        m_samples.clear();
        this->selectPopulationTo(m_samples, m_sampleSize);
        this->saveTrainingResults(m_samples, m_maxGeneration);

        this->updateLatestSaveGenInfo(m_maxGeneration);
        this->updateLatestNNFile(m_maxGeneration);
    } else {
        LOG(INFO) << "Training result already saved. no need save 2nd time.";
    }

    return 0;
}

void TrainApp::training() {

    std::string result;
    std::string label = fmt::format("GA: training total");

    utility::time::measure(label, result, [&]() {
        this->trainingImpl();
    });

    LOG(INFO) << result;
}

void TrainApp::trainingImpl() {

    show_console_cursor(false);

    BlockProgressBar bar{
        option::BarWidth{50},
        option::ForegroundColor{Color::green},
        option::ShowElapsedTime{true},
        option::ShowRemainingTime{true},
        option::MaxProgress{m_maxGeneration}};

    bar.set_progress(m_generation);

    do {
        bar.set_option(
            option::PostfixText{std::to_string(m_generation) + "/" + std::to_string(m_maxGeneration)});

        m_generationStartTime = std::chrono::high_resolution_clock::now();

        ///////////////////////////////////////////////////////////////////////
        this->evaluate();
        this->samplingEvaluateResult();

        // next generation

        if (m_generation + 1 <= m_maxGeneration) {
            this->selection();
            this->crossover();
            this->mutate();
        }
        ///////////////////////////////////////////////////////////////////////

        m_generationEndTime = std::chrono::high_resolution_clock::now();
        m_generationDuration = m_generationEndTime - m_generationStartTime;

        std::string generationLog = fmt::format("GA: generation = {} Total - {}",
                                                m_generation,
                                                utility::time::formatToString(m_generationDuration));
        LOG(INFO) << generationLog;
        bar.set_progress(m_generation);

    } while (++m_generation <= m_maxGeneration);

    bar.mark_as_completed();
    show_console_cursor(true);
}

void TrainApp::evaluate() {
    std::string result;
    std::string label = fmt::format("GA: generation = {} evaluate", m_generation);

    utility::time::measure(label, result, [&]() {
        this->evaluateImpl();
    });

    LOG(INFO) << result;
}

void TrainApp::evaluateImpl() {
    // run until all snakes die
    for (auto &snakeApp : m_population) {
        m_pool->queueJob([&snakeApp]() {
            snakeApp->start();
        });
    }

    waitDieOut();

    // caculate the fitness
    for (auto &snakeApp : m_population) {
        snakeApp->getSnakeModel()->fitness();
    }

    // sort the rank
    std::sort(m_population.begin(), m_population.end(),
              [](const auto &lhs, const auto &rhs) { return lhs->getSnakeModel()->getRank() > rhs->getSnakeModel()->getRank(); });
}

void TrainApp::samplingEvaluateResult() {
    // print top
    if (0 == m_generation % AppConfig::ReportFrequency()) {
        reportGenerationInfo(m_generation);
    }

    // save this generation
    if (0 == m_generation % AppConfig::SaveFrequency()) {
        saveSamples();
    }
}

void TrainApp::selection() {
    std::string result;
    std::string label = fmt::format("GA: generation = {} selection", m_generation);

    utility::time::measure(label, result, [&]() {
        this->selectionImpl();
    });

    LOG(INFO) << result;
}

void TrainApp::selectionImpl() {
    m_samplesFitnessSum = 0;
    m_samples.clear();

    selectPopulationTo(m_samples, m_sampleSize);

    std::for_each(m_samples.begin(), m_samples.end(), [this](const auto &s) {
        this->m_samplesFitnessSum += s->getSnakeModel()->getRank();
    });
}

void TrainApp::crossover() {
    std::string result;
    std::string label = fmt::format("GA: generation = {} crossover", m_generation);

    utility::time::measure(label, result, [&]() {
        this->crossoverImpl();
    });

    LOG(INFO) << result;
}

void TrainApp::crossoverImpl() {
    // crossover make next generation population
    m_population.clear();

    // add the samples to next generation without reduce the crossover population size.
    // next generation should be AppConfig::PopulationSize() + m_sampleSize;
    // crossoverSize = AppConfig::PopulationSize()
    m_populationSize = AppConfig::PopulationSize() + m_sampleSize;
    m_population.reserve(m_populationSize);

    for (int i = 0; i < m_populationSize; i++) {
        m_population.push_back(std::make_shared<SnakeApp>());
    }

    int eliteSize = m_sampleSize;
    int crossoverSize = m_populationSize;

    //杂交产生后代
    std::for_each(m_population.begin(), m_population.begin() + crossoverSize, [this](const auto &s) {
        m_pool->queueJob([&]() {
            ///////////////////////////////
            int parentIndex1 = RouletteWheelSelection(this->m_samples, this->m_samplesFitnessSum);
            int parentIndex2 = RouletteWheelSelection(this->m_samples, this->m_samplesFitnessSum);
            while ((parentIndex1 == parentIndex2) && (parentIndex2 = RouletteWheelSelection(m_samples, this->m_samplesFitnessSum)))
                ;
            ///////////////////////////////
            auto topology = s->getSnakeModel()->getBrain()->getNeuralNetwork()->getTopology();
            int weightMatrixNum = topology.size() - 1;
            for (int weightIndex = 0; weightIndex < weightMatrixNum; weightIndex++) {

                std::shared_ptr<Matrix> weight = s->getSnakeModel()->getBrain()->getNeuralNetwork()->weightMatrixAt(weightIndex);

                for (int i = 0; i < weight->getRowNum(); i++) {
                    for (int j = 0; j < weight->getColNum(); j++) {

                        double d = utility::random::generateRandomDouble(0, 1);
                        double newValue = 0.0;
                        if (d > 0.5) {
                            newValue = m_samples[parentIndex1]->getSnakeModel()->getBrain()->getNeuralNetwork()->weightMatrixAt(weightIndex)->getValue(i, j);
                        } else {
                            newValue = m_samples[parentIndex2]->getSnakeModel()->getBrain()->getNeuralNetwork()->weightMatrixAt(weightIndex)->getValue(i, j);
                        }

                        weight->setValue(i, j, newValue);
                    }
                }
            }

            s->getSnakeModel()->setCrossoverFlag(true);
            ///////////////////////////////
        });
    });

    std::string result;
    std::string label = fmt::format("GA: generation = {} waitCrossover", m_generation);

    utility::time::measure(label, result, [&]() {
        waitCrossover(crossoverSize);
    });

    LOG(INFO) << result;

    //精英直接保留
    for (int pIndex = crossoverSize, eIndex = 0; pIndex < m_populationSize && eIndex < eliteSize; pIndex++, eIndex++) {

        /////////////////////////
        auto topology = m_population[pIndex]->getSnakeModel()->getBrain()->getNeuralNetwork()->getTopology();
        int weightMatrixNum = topology.size() - 1;

        for (int weightIndex = 0; weightIndex < weightMatrixNum; weightIndex++) {

            std::shared_ptr<Matrix> weight = m_population[pIndex]->getSnakeModel()->getBrain()->getNeuralNetwork()->weightMatrixAt(weightIndex);
            std::shared_ptr<Matrix> eliteWeight = m_samples[eIndex]->getSnakeModel()->getBrain()->getNeuralNetwork()->weightMatrixAt(weightIndex);

            for (int i = 0; i < weight->getRowNum(); i++) {
                for (int j = 0; j < weight->getColNum(); j++) {
                    weight->setValue(i, j, eliteWeight->getValue(i, j));
                }
            }
        }
        /////////////////////////
    }
}

void TrainApp::mutate() {
    std::string result;
    std::string label = fmt::format("GA: generation = {} mutate", m_generation);

    utility::time::measure(label, result, [this]() {
        this->mutateImpl();
    });

    LOG(INFO) << result;
}

void TrainApp::mutateImpl() {

    std::for_each(m_population.begin(), m_population.end(), [this](const auto &s) {
        s->getSnakeModel()->getBrain()->mutate(m_mutateValueTable);
    });
}

void TrainApp::initMutateTable() {

    std::string result;
    std::string label = fmt::format("GA: generation = {} initMutateTable", m_generation);

    utility::time::measure(label, result, [&]() {
        m_mutateValueTable.reserve(c_mutateValueTableSize);
        m_mutateValueTable.clear();

        for (double start = c_mutateValueRangeLow; start <= c_mutateValueRangeHigh; start += c_mutateValueStep) {
            m_mutateValueTable.push_back(start);
        }
    });

    LOG(INFO) << result;
}

void TrainApp::initPopulation() {

    std::string result;
    std::string label = fmt::format("GA: generation = {} initPopulation", m_generation);

    utility::time::measure(label, result, [&]() {
        m_population.reserve(m_populationSize);
        m_population.clear();

        for (int i = 0; i < m_populationSize; i++) {
            auto s = std::make_shared<SnakeApp>();
            m_population.push_back(s);
        }
    });

    LOG(INFO) << result;
}

void TrainApp::initSamples() {

    std::string result;
    std::string label = fmt::format("GA: generation = {} initSamples", m_generation);

    utility::time::measure(label, result, [&]() {
        m_samples.reserve(m_sampleSize);
        m_samples.clear();
    });

    LOG(INFO) << result;
}

bool TrainApp::waitDieOut() {

    int checkCount = 0;
    bool natureRunning = true;

    while (natureRunning) {
        checkCount++;

        int aliveCount = m_population.size();
        for (auto &snakeApp : m_population) {
            if (snakeApp->getState().isEnd()) {
                aliveCount--;
            } else {
#if 0
                // disable high frequency log temporarily
                LOG(INFO) << fmt::format("checking alive snake, totalStepCount = {}, eatStepCount = {}, score = {}",
                                         snakeApp->getSnakeModel()->getTotalStepCount(),
                                         snakeApp->getSnakeModel()->getEatStepCount(),
                                         snakeApp->getSnakeModel()->getScore());
#endif
            }
        }

        if (0 == aliveCount) {
            natureRunning = false;
            break;
        }

        auto sleepTime = std::chrono::milliseconds(5);
        std::this_thread::sleep_for(sleepTime);
    }

    return true;
}

bool TrainApp::waitCrossover(int crossoverCount) {

    int checkCount = 0;
    bool checking = true;

    while (checking) {
        checkCount++;

        int counter = crossoverCount;
        for (int i = 0; i < crossoverCount; i++) {
            if (m_population[i]->getSnakeModel()->getCrossoverFlag()) {
                counter--;
            }
        }

        if (0 == counter) {
            checking = false;
            break;
        }

        auto sleepTime = std::chrono::milliseconds(5);
        std::this_thread::sleep_for(sleepTime);
    }

    return true;
}

int TrainApp::RouletteWheelSelection(const std::vector<std::shared_ptr<SnakeApp>> &population, const long double &fitnessSum) {

    long double slice = utility::random::generateRandomDouble(0, 1) * fitnessSum;
    long double total = 0.0;
    int selectedIndex = -1;
    int size = population.size();
    for (int i = 0; i < size; i++) {
        total += population[i]->getSnakeModel()->getRank();

        if (total > slice) {
            selectedIndex = i;
            break;
        }
    }

    return selectedIndex;
}

void TrainApp::saveSamples() {

    saveTrainingResults(m_samples, m_generation);

    updateLatestSaveGenInfo(m_generation);
    updateLatestNNFile(m_generation);
}

void TrainApp::tryRestoreLastTraining() {

    fs::path taskDir = m_trainingDataPath / m_trainingTaskName;
    if (fs::exists(taskDir)) {
        fmt::print("Training task found: {}\n", taskDir.string());
        LOG(INFO) << "Training task already exist";
        // already exist training task
        int latestSavedSamples = AppConfig::LatestSaveGeneration();

        if (AppConfig::LatestSaveGeneration() < AppConfig::MaxGeneration()) {
            // check the gen dir exist
            std::string name = fmt::format(m_genDirNameFormat, latestSavedSamples);
            auto genDirName = fs::path(name);
            auto genDir = taskDir / genDirName;

            if (fs::exists(genDir)) {
                // load the samples and contitnue training
                LOG(INFO) << fmt::format("latestSavedSamples dir exist: {}, load the samples and continue training", name);
                this->restoreFromSavedSamples();
                fmt::print("Restore from last saved generation {} success.\n", AppConfig::LatestSaveGeneration());

            } else {
                LOG(ERROR) << fmt::format("latestSavedSamples = {}, but data missing. start new training", name);
            }

        } else {
            // train is done.
            m_trainTaskDone = true;
            fmt::print("Training task done, won't start.\n");
            LOG(INFO) << "LatestSaveGeneration equals MaxGeneration, training task is done";
        }

    } else {
        // new training task
        // set the weights to random value
        for (int i = 0; i < m_populationSize; i++) {
            m_population[i]->getSnakeModel()->getBrain()->getNeuralNetwork()->setWeightMatricesWithRandomValue();
        }

        LOG(INFO) << "not found exist task, new training task";
    }
}

void TrainApp::restoreFromSavedSamples() {

    // init m_samples
    LOG(INFO) << "restoreFromSavedSamples init m_samples.";
    m_samples.clear();
    for (int i = 0; i < m_sampleSize; i++) {
        m_samples.push_back(std::make_shared<SnakeApp>());
    }

    // load sample files.
    LOG(INFO) << "restoreFromSavedSamples load sample files.";
    fs::path genDir;
    createGenerationDir(AppConfig::LatestSaveGeneration(), genDir);

    for (int i = 0; i < m_sampleSize; i++) {
        std::string filename = fmt::format(m_TrainResultFileNameFormat, i);
        auto fileURI = genDir / fs::path(filename);
        auto fileURIStr = fileURI.string();

        m_samples[i]->getSnakeModel()->getBrain()->getNeuralNetwork()->loadNeuralNetwork(fileURIStr);
        json nnDescription = m_samples[i]->getSnakeModel()->getBrain()->getNeuralNetwork()->getDescription();

        long double fitness = nnDescription["fitness"];

        // RouletteWheelSelection need fitness and sum
        this->m_samplesFitnessSum += fitness;
        m_samples[i]->getSnakeModel()->setRank(fitness);
    }

    // crossover make the population
    LOG(INFO) << "restoreFromSavedSamples crossover make the population.";
    this->crossover();

    // mutate
    LOG(INFO) << "restoreFromSavedSamples mutate.";
    this->mutate();

    // m_generation + 1
    m_generation = AppConfig::LatestSaveGeneration() + 1;

    LOG(INFO) << "restoreFromSavedSamples restore finished, will start training.";
    // start training....
}

void TrainApp::reportGenerationInfo(int generation) {

    LOG(INFO) << fmt::format("====================== Generation Report: {:05} ========================\n", generation);

    double genScoreSum = 0;
    double genFitnessSum = 0;
    std::for_each(m_population.begin(), m_population.end(), [&genScoreSum, &genFitnessSum](const auto &s) {
        genFitnessSum += s->getSnakeModel()->getRank();
        genScoreSum += s->getSnakeModel()->getScore();
    });

    LOG(INFO) << fmt::format("generation {} - avg_score = {}\n",
                             generation,
                             genScoreSum / m_population.size());

    LOG(INFO) << fmt::format("top 3:\n");
    std::for_each(m_population.begin(), m_population.begin() + 3, [](const auto &s) {
        LOG(INFO) << fmt::format("fitness = {}, score = {}, step = {}\n",
                                 s->getSnakeModel()->getRank(),
                                 s->getSnakeModel()->getScore(),
                                 s->getSnakeModel()->getTotalStepCount());
    });

    LOG(INFO) << fmt::format("======================== Generation Report End =========================\n");
}

void TrainApp::buildTrainingInfo(json &authorInfo, const std::shared_ptr<SnakeApp> &snakeApp) {

    std::time_t t = std::time(nullptr);
    std::string create = fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(t));

    authorInfo["populationSize"] = AppConfig::PopulationSize();
    authorInfo["sampleSize"] = m_sampleSize;
    authorInfo["generation"] = m_generation;

    authorInfo["fitness"] = snakeApp->getSnakeModel()->getRank();
    authorInfo["score"] = snakeApp->getSnakeModel()->getScore();
    authorInfo["stepCount"] = snakeApp->getSnakeModel()->getTotalStepCount();

    authorInfo["create"] = create;
}

void TrainApp::createTrainingTaskDir() {

    fs::path taskDir = m_trainingDataPath / m_trainingTaskName;

    if (!fs::exists(taskDir)) {

        bool r = fs::create_directories(taskDir);
        if (!r) {
            LOG(ERROR) << "create training taskDir error, path = " << taskDir;
        } else {
            LOG(INFO) << "create training taskDir success, path = " << taskDir;
        }
    }
}

void TrainApp::createGenerationDir(int gen, fs::path &generationDir) {

    auto genDirNameStr = fmt::format(m_genDirNameFormat, gen);
    auto genDirName = fs::path(genDirNameStr);
    auto genDir = m_trainingDataPath / m_trainingTaskName / genDirName;
    generationDir = genDir;

    if (!fs::exists(genDir)) {

        bool r = fs::create_directories(genDir);
        if (!r) {
            LOG(ERROR) << "create training genDir error, path = " << genDir;
        } else {
            LOG(INFO) << "create training genDir success, path = " << genDir;
        }
    }
}

void TrainApp::selectPopulationTo(std::vector<std::shared_ptr<SnakeApp>> &target, int size) {
    target.clear();

    std::for_each(m_population.begin(), m_population.begin() + size, [&target](const auto &s) {
        target.push_back(s);
    });
}

void TrainApp::saveTrainingResults(const std::vector<std::shared_ptr<SnakeApp>> &src, int gen) {

    fs::path genDir;
    createTrainingTaskDir();
    createGenerationDir(gen, genDir);

    int resultsNum = src.size();
    for (int i = 0; i < resultsNum; i++) {

        std::string filename = fmt::format(m_TrainResultFileNameFormat, i);
        auto fileURI = genDir / fs::path(filename);

        json trainingInfo;

        this->buildTrainingInfo(trainingInfo, src[i]);

        src[i]->getSnakeModel()->getBrain()->getNeuralNetwork()->setDescription(trainingInfo);
        src[i]->getSnakeModel()->getBrain()->getNeuralNetwork()->saveNeuralNetwork(fileURI.string());
    }
}

void TrainApp::updateLatestSaveGenInfo(int gen) {

    // update the task latestSaveGeneration and latestSaveTimestamp in appconfig.json
    std::time_t t = std::time(nullptr);
    std::string create = fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(t));
    AppConfig::Get().setLatestSaveGeneration(gen);
    AppConfig::Get().setLatestSaveTimestamp(create);
    AppConfig::Get().saveAppConfig();
}

void TrainApp::updateLatestNNFile(int gen) {

    auto genDirNameStr = fmt::format(m_genDirNameFormat, gen);
    auto genDirName = fs::path(genDirNameStr);
    auto genDir = m_trainingDataPath / m_trainingTaskName / genDirName;

    // the first one should be the top nn, which index is 0
    std::string nnFilenameStr = fmt::format(m_TrainResultFileNameFormat, 0);
    auto nnFileURIString = (genDir / fs::path(nnFilenameStr)).string();

    updateLatestNNFile(nnFileURIString);
}
void TrainApp::updateLatestNNFile(std::string &filepath) {
    AppConfig::Get().setNeuralNetworkFilename(filepath);
    AppConfig::Get().saveAppConfig();
}
