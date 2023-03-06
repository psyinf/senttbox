#pragma once
#include <entt/entt.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
class Core
{
    struct logging
    {
        std::string               file_name             = "log.h";
        spdlog::level::level_enum default_level_console = spdlog::level::info;
        spdlog::level::level_enum default_level_file    = spdlog::level::info;
    };

public:
    Core()
    {
        setupLogging(logging());

        spdlog::info("Core starting ...");
    }
    ~Core() 
    {
        spdlog::info("Core shutting down ...");
    }

private:
    void setupLogging(const logging& logging_cfg) const
    {

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink    = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logging_cfg.file_name);

        console_sink->set_level(logging_cfg.default_level_console);
        file_sink->set_level(logging_cfg.default_level_file);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        auto                          logger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());

        spdlog::set_default_logger(logger);
    }

private:
    
};


int main(int argc, char** argv)
{
    Core core;
    entt::registry registry;


    return 0;
}