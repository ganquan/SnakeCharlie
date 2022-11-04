#include "AppConfig.h"
#include "SnakeApp.h"
#include "TrainApp.h"
#include <fmt/core.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <iostream>
#include <string>

#ifdef DEBUG_BINARY
const bool g_isDebugBinary = true;
#else
const bool g_isDebugBinary = false;
#endif

const std::string g_version = "0.0.3";
const std::string g_help = "Usage:\
                            \n  snake -mode <running mode>\
                            \n      running mode should be one of '0/1/2', '0' is default.\
                            \n      \
                            \n      0: human play\
                            \n      1: train the AI\
                            \n      2: AI play";

DEFINE_bool(h, false, "show help");
DECLARE_bool(help);      // make sure you can access FLAGS_help
DECLARE_bool(helpshort); // make sure you can access FLAGS_helpshort

/* running mode */
static bool ValidateMode(const char *flagname, int64_t value) {
    if (value >= 0 && value <= 2) { // value is ok
        return true;
    }

    std::cout << std::endl;
    std::cout << "Set '" << flagname << "' value error. value = " << value << std::endl;
    std::cout << std::endl;
    std::cout << g_help << std::endl;

    return false;
}
DEFINE_int64(mode, 0, "running mode should be one of '0/1/2', 0 is default");
DEFINE_validator(mode, &ValidateMode);

void initFlags(int argc, char *argv[]) {
    gflags::SetVersionString(g_version);
    gflags::SetUsageMessage(g_help);

    gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    if (FLAGS_help || FLAGS_h) {
        FLAGS_help = false;
        FLAGS_helpshort = true;
    }
    gflags::HandleCommandLineHelpFlags();
}

void initLogger(char *argv[]) {
    FLAGS_log_dir = "./";

    FLAGS_alsologtostderr = false;

    google::InitGoogleLogging(argv[0]);

    google::SetLogDestination(google::INFO, "snake_info");
    google::SetLogDestination(google::WARNING, "snake_warning");
    google::SetLogDestination(google::ERROR, "snake_error");
    google::SetLogFilenameExtension(".log");

    google::SetVersionString(g_version);
    google::SetUsageMessage(g_help);

    FLAGS_minloglevel = 0; // 0:INFO, 1:WARNING, 2:ERROR
    FLAGS_v = -1;          // silent VLOG(0)
}

void deinitLogger() {

    google::FlushLogFiles(google::INFO);
    google::FlushLogFiles(google::WARNING);
    google::FlushLogFiles(google::ERROR);

    google::ShutdownGoogleLogging();
}

int main(int argc, char *argv[]) {
    int result = 0;

    initFlags(argc, argv);
    initLogger(argv);

    AppConfig::Get().setAppRunMode(AppRunMode(FLAGS_mode));
    AppConfig::Get().initAppConfig();

    auto mode = AppConfig::RunMode();
    LOG(INFO) << "App run mode = " << mode.description();
    fmt::print("App run mode = {}\n", mode.description());

    if (mode.isHumanMode() || mode.isAIMode()) {
        result = SnakeApp().start();
    }

    if (mode.isTrainMode()) {

        if (g_isDebugBinary) {
            char confirm;
            do {
                fmt::print("This binary compiled in Debug mode, still training? [y/n]\n");
                std::cin >> confirm;
            } while (!std::cin.fail() && confirm != 'y' && confirm != 'n');

            if ('y' == confirm) {
                result = TrainApp().start();
            } else {
                fmt::print("Bye\n");
                return 0;
            }

        } else {
            result = TrainApp().start();
        }
    }

    deinitLogger();

    return result;
}
