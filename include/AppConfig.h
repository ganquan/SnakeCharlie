#pragma once
#include "AppRunMode.h"
#include <SDL2/SDL.h>
#include <filesystem>
#include <fmt/core.h>
#include <map>
#include <string>
#include <vector>

class AppConfig {

public:
    // public methods for user
    static AppConfig &Get() {
        static AppConfig s_instance;
        return s_instance;
    }
    /* SnakeApp */
    static AppRunMode RunMode() { return Get().ImplRunMode(); }
    static int InitInterval() { return Get().ImplInitInterval(); }
    static int MinInterval() { return Get().ImplMinInterval(); }
    static int IntervalStep() { return Get().ImplInterval(); }

    /* Snake Node */
    // snake block width, in pixel
    static int SnakeBlockWidth() { return Get().ImplSnakeBlockWidth(); }
    // snake block height, in pixel
    static int SnakeBlockHeight() { return Get().ImplSnakeBlockHeight(); }

    /* Playboard Node */
    static int PlayboardRowNum() { return Get().ImplPlayboardRowNum(); }
    static int PlayboardColNum() { return Get().ImplPlayboardColNum(); }
    static int PlayboardSize() { return Get().ImplPlayboardSize(); }
    static bool PlayboardDrawGrid() { return Get().ImplPlayboardDrawGrid(); }
    static int PlayboardGridSize() { return Get().ImplPlayboardGridSize(); }

    /* UI Node */
    static std::string GUIWindowTitle() { return Get().ImplGUIWindowTitle(); }

    static int GUIWindowWidth() { return Get().ImplGUIWindowWidth(); }
    static int GUIWindowHeight() { return Get().ImplGUIWindowHeight(); }
    static int PlayboardWidth() { return Get().ImplPlayboardWidth(); }
    static int PlayboardHeight() { return Get().ImplPlayboardHeight(); }
    static int InfoboardWidth() { return Get().ImplInfoboardWidth(); }
    static int InfoboardHeight() { return Get().ImplInfoboardHeight(); }

    static int Margin() { return Get().ImplMargin(); }
    static int Padding() { return Get().ImplPadding(); }
    static std::string TextLabelFontPath() { return Get().ImplTextLabelFontPath(); }
    static int TextLabelFontSize() { return Get().ImplTextLabelFontSize(); }

    /* AI Node */
    static std::string NeuralNetworkFilename() { return Get().ImplNeuralNetworkFilename(); }
    static bool PlayManually() { return Get().ImplPlayManually(); }

    /* Training Node */
    static int MaxGeneration() { return Get().ImplMaxGeneration(); }
    static int PopulationSize() { return Get().ImplPopulationSize(); }
    static int SampleSize() { return Get().ImplSampleSize(); }

    static int ReportFrequency() { return Get().ImplReportFrequency(); }
    static int SaveFrequency() { return Get().ImplSaveFrequency(); }
    static int WanderThreshold() { return Get().ImplWanderThreshold(); }
    static bool StrictWander() { return Get().ImpStrictWander(); }

    static std::string TrainingTaskName() { return Get().ImplTrainingTaskName(); }
    static std::vector<int> TrainingTopology() { return Get().ImplTrainingTopology(); }
    static std::string TrainingDataPath() { return Get().ImplTrainingDataPath(); }
    static int LatestSaveGeneration() { return Get().ImplLatestSaveGeneration(); }

private:
    // implementation of public methods
    /* SnakeApp */
    AppRunMode ImplRunMode() { return m_appRunMode; }

    inline int ImplInitInterval() { return AppConfig::RunMode().isAIMode() ? AI_initMoveInterval : initMoveInterval; }
    inline int ImplMinInterval() { return AppConfig::RunMode().isAIMode() ? AI_minMoveInterval : minMoveInterval; }
    inline int ImplInterval() { return AppConfig::RunMode().isAIMode() ? AI_interval : interval; }

    /* Snake Node */
    inline int ImplSnakeBlockWidth() { return snakeBodyWidth; }
    inline int ImplSnakeBlockHeight() { return snakeBodyHeight; }

    /* Playboard Node */
    inline int ImplPlayboardRowNum() { return playboardRow; }
    inline int ImplPlayboardColNum() { return playboardCol; }
    inline int ImplPlayboardSize() { return ImplPlayboardRowNum() * ImplPlayboardColNum(); }

    inline bool ImplPlayboardDrawGrid() { return playboardDrawGrid; }
    inline int ImplPlayboardGridSize() { return playboardGridSize; }

    /* UI Node */
    inline std::string ImplGUIWindowTitle() { return windowTitle; }
    inline int ImplGUIWindowWidth() {
        return ImplPlayboardWidth()               // playboard
               + 2 * ImplMargin() + ImplPadding() // margin * 2 + padding
               + ImplInfoboardWidth();            // info board
    }
    inline int ImplGUIWindowHeight() {
        return ImplPlayboardHeight() // playboard
               + 2 * ImplMargin();
    }

    inline int ImplPlayboardWidth() { return ImplPlayboardColNum() * ImplSnakeBlockWidth(); }
    inline int ImplPlayboardHeight() { return ImplPlayboardRowNum() * ImplSnakeBlockHeight(); }

    inline int ImplInfoboardWidth() { return ImplPlayboardWidth() * 0.8; }
    inline int ImplInfoboardHeight() { return ImplPlayboardHeight(); }

    inline int ImplMargin() { return margin; }
    inline int ImplPadding() { return padding; }
    inline std::string ImplTextLabelFontPath() { return lableFontPath; }
    inline int ImplTextLabelFontSize() { return lableFontSize; }

    /* AI Node */
    inline std::string ImplNeuralNetworkFilename() { return nnFilename; }
    inline bool ImplPlayManually() { return playManually; }

    /* Training Node */
    inline int ImplMaxGeneration() { return maxGeneration; }
    inline int ImplPopulationSize() { return populationSize; }
    inline int ImplSampleSize() { return sampleSize; }

    inline int ImplReportFrequency() { return reportFrequency; }
    inline int ImplSaveFrequency() { return saveFrequency; }
    inline int ImplWanderThreshold() { return wanderThreshold; }
    inline bool ImpStrictWander() { return strictWander; }

    inline std::string ImplTrainingTaskName() { return taskName; }
    inline std::vector<int> ImplTrainingTopology() { return topology; }

    inline std::string ImplTrainingDataPath() { return trainingDataPath; }
    inline int ImplLatestSaveGeneration() { return latestSaveGeneration; }

public:
    void setAppRunMode(AppRunMode mode) { m_appRunMode = mode; }
    void setNeuralNetworkFilename(const std::string &filename) { nnFilename = std::string(filename); }
    void setLatestSaveGeneration(int gen) { latestSaveGeneration = gen; };
    void setLatestSaveTimestamp(std::string create) { latestSaveTimestamp = create; }

    void initAppConfig();
    void saveAppConfig(const std::string &filename = "../config/appConfig.json");
    void loadAppConfig(const std::string &filename = "../config/appConfig.json");

    // Singleton pattern delete these copy method
public:
    AppConfig(AppConfig const &) = delete;
    void operator=(AppConfig const &) = delete;

private:
    // private members
    const std::string defaultConfigFileName = "../config/appConfig.json";
    AppRunMode m_appRunMode = AppRunMode::human;
    int initMoveInterval;
    int minMoveInterval;
    int interval;

    // snake Node
    int snakeBodyWidth;
    int snakeBodyHeight;

    // playboard Node
    int playboardRow;
    int playboardCol;
    bool playboardDrawGrid;
    int playboardGridSize;

    // UINode
    int margin;
    int padding;

    std::string lableFontPath;
    int lableFontSize;
    std::string windowTitle;

    // AI Node
    std::string nnFilename;
    bool playManually;
    int AI_initMoveInterval;
    int AI_minMoveInterval;
    int AI_interval;

    // training Node
    int maxGeneration;
    int populationSize;
    int sampleSize;

    int reportFrequency;
    int saveFrequency;
    int wanderThreshold;
    bool strictWander;

    std::string taskName;
    std::vector<int> topology;
    std::string trainingDataPath;
    int latestSaveGeneration;
    std::string latestSaveTimestamp;

private:
    AppConfig();
    ~AppConfig();

    void setDefaultValue();
};
