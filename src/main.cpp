#include <protobot/protobot.h>
#include <protobot/config.h>
#include <protobot/client.h>
#include <sstream>
#include <algorithm>
#include <exception>
#include <CLI/CLI.hpp>
#include <rang.hpp>


#include <csignal>
void signal_handler(int s) {
    std::cout << std::endl << rang::style::reset << rang::fg::red << rang::style::bold;
    std::cout << "Control-C detected, exiting..." << rang::style::reset << std::endl;
    std::exit(1);
}






using namespace protobot;

int main(int argc, char const *argv[])
{
    // nice ctrl+c handling
    struct sigaction sigIntHandler{};
    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);

    // application parameters
    std::string config_file{"config/config.json"};
    std::string log_file{"logs/protobot.log"};
    std::string modules_path{"modules"};
    bool dev_mode{false};

    // add command line options
    CLI::App app("ProtoBot Discord Bot");
    app.set_version_flag("--version", "0.420.69");
    app.add_option("-m,--modules-path,modules-path", modules_path, "Folder containing the bot modules");
    app.add_option("-c,--config,config", config_file, "JSON config file");
    app.add_option("-l,--logfile,logfile", log_file, "Log output file");
    app.add_option("-d,--dev,dev", dev_mode, "Developer mode");
    CLI11_PARSE(app, argc, argv);


    // read config file
    config cfg{config_file};
    cfg.set_modules_path(modules_path);

    // setup bot client
    uint32_t intents = dpp::i_default_intents;
    intents |= dpp::i_message_content;
    dpp::cluster core(cfg.token(), intents);

    // create database connection
    database db{cfg.database_host(),
                cfg.database_username(),
                cfg.database_password(),
                cfg.database_db_name()};

    // create bot client
    client bot(&core, &cfg, &db, dev_mode);

    // setup spdlog logger
    std::shared_ptr<spdlog::logger> log;
    spdlog::init_thread_pool(8192, 2);
    std::vector<spdlog::sink_ptr> sinks;
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file, 1024 * 1024 * 5, 10);
    sinks.push_back(stdout_sink);
    sinks.push_back(rotating);
    log = std::make_shared<spdlog::async_logger>("test", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(log);
    log->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [th#%t]%$ : %v");
    log->set_level(spdlog::level::level_enum::debug);

    // integrate spdlog logger to D++ log events
    core.on_log([&log](const dpp::log_t & event) {
        switch (event.severity) {
            case dpp::ll_trace:
                log->trace("{}", event.message);
                break;
            case dpp::ll_debug:
                log->debug("{}", event.message);
                break;
            case dpp::ll_info:
                log->info("{}", event.message);
                break;
            case dpp::ll_warning:
                log->warn("{}", event.message);
                break;
            case dpp::ll_error:
                log->error("{}", event.message);
                break;
            case dpp::ll_critical:
            default:
                log->critical("{}", event.message);
                break;
        }
    });

    if(dev_mode) {
        bot.log(dpp::ll_info, "=== RUNNING IN DEVELOPER MODE ===");
    }

    // load client
    bot.load_modules();
    bot.bind_events();

#ifdef ANGELSCRIPT_INTEGRATION
    bot.load_as_engine();
#endif

    try {
        // connect and start the event loop
        core.start(false);
    }
    catch (std::exception &e) {
        core.log(dpp::ll_error, fmt::format("Oh no! {} :<", e.what()));
    }

    return 0;
}
