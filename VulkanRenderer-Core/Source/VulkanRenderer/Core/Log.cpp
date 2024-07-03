#include <vkrenpch.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "VulkanRenderer/Core/Log.h"

namespace vkren {

  std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
  std::shared_ptr<spdlog::logger> Log::s_ImplLogger;

  void Log::Init()
  {
    std::vector<spdlog::sink_ptr> log_sinks;
    log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("VulkanRenderer.log", true));

    log_sinks[0]->set_pattern("%^[%T] %n %v%$");
    log_sinks[1]->set_pattern("[%T] [%l] %n %v");

    s_CoreLogger = std::make_shared<spdlog::logger>("[CORE]", begin(log_sinks), end(log_sinks));
    spdlog::register_logger(s_CoreLogger);
    s_CoreLogger->set_level(spdlog::level::trace);
    s_CoreLogger->flush_on(spdlog::level::trace);

    s_ImplLogger = std::make_shared<spdlog::logger>("[IMPL]", begin(log_sinks), end(log_sinks));
    spdlog::register_logger(s_ImplLogger);
    s_ImplLogger->set_level(spdlog::level::trace);
    s_ImplLogger->flush_on(spdlog::level::trace);
  }

}